#ifndef TODO_WIDGET_HH
#define TODO_WIDGET_HH

#include <ncurses.h>
#include <fstream>

#include "td-except.h"

namespace todo {

  class widget_fu : public exception {
    public:
      virtual void handle(widget* handler) const;
  };

  class widget {
    public:
      widget();
      virtual ~widget();

      virtual void callback_handler(int input) {};

      virtual int print(WINDOW * win) = 0;

      virtual void set_focus(widget * widget);
      virtual widget * get_focus();
      virtual void return_focus();
      virtual void call_focus(int input);

      //TODO private
      virtual void callback(int input);

      //TODO change logging system
      void log(std::string type, std::string id, std::string msg);
      void log_error(std::string id, std::string msg);
      void log_debug(std::string id, std::string msg);

      static void static_log(std::string type, std::string id, std::string msg);
      static void static_log_error(std::string id, std::string msg);
      static void static_log_debug(std::string id, std::string msg);

    private:
      static std::ofstream log_file;

    protected:
      widget *  m_focus;
      widget_fu m_fu;

  }; // class widget

}; // namespace todo

#endif // #ifndef TODO_WIDGET
