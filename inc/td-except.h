#ifndef TODO_EXCEPTION_HH
#define TODO_EXCEPTION_HH

#include "td-widget.h"

namespace td_utils {

  class todo_exception {
    public:
      todo_exception(todo_widget * notifier) : m_notifier(notifier) {}
      virtual ~todo_exception() {}
      virtual bool handle(todo_widget * handler) = 0;
    protected:
      todo_widget * m_notifier;
  }; //class todo_exception

}; // namespace td_utils

#endif // #ifndef TODO_EXCEPTION_HH
