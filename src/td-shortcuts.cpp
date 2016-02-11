#include <td-cmd.h>
#include <td-shortcuts.h>

#include <assert.h>
#include <list>

namespace td_utils {

  SHORTCUT_FUNC(undo);
  SHORTCUT_FUNC(del);
  UNDO_FUNCTION(del);
  SHORTCUT_FUNC(set_item);

#define P99_PROTECT(...) __VA_ARGS__
  todo_shc shortcuts[] = {
    //< undo last command = "uu"
    SHORTCUT(undo, P99_PROTECT({ 0x75 }), 1),
    //< delete selected item command = "dd"
    SHORTCUT(del, P99_PROTECT({ 0x64, 0x64 }), 2),
    //< set selected item state = "s{d,t,w}"
    SHORTCUT(set_item, P99_PROTECT({ 0x73, FORWARD_INPUT }), 2)
  };

  const unsigned int shortcut_count = sizeof(shortcuts) / sizeof(shortcuts[0]);

  int shortcut_update(int input, todo_gui * gui, todo_list * list) {
    assert(gui && list);
    for(unsigned int l = 0; l < shortcut_count; l++) {
      if(shortcuts[l].shc_list[shortcuts[l].shc_index] == input ||
          shortcuts[l].shc_list[shortcuts[l].shc_index] == FORWARD_INPUT) {
        // new keyboard input did fit, increase index
        if(++shortcuts[l].shc_index == shortcuts[l].shc_size) {
          // execute shortcut function
          shortcuts[l].execute(gui, list, input);
          for(unsigned int m = 0; m < shortcut_count; m++)
            shortcuts[m].shc_index = 0;
        }
      } else {
        // new keyboard input did not fit, reset index
        shortcuts[l].shc_index = 0;
      }
    }
    return 0;
  }

  int exec_shortcut_del(todo_gui * gui,
      todo_list * list, int input __attribute__((unused)) ) {
    assert(gui && list);
    todo_item * item = list->get_selection();
    if(item) {
      list->remove_item(list->get_selection());
      g_executed.push_back(&undo_function_del);
      gui->update();
      gui->print_msg("Removed item...");
    } else {
      gui->print_msg("No item selected...");
    }
    return 0;
  }

  int undo_function_del(todo_gui * gui, todo_list * list) {
    assert(gui && list);
    list->undo_remove();
    gui->update();
    gui->print_msg("Restored item...");
    return 0;
  }

  int exec_shortcut_undo(todo_gui * gui,
      todo_list * list, int input __attribute__((unused)) ) {
    assert(gui && list);
    if(g_executed.empty()) {
      gui->print_msg("Can not undo more...");
      return 1;
    }
    undo_function u_f = g_executed.back();
    if(u_f) {
      // call undo function of last executed command
      u_f(gui, list);
      //remove last executed command from the list
      g_executed.pop_back();
    } else {
      gui->print_msg("ERROR: Cannot undo last command!");
      return -1;
    }
    return 0;
  }

  int exec_shortcut_set_item(todo_gui * gui, todo_list * list, int input) {
    assert(gui && list);
    todo_item * item = list->get_selection();
    switch(input) {
      case 0x64: //'d'
        item->set_state(todo_item::DONE);
        gui->print_msg_u("Set item to DONE");
        break;
      case 0x74: //'t'
        item->set_state(todo_item::TODO);
        gui->print_msg_u("Set item to TODO");
        break;
      case 0x77: //'w'
        item->set_state(todo_item::WORK_IN_PRG);
        gui->print_msg_u("Set item to WORK IN PROGRESS");
        break;
      default:
        gui->print_msg_u("Invalid command...");
        return -1;
    }
    list->sort();
    gui->update();
    return 0;
  }

}; // namespace td_utils
