#ifndef TODO_EXCEPTION_HH
#define TODO_EXCEPTION_HH

#include <utility>

namespace todo {

  class widget;

  ///< normal exception
  class exception {
    public:

      exception():
        m_notifier() {
      }

      virtual ~exception() {
      }

      exception& notifier(widget* notifier) {
        m_notifier = notifier;
        return *this;
      }

      widget* notifier() const {
        return m_notifier;
      }

      virtual void handle(widget * handler) const {
      }

    protected:
      widget * m_notifier;

  }; // class exception

}; // namespace todo

#endif // #ifndef TODO_EXCEPTION_HH
