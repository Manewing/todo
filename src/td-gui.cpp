#include "td-gui.h"

#include "td-except.h"
#include "td-cmd.h"
#include "td-shortcuts.h"

class gui_edit_submit_exception : public todo::exception {
  public:
    virtual void handle(todo::widget * handler) const {
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
    virtual void handle(todo::widget * handler) const {
      todo::widget::static_log_debug("gui_update_exception", "print");
      m_notifier->print(stdscr);
    }
};

namespace todo {

  gui_header::gui_header():
    frame() {
    /* nothing todo */
  }

  gui_header::~gui_header() {
    /* nothing todo */
  }

  int gui_header::print(WINDOW* win) {
    const int cols = m_bottom.scr_x - m_top.scr_x;
    mvwprintw(m_win, 1, 0, "  # ID   |   Name    ");
    mvwprintw(m_win, 1, cols - 20, "| Priority | State");
    return frame::print(win);
  }

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
    m_cmdline_edit.set_callback(new gui_edit_submit_exception, CMDK_ENTER);
    m_cmdline_edit.set_callback(new gui_update_exception, CMDK_TRIGGERED);
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
    m_focus = NULL;
  }

  void gui::update() {
    print();
    print_msg(m_msg_u);
    m_msg_u = "";
    refresh();
  }

  int gui::print(WINDOW * win) {
    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    // print header
    m_header.set_pos(win, {0, 0}, {-1, 3});
    m_header.print(win);

    //print items
    m_list.set_pos(win, {0, 2}, { -1, max_row - 1});
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
