#ifndef TODO_WIDGET_HH
#define TODO_WIDGET_HH

#include <ncurses.h>
#include <list>

namespace td_utils {

  class todo_widget {
    public:
      todo_widget(todo_widget * parent);
      virtual ~todo_widget();

      virtual int callback(int input);

      virtual int print(WINDOW * win) = 0;

      virtual void set_focus(todo_widget * widget);
      virtual todo_widget * get_focus();
      virtual void return_focus();

    protected:
      todo_widget * m_parent; //TODO remove
      todo_widget * m_focus;

  }; // class todo_widget

}; // namespace td_utils

#endif // #ifndef TODO_WIDGET
