#include "td-frame.h"

namespace td_utils {

  todo_frame::todo_frame(todo_widget * parent,
                         WINDOW * topwin)
            : todo_widget(parent), m_win(NULL) {
    m_win = derwin(topwin, 24, 80, 0, 0);
  }

  todo_frame::~todo_frame() {
    delwin(m_win);
  }

  void todo_frame::set_pos(WINDOW * topwin, td_screen_pos_t top, td_screen_pos_t bottom) {
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
    refresh();
  }

  int todo_frame::print(WINDOW * win) {
    (void)win;
    wborder(m_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(m_win);
    return m_bottom.scr_y - m_top.scr_y;
  }

}; // namespace td_utils
