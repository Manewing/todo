#ifndef TODO_UPDATE_HH
#define TODO_UPDATE_HH

#include <list>

#include "td-except.h"

namespace todo {

  class update_if {
    public:
      update_if();
      virtual ~update_if();
      virtual void set_update(exception * except);
      virtual void update();

    protected:
      exception * m_update_except;
  };

  class urecall_exception : public exception {
    public:
      urecall_exception(widget * notifier);
      virtual ~urecall_exception();
      virtual void process(widget * handler);
  };

}; // namespace todo

#endif // #ifndef TODO_UPDATE_HH
