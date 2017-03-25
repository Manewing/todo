#include "td-update.h"

namespace todo {

  update_if::update_if() : m_update_except(NULL) {}

  update_if::~update_if() {
    delete m_update_except;
  }

  void update_if::set_update(exception * except) {
    if(m_update_except != except && m_update_except)
      delete m_update_except;
    m_update_except = except;
  }

  void update_if::update() {
    if(m_update_except)
      throw m_update_except;
  }

  urecall_exception::urecall_exception(widget * notifier):
    exception(notifier) {}

  urecall_exception::~urecall_exception() {}

  void urecall_exception::process(widget * handler) {
    update_if * uif = dynamic_cast<update_if*>(handler);
    uif->update();
  }
};
