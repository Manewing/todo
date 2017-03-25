#ifndef TODO_FRAME_HH
#define TODO_FRAME_HH

#include "td-widget.h"
#include "td-defs.h"

#include <list>

namespace todo {

  class frame : public widget {
    public:
      frame();
      virtual ~frame();

      //bottom -1, -1 => auto scale
      virtual void set_pos(WINDOW * topwin, td_screen_pos_t top, td_screen_pos_t bottom);

      virtual int print(WINDOW * win);

      WINDOW * get_win();

    private:
      frame(const frame&);
      void operator = (const frame&);

    protected:
      WINDOW * m_win;
      td_screen_pos_t m_top;
      td_screen_pos_t m_bottom;

  }; // class frame
}; // namespace todo

#endif
