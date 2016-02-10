#include <td-gui.h>


namespace td_utils {

  todo_gui::todo_gui(todo_list &list)
          : m_scroll(0), m_focus(NO_FOCUS),
            m_list(list), m_msg_u(),
            m_cmdline(":"), m_cmdline_pos(1),
            m_cmdline_history(), m_cmdline_histptr(m_cmdline_history.end()) {
    // ncurses init
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();
    set_escdelay(0);

    //init colors
    start_color();
    use_default_colors(); //use terminal colors
    for(int i = 0; i < 8; i++)
      init_pair(i+1, i, -1);

    //update
    update();
  }

  todo_gui::~todo_gui() {
    endwin();
  }

  void todo_gui::update() {
    getmaxyx(stdscr, m_row, m_col);
    print();
    print_msg(m_msg_u);
    m_msg_u = "";
    refresh();
  }

#define HEADER_OFFSET 5
#define BOTTOM_OFFSET 1

  void todo_gui::print() {
    int row = 0, col = 0;
    clear();

    //print items
    m_list.print(row + HEADER_OFFSET + m_scroll, col, m_col, m_row - BOTTOM_OFFSET);

    //draw header
    char filler[HEADER_OFFSET+1] = { ' ', '_', ' ', '-', ' ', ' ' };
    int fillpos[HEADER_OFFSET+1] = {  0,   1,   2,   3,   4,   m_row-1 };
    for(int y = 0; y < HEADER_OFFSET+1; y++) {
      move(fillpos[y], 0);
      for(int x = 0; x < m_col; x++) {
        addch(filler[y]);
      }
    }
    int cpos1 = 0, cpos2 = 0;
    mvprintw(0, (m_col-TD_HEADER_SIZE)/2, "%s", TD_HEADER_STR);
    mvprintw(2, col, "| %n# ID   |   %nName   |", &cpos1, &cpos2);
    mvprintw(2, m_col - 20, "| %nPriority | %nState |", &cpos1, &cpos2); //TODO dynamic
    curs_set(0); //hide curser
  }

  void todo_gui::scroll_down() {
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
  }

  void todo_gui::print_msg(std::string msg) { //TODO colored?
    mvprintw(m_row-1, 0, "%s", msg.c_str());
    refresh(); // no need to update everything
  }

  void todo_gui::start_cmdline() {
    update();
    m_cmdline_pos = m_cmdline.length();
    print_msg(m_cmdline);
    curs_set(1);
  }

  void todo_gui::add_cmdline(char ch) {
    std::string str = "";
    str += ch;
    m_cmdline.insert(m_cmdline_pos, str);
    m_cmdline_pos++;
    print_msg(m_cmdline);
    print_msg(m_cmdline.substr(0, m_cmdline_pos));
    m_cmdline_histptr = m_cmdline_history.end();
    curs_set(1);
  }

  void todo_gui::bsp_cmdline() {
    if(m_cmdline.length() <= 1) //dont delete ':'
      return;
    m_cmdline.erase(m_cmdline_pos-1, 1);
    m_cmdline_pos--;
    print_msg(m_cmdline + " ");
    print_msg(m_cmdline.substr(0, m_cmdline_pos));
    m_cmdline_histptr = m_cmdline_history.end();
    curs_set(1);
  }

  void todo_gui::clear_cmdline() {
    m_cmdline_pos = 1;
    m_cmdline = ":";
    update();
  }

  void todo_gui::cleft_cmdline() {
    if(m_cmdline_pos > 1) {
      m_cmdline_pos--;
      print_msg(m_cmdline.substr(0, m_cmdline_pos)); //update curser
      curs_set(1);
    }
  }

  void todo_gui::cright_cmdline() {
    if(m_cmdline_pos < m_cmdline.length()) {
      m_cmdline_pos++;
      print_msg(m_cmdline.substr(0, m_cmdline_pos)); //update curser
      curs_set(1);
    }
  }

  void todo_gui::add_cmdline_history() {
    m_cmdline_history.push_back(m_cmdline);
    m_cmdline_histptr = m_cmdline_history.end();
  }

  void todo_gui::get_prev_cmdline() {
    if(m_cmdline_history.empty())
      return;
    if(m_cmdline_histptr != m_cmdline_history.begin())
      m_cmdline_histptr--;
    m_cmdline = *m_cmdline_histptr;
    start_cmdline();
  }

  void todo_gui::get_next_cmdline() {
    if(m_cmdline_history.empty())
      return;
    if(m_cmdline_histptr == m_cmdline_history.end()
        || m_cmdline_histptr == --m_cmdline_history.end())
      m_cmdline = ":";
    else
      m_cmdline = *(++m_cmdline_histptr);
    start_cmdline();
  }

  std::string todo_gui::get_cmdline() {
    return m_cmdline.substr(1, m_cmdline.length());
  }

}; // namespace td_utils
