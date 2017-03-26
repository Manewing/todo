#include "td-gui.h"

#include "td-except.h"
#include "td-cmd.h"
#include "td-shortcuts.h"

class gui_edit_submit_exception : public todo::exception {
  public:
    gui_edit_submit_exception(todo::edit * edit)
      : todo::exception(edit) {}
    virtual void process(todo::widget * handler) {
      // get gui and edit
      todo::gui * gui = dynamic_cast<todo::gui*>(handler);
      todo::edit * edit = dynamic_cast<todo::edit*>(m_notifier);
      td_utils::execute_cmdline(edit->get_text(), gui, &gui->lst());
      // edit has done its job hide it again
      edit->clear();
      edit->print(stdscr);
      edit->visible(false);
      //update gui
      gui->set_focus(NULL);
      gui->update();
    }
};

class gui_update_exception : public todo::exception {
  public:
    gui_update_exception(todo::widget * notifier)
      : todo::exception(notifier) {}
    virtual void process(todo::widget * handler) {
      todo::widget::static_log_debug("gui_update_exception", "print");
      m_notifier->print(stdscr);
    }
};

namespace todo {

  gui::gui():
    widget(),
    m_list(),
    m_quit(false),
    m_msg_u(),
    m_cmdline_edit(":") {

    //init colors
    start_color();
    use_default_colors(); //use terminal colors
    for(int i = 0; i < 8; i++)
      init_pair(i+1, i, -1);

    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    td_screen_pos_t pos = { 0, max_row-1 };
    td_screen_pos_t end = { max_col, max_row-1 };
    m_cmdline_edit.set_pos(pos);
    m_cmdline_edit.set_end(end);
    m_cmdline_edit.set_callback(new gui_edit_submit_exception(&m_cmdline_edit), CMDK_ENTER);
    m_cmdline_edit.set_callback(new gui_update_exception(&m_cmdline_edit), CMDK_TRIGGERED);
    m_cmdline_edit.visible(false);

    //update
    update();
  }

  gui::~gui() {
    endwin();
  }

  void gui::callback_handler(int input) {
    widget::log_debug("gui", "in callback_handler");
    switch(input) {
      case CMDK_START_CMD:
        set_focus(&m_cmdline_edit);
        m_cmdline_edit.visible(true);
        m_cmdline_edit.print(stdscr);
        break;
      case CMDK_EXIT:
        quit();
        break;
      case CMDK_ARROW_UP:
        m_list.select_prev();
        break;
      case CMDK_ARROW_DOWN:
        m_list.select_next();
        break;
      case CMDK_ENTER:
        m_list.expand_selected();
        break;
      case CMDK_EDIT:
        m_list.expand_selected(true);
        set_focus(m_list.get_selection());
      default:
        td_utils::shortcut_update(input, this, &m_list);
        break;
    }

    update();
  }

  void gui::return_focus() {
    /* nothing todo */
  }

  void gui::update() {
    print();
    print_msg(m_msg_u);
    m_msg_u = "";
    refresh();
  }

  void gui::print_header(WINDOW * win) {
    int cpos1 = 0, cpos2 = 0, col, max_col;
    getmaxyx(stdscr, col, max_col);

    //print top line
    wmove(win, 0, 0);
    for(col = 0; col < max_col; col++)
      waddch(win, '-');

    //print table
    mvwprintw(win, 1, 0, "| %n# ID   |   %nName    ", &cpos1, &cpos2);
    mvwprintw(win, 1, max_col - 20, "| %nPriority | %nState |", &cpos1, &cpos2);
  }

  int gui::print(WINDOW * win) {
    int row = 0, col = 0;
    //clear();

    print_header(win);

    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    //print items
    td_screen_pos_t top = {col, row + 2 },
                    bottom = { max_col, max_row-1 };
    m_list.set_pos(win, top, bottom);
    m_list.print(win);
    return 0;
  }

  void gui::print_msg(std::string msg) { //TODO colored?
    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);
    mvprintw(max_row-1, 0, "%s", msg.c_str());
  }

  void gui::quit() {
    m_quit = true;
  }

  bool gui::is_running() {
    return !m_quit;
  }

  void gui::init() {
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();
    set_escdelay(0);
  }

}; // namespace todo
