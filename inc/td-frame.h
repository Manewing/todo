#ifndef TODO_FRAME_HH
#define TODO_FRAME_HH

#include "td-widget.h"
#include "td-defs.h"

#include <list>

namespace td_utils {

  class todo_frame : public todo_widget {
    public:
      todo_frame(todo_widget * parent,
                 WINDOW * topwin = stdscr);
      virtual ~todo_frame();

      //bottom -1, -1 => auto scale
      virtual void set_pos(WINDOW * topwin, td_screen_pos_t top, td_screen_pos_t bottom);

      virtual int print(WINDOW * win);

    private:
      todo_frame(const todo_frame&);
      void operator = (const todo_frame&);

    protected:
      WINDOW * m_win;
      td_screen_pos_t m_top;
      td_screen_pos_t m_bottom;

  }; // class todo_frame
}; // namespace td_utils

#endif
