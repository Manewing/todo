#ifndef TODO_SHORTCUTS_HH
#define TODO_SHORTCUTS_HH

#include <td-gui.h>
#include <td-list.h>

namespace td_utils {

#define FORWARD_INPUT 0xFF

#define SHORTCUT_FUNC(name) int exec_shortcut_ ## name (todo_gui * gui, todo_list * list, int input)
#define SHORTCUT(name, list, size) {(int[])list, size, 0, &exec_shortcut_ ## name }

  typedef struct {
    int * shc_list;          //< list of input characters for shortcut
    unsigned int shc_size;   //< size of shortcut list
    unsigned int shc_index;  //< current index in list
    int (*execute)(todo_gui * gui, todo_list * list, int input);
  } todo_shc;

  int shortcut_update(int input, todo_gui * gui, todo_list * list);

}; // namespace td_utils

#endif // #ifndef TODO_SHORTCUTS_HH
