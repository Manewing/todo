#include "td-frame.h"

namespace todo {

  frame::frame():
    widget(),
    m_win(NULL)
  {
    m_win = newwin(24, 80, 0, 0);
  }

  frame::~frame() {
    delwin(m_win);
  }

  void frame::set_pos(WINDOW * topwin, td_screen_pos_t top, td_screen_pos_t bottom) {
    int tmp;
    m_top = top;
    m_bottom = bottom;
    if(m_bottom.scr_x == -1) {
      getmaxyx(topwin, tmp, m_bottom.scr_x);
    }
    if(m_bottom.scr_y == -1) {
      getmaxyx(topwin, m_bottom.scr_y, tmp);
    }
    wresize(m_win, m_bottom.scr_y-m_top.scr_y, m_bottom.scr_x-m_top.scr_x);
    mvwin(m_win, m_top.scr_y, m_top.scr_x);
    wrefresh(m_win);
  }

  int frame::print(WINDOW * win) {
    (void)win;
    wborder(m_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(m_win);
    return m_bottom.scr_y - m_top.scr_y;
  }

  WINDOW * frame::get_win() {
    return m_win;
  }

}; // namespace todo
