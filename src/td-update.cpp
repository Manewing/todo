#include "td-update.h"
#include "td-widget.h"

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

  void urecall_exception::handle(widget * handler) const {
    update_if * uif = dynamic_cast<update_if*>(handler);
    uif->update();
  }
};
