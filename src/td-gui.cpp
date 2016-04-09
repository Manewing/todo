#include "td-gui.h"

#include "td-except.h"
#include "td-cmd.h"


namespace td_utils {

  class gui_edit_submit_exception : public todo_exception {
    public:
      gui_edit_submit_exception(todo_edit * edit)
        : todo_exception(edit) {}
      virtual bool handle(todo_widget * handler) {
        // get gui and edit
        todo_gui * gui = dynamic_cast<todo_gui*>(handler);
        todo_edit * edit = dynamic_cast<todo_edit*>(m_notifier);
        execute_cmdline(edit->get_text(), gui, &gui->m_list);
        // edit has done its job hide it again
        edit->clear();
        edit->print(stdscr);
        edit->visible(false);
        //update gui
        gui->set_focus(NULL);
        gui->update();
        return true;
      }
  };

  class gui_edit_update_exception : public todo_exception {
    public:
      gui_edit_update_exception(todo_edit * edit)
        : todo_exception(edit) {}
      virtual bool handle(todo_widget * handler) {
        (void)handler;
        m_notifier->print(stdscr);
        return true;
      }
  };

  todo_gui::todo_gui()
          : todo_widget(NULL), m_quit(false), m_scroll(0),
            m_list(this), m_msg_u(),
            m_cmdline_edit(this, ":") {
    //init colors
    start_color();
    use_default_colors(); //use terminal colors
    for(int i = 0; i < 8; i++)
      init_pair(i+1, i, -1);

    //update
    update();

    td_screen_pos_t pos = { 0, m_row-1 };
    td_screen_pos_t end = { m_col, m_row-1 };
    m_cmdline_edit.set_pos(pos);
    m_cmdline_edit.set_end(end);
    m_cmdline_edit.set_callback(new gui_edit_submit_exception(&m_cmdline_edit), CMDK_ENTER);
    m_cmdline_edit.set_callback(new gui_edit_update_exception(&m_cmdline_edit), CMDK_TRIGGERED);
    m_cmdline_edit.visible(false);
  }

  todo_gui::~todo_gui() {
    endwin();
  }

  int todo_gui::callback(int input) {
    if(todo_widget::callback(input) == 0) {
      switch(input) {
        case CMDK_START_CMD:
          set_focus(&m_cmdline_edit);
          m_cmdline_edit.visible(true);
          m_cmdline_edit.print(stdscr);
          break;
        case CMDK_EXIT:
          quit();
          break;
        case CMDK_EDIT:
          set_focus(&m_list);
        default:
          m_list.callback(input);
          break;
      }
    }
    return 0;
  }

  void todo_gui::update() {
    getmaxyx(stdscr, m_row, m_col);
    print();
    print_msg(m_msg_u);
    m_msg_u = "";
    refresh();
  }

  int todo_gui::print(WINDOW * win) {
    int row = 0, col = 0;
    clear();

    //draw top line
    wmove(win, row, col);
    for(col = 0; col < m_col; col++)
      waddch(win, '-');

    int cpos1 = 0, cpos2 = 0;
    col = 0;
    mvwprintw(win, 1, col, "| %n# ID   |   %nName    ", &cpos1, &cpos2);
    mvwprintw(win, 1, m_col - 20, "| %nPriority | %nState |", &cpos1, &cpos2);

    //print items
    td_screen_pos_t top = {col, row + 2 + m_scroll},
                    bottom = { m_col, m_row-1 };
    m_list.set_pos(win, top, bottom);
    m_list.print(win);

    curs_set(0); //hide curser
    return 0;
  }

  /*void todo_gui::scroll_down() {
    int bottom_row = m_list.select_next();
    if(bottom_row + 1 + BOTTOM_OFFSET >= m_row)
      m_scroll -= (bottom_row + 1 + BOTTOM_OFFSET) - m_row;
    update();
  }

  void todo_gui::scroll_up() {
    int top_row = m_list.select_prev();
    if(top_row < HEADER_OFFSET)
      m_scroll += HEADER_OFFSET - top_row;
    update();
  }

  void todo_gui::expand_item() {
    m_list.expand_selected();
    todo_item * item = m_list.get_selection();
    update();
    if(item) {
      int bottom_row = item->get_bottom_row();
      if(bottom_row + 1 >= m_row)
        m_scroll -= (bottom_row + 1) - m_row;
    }
    update();
  }*/

  void todo_gui::print_msg(std::string msg) { //TODO colored?
    mvprintw(m_row-1, 0, "%s", msg.c_str());
  }

  void todo_gui::quit() {
    m_quit = true;
  }

  bool todo_gui::is_running() {
    return !m_quit;
  }

}; // namespace td_utils
