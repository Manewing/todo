#include "td-widget.h"

#include "td-defs.h"
#include "td-except.h"

namespace todo {

  void widget_fu::handle(widget * handler) const {
    widget::static_log_debug("focus update", "handler: %p", handler);
    handler->set_focus(handler);
  }

  int widget::access_counter = 0;
  std::ofstream widget::log_file;

  widget::widget():
    m_focus(NULL),
    m_fu() {
    if(access_counter++ == 0) {
      log_file.open( ::todo::utils::get_path(
              widget::log_file_name, ::todo::utils::get_homedir()
            ), std::ios_base::out);
    }
  }

  widget::~widget() {
    if(--access_counter == 0) {
      log_file.close();
    }
  }

  void widget::set_focus(widget * w) {
    m_focus = (w == NULL ? this : w);
    if (w) {
      try {
        callback(CMDK_TRIGGERED);
      } catch (exception * except) {
        except->handle(this);
      }
    }
  }

  void widget::return_focus() {
    m_focus = NULL;
    throw &m_fu;
  }

  void widget::call_focus(int input) {
    widget::log_debug("widget", "to m_focus->callback(%x)", input);
    try {
      m_focus->callback(input);
    } catch (exception * except) {
      except->handle(this);
    }
  }

  void widget::callback(int input) {
    if (m_focus == NULL)
      m_focus = this;
    if (m_focus == this) {
      widget::log_debug("widget", "callback -> callback_handler (%x)", input);
      this->callback_handler(input);
    } else {
      call_focus(input);
    }
  }

  void widget::log(const char* type, std::string id, std::string msg) {
    widget::log_file << "[" << type << "]" << "(" << id << "=" << this << "): "
                     << msg << std::endl;
  }

  void widget::static_log(const char* type, std::string id, std::string msg) {
    widget::log_file << "[" << type << "]" << "(" << id << "): "
                     << msg << std::endl;
  }

}; // namespace todo
