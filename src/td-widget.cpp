#include "td-widget.h"

#include "td-defs.h"
#include "td-except.h"

namespace td_utils {

  class todo_widget_focus_update : public todo_exception {
    public:
      todo_widget_focus_update(todo_widget * notifier = NULL)
        : todo_exception(notifier) {}
      virtual bool handle(todo_widget * handler) {
        handler->set_focus(NULL);
        return false;
      }
  };

  todo_widget::todo_widget(todo_widget * parent)
             : m_parent(parent), m_focus(NULL) {
  }

  todo_widget::~todo_widget() {
  }

  int todo_widget::callback(int input) {
    if(m_focus) {
      try {
        m_focus->callback(input);
        return 1;
      } catch(todo_exception * except) {
        if(except->handle(this) == false)
          delete except;
        if(m_focus == NULL) // focus has changed
          return 1;
      }
    }
    return 0;
  }

  void todo_widget::set_focus(todo_widget * widget) {
    m_focus = widget;
  }

  todo_widget * todo_widget::get_focus() {
    return m_focus;
  }

  void todo_widget::return_focus() {
    throw new todo_widget_focus_update();
  }

}; // namespace td_utils
