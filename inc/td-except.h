#ifndef TODO_EXCEPTION_HH
#define TODO_EXCEPTION_HH

#include "td-widget.h"

namespace todo {

  ///< normal exception
  class exception {
    public:
      exception(widget * notifier) : m_notifier(notifier) {}
      virtual ~exception() {}
      virtual void handle(widget * handler) {
        this->process(handler);
      }
      virtual void process(widget * handler) = 0;
    protected:
      widget * m_notifier;
  }; // class exception

  ///< single use exception
  class su_exception : public exception {
    private:
      static void clean_up(su_exception * except) { delete except; }
    public:
      su_exception(widget * notifier) : exception(notifier) {}
      virtual ~su_exception() {}
      virtual void handle(widget * handler) {
        exception::handle(handler);
        su_exception::clean_up(this);
      }
      virtual void process(widget * handler) = 0;
  }; // class su_exception

}; // namespace todo

#endif // #ifndef TODO_EXCEPTION_HH
