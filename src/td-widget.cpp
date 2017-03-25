#include "td-widget.h"

#include "td-defs.h"
#include "td-except.h"

#include <iostream>

//debug TODO remove
#include <sstream>

namespace todo {

  class widget_focus_update : public su_exception {
    public:
      widget_focus_update(widget * notifier = NULL)
        : su_exception(notifier) {}
      virtual void process(widget * handler) {
        widget::log_debug("focus update", " -");
        handler->set_focus(handler);
      }
  };


  static unsigned int access_counter = 0;
  std::ofstream widget::log_file;

  widget::widget() : m_focus(this) {
    if(access_counter++ == 0) {
      log_file.open(
        #ifdef TD_DEBUG
          "td.log",
       #else
          ".td.log", // TODO
       #endif
          std::ios_base::out);
    }
  }

  widget::~widget() {
    if(--access_counter == 0) {
      log_file.close();
    }
  }

  void widget::set_focus(widget * w) {
    m_focus = (w == NULL ? this : w);
  }

  widget * widget::get_focus() {
    return m_focus;
  }

  void widget::return_focus() {
    throw new widget_focus_update();
  }

  void widget::call_focus(int input) {
#ifdef TD_DEBUG
    std::stringstream ss;
    ss << "widget[" << this << "] (0x" << std::hex << input << ")";
    widget::log_debug(ss.str(), "to m_focus->callback");
#endif
    try {
      m_focus->callback(input);
    } catch(exception * except) {
      except->handle(this);
    }
  }

  void widget::callback(int input) {
    if(m_focus == this) {
#ifdef TD_DEBUG
      std::stringstream ss;
      ss << "widget[" << this << "] (0x" << std::hex << input << ")";
      widget::log_debug(ss.str(), "callback -> to callback_handler");
#endif
      this->callback_handler(input);
    } else {
      call_focus(input);
    }
  }

  void widget::log(std::string type, std::string id, std::string msg) {
    widget::log_file << "[" << type << "]" << "(" << id << "): "
                     << msg << std::endl;
  }

  void widget::log_error(std::string id, std::string msg) {
    widget::log("ERROR", id, msg);
  }

  void widget::log_debug(std::string id, std::string msg) {
    widget::log("DEBUG", id, msg);
  }

}; // namespace todo
