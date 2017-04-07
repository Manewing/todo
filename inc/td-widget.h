#ifndef TODO_WIDGET_HH
#define TODO_WIDGET_HH

#include <ncurses.h>
#include <fstream>

#include "td-except.h"
#include "td-utils.h"

namespace todo {

  class widget_fu : public exception {
    public:
      virtual void handle(widget* handler) const;
  };

  class widget {
    public:
      static constexpr const char* log_file_name = ".td.log";

    public:
      widget();
      virtual ~widget();

      virtual void callback_handler(int input) {
        (void)input;
        /* nothing todo */
      }

      virtual int print(WINDOW * win) = 0;

      virtual void    set_focus(widget * widget = NULL);
      inline widget*  get_focus() const { return m_focus; }
      inline bool     has_focus() const { return m_focus == this; }

      virtual void return_focus();
      virtual void call_focus(int input);
      virtual void callback(int input);

      template <typename ... Args>
      inline void log_error(std::string id,
                            const char* fmt,
                            Args ... args) {
        widget::log("ERROR", id, ::todo::utils::format(fmt, args ...));
      }

      template <typename ... Args>
      inline void log_debug(__dbg std::string id,
                            __dbg const char* fmt,
                            __dbg Args ... args) {
        #ifdef TD_DEBUG
        widget::log("DEBUG", id, ::todo::utils::format(fmt, args ...));
        #endif // #ifdef TD_DEBUG
      }

      template <typename ... Args>
      inline static void static_log_error(std::string id,
                                          const char* fmt,
                                          Args ... args) {
        widget::static_log("ERROR", id, ::todo::utils::format(fmt, args ...));
      }

      template <typename ... Args>
      inline static void static_log_debug(__dbg std::string id,
                                          __dbg const char* fmt,
                                          __dbg Args ... args) {
        #ifdef TD_DEBUG
        widget::static_log("DEBUG", id, ::todo::utils::format(fmt, args ...));
        #endif // #ifdef TD_DEBUG
      }

    private:
      void log(const char* type, std::string id, std::string msg);
      static void static_log(const char* type, std::string id, std::string msg);

    private:
      static int           access_counter;
      static std::ofstream log_file;

    protected:
      widget *  m_focus;
      widget_fu m_fu;

  }; // class widget

}; // namespace todo

#endif // #ifndef TODO_WIDGET
