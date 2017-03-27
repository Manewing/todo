#include "td-widget.h"

#include "td-defs.h"
#include "td-except.h"

#include <iostream>

//debug TODO remove
#include <sstream>

namespace todo {

  void widget_fu::handle(widget * handler) const {
    std::stringstream ss;
    ss << "handler: " << handler;
    widget::static_log_debug("focus update", ss.str());
    handler->set_focus(handler);
  }

  static unsigned int access_counter = 0;
  std::ofstream widget::log_file;

  widget::widget():
    m_focus(this),
    m_fu() {
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
    throw &m_fu;
  }

  void widget::call_focus(int input) {
#ifdef TD_DEBUG
    std::stringstream ss;
    ss << "to m_focus->callback(" << std::hex << input << ")";
    widget::log_debug("widget", ss.str());
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
      ss << "callback -> callback_handler(" << input << ")";
      widget::log_debug("widget", ss.str());
#endif
      this->callback_handler(input);
    } else {
      call_focus(input);
    }
  }

  void widget::log(std::string type, std::string id, std::string msg) {
    widget::log_file << "[" << type << "]" << "(" << id << "=" << this << "): "
                     << msg << std::endl;
  }

  void widget::log_error(std::string id, std::string msg) {
    widget::log("ERROR", id, msg);
  }

  void widget::log_debug(std::string id, std::string msg) {
    widget::log("DEBUG", id, msg);
  }

  void widget::static_log(std::string type, std::string id, std::string msg) {
    widget::log_file << "[" << type << "]" << "(" << id << "): "
                     << msg << std::endl;
  }

  void widget::static_log_error(std::string id, std::string msg) {
    widget::static_log("ERROR", id, msg);
  }

  void widget::static_log_debug(std::string id, std::string msg) {
    widget::static_log("DEBUG", id, msg);
  }

}; // namespace todo
