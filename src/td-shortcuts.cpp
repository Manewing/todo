#include <td-shortcuts.h>

#include <td-gui.h>

namespace {

  const int shortcut_undo_list[] = { 0x75 };
  void shortcut_undo(int input) {
    (void)input;
    auto& gui = ::todo::gui::get();
    if (!gui.lst().undo())
      gui.print_msg_u("Can not undo more...");
  }

  const int shortcut_del_list[] = { 0x64, 0x64 };
  void shortcut_del(int input) {
    (void)input;
    auto& gui = ::todo::gui::get();
    auto item = gui.lst().get_selection();
    if(item) {
      gui.lst().remove(item);
      gui.print_msg_u("Removed item...");
    } else {
      gui.print_msg_u("No item selected...");
    }
  }

  const int shortcut_set_item_list[] = { 0x73, ::todo::shortcut_handler::FWDINPUT };
  void shortcut_set_item(int input) {
    auto& gui = ::todo::gui::get();
    auto item = gui.lst().get_selection();
    switch(input) {
      case 0x64: //'d'
        item->set_state(::todo::item::DONE);
        gui.print_msg_u("Set item to DONE");
        break;
      case 0x74: //'t'
        item->set_state(todo::item::TODO);
        gui.print_msg_u("Set item to TODO");
        break;
      case 0x77: //'w'
        item->set_state(todo::item::WORK_IN_PRG);
        gui.print_msg_u("Set item to WORK IN PROGRESS");
        break;
      default:
        gui.print_msg_u("Invalid command...");
        break;
    }
    gui.lst().sort();
  }
};

namespace todo {

  #define _SHCL(name) shortcut_ ## name ##_list
  #define SHC(name) { _SHCL(name), (sizeof(_SHCL(name)) / sizeof(_SHCL(name)[0])), 0, &shortcut_ ##name }
  shortcut_handler shortcut_handler::m_instance = shortcut_handler::shortcuts_t({
    SHC(undo),
    SHC(del),
    SHC(set_item)
  });
  #undef _SHCL
  #undef SHC

  shortcut_handler::shortcut_handler(shortcuts_t && shcs):
    m_shortcuts(std::forward<shortcuts_t>(shcs)) {
    /* nothing todo */
  }

  shortcut_handler::~shortcut_handler() {
    /* nothing todo */
  }

  shortcut_handler& shortcut_handler::get() {
    return shortcut_handler::m_instance;
  }

  void shortcut_handler::update(int input) {

    for (auto& it : m_shortcuts) {

      // check if new input matches next entry in list
      if (it.list[it.idx] == input || it.list[it.idx] == FWDINPUT) {

        // input matches, check if end is reached
        if (++it.idx == it.size) {

          // end reached, execute function
          it.execute(input);

          // reset all shortcuts
          for (auto& _it : m_shortcuts)
            _it.idx = 0;
        }
      } else {

        // input did not match, reset the shortcut
        it.idx = 0;

      }
    }

  }

}; // namespace todo
