#include "td-gui.h"

#include "td-except.h"
#include "td-cmd.h"
#include "td-shortcuts.h"

class gui_edit_submit_exception : public todo::exception {
  public:
    virtual void handle(todo::widget * handler) const {
      // get gui and edit
      ::todo::gui * gui = dynamic_cast<::todo::gui*>(handler);
      ::todo::edit * edit = dynamic_cast<::todo::edit*>(m_notifier);
      ::todo::command_line::get().execute(edit->get_text());

      // edit has done its job hide it again
      edit->clear();
      edit->visible(false);

      //update gui
      gui->set_focus();
      gui->update();
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

  td_screen_pos_t gui::cursor_pos = {-1, -1};

  gui& gui::get() {
    if (m_instance == NULL) {

      // screen has to be initialized before any
      // new window can be created
      initscr();

      m_instance = new gui();
    }
    return *m_instance;
  }

  void gui::free() {
    endwin();
    delete m_instance;
    m_instance = NULL;
  }

  gui* gui::m_instance = NULL;

  gui::gui():
    widget(),
    m_list(),
    m_quit(false),
    m_msg_u(),
    m_cmdline_edit(":") {

    // set input to raw
    raw();
    keypad(stdscr, true);
    noecho();
    set_escdelay(0);

    //init colors
    start_color();

    // use terminal colors
    use_default_colors();
    for(int i = 0; i < 8; i++)
      init_pair(i+1, i, -1);

    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    td_screen_pos_t pos = { 0, max_row-1 };
    td_screen_pos_t end = { max_col, max_row-1 };
    m_cmdline_edit.set_pos(pos);
    m_cmdline_edit.set_end(end);
    m_cmdline_edit.set_callback(new gui_edit_submit_exception, CMDK_ENTER);
    m_cmdline_edit.visible(false);

    //update
    update();
  }

  gui::~gui() {
    /* nothing todo */
  }

  void gui::callback_handler(int input) {
    widget::log_debug("gui", "in callback_handler");
    switch(input) {
      case CMDK_TRIGGERED:
        // focus return to gui
        m_cmdline_edit.clear();
        m_cmdline_edit.visible(false);
        break;
      case CMDK_START_CMD:
        m_cmdline_edit.visible(true);
        set_focus(&m_cmdline_edit);
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
        ::todo::shortcut_handler::get().update(input);
        break;
    }

    update();
  }

  void gui::return_focus() {
    m_focus = this;
  }

  void gui::update() {
    print();
    refresh();
  }

  int gui::print(WINDOW * win) {
    curs_set(0);
    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    // print header
    m_header.set_pos(win, {0, 0}, {-1, 3});
    m_header.print(win);

    //print items
    m_list.set_pos(win, {0, 2}, { -1, max_row - 1});
    m_list.print(win);

    // clear last line
    curs_set(0);
    move(max_row-1, 0);
    clrtoeol();

    // print message
    if (m_msg_u.size()) {
      mvprintw(max_row-1, 0, "%s", m_msg_u.c_str());
      m_msg_u = "";
    }

    // print command line
    m_cmdline_edit.print(win);

    curs_set(1);
    move(gui::cursor_pos.scr_y, gui::cursor_pos.scr_x);
    gui::cursor_pos.scr_x = 0;
    gui::cursor_pos.scr_y = max_row - 1;

    return 0;
  }

}; // namespace todo
