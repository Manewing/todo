#ifndef TODO_SHORTCUTS_HH
#define TODO_SHORTCUTS_HH

#include <td-gui.h>
#include <td-list.h>

namespace td_utils {


  /**
   * @brief todo shortcut
   */
  typedef struct {
    int * shc_list;          //< list of input characters for shortcut
    unsigned int shc_size;   //< size of shortcut list
    unsigned int shc_index;  //< current index in list
    //< function to exectute when shortcut is triggered
    int (*execute)(todo_gui * gui, todo_list * list, int input);
  } todo_shc;

  /**
   * @brief updates shortcuts if user input occurred
   * @param[in]     input - new user input
   * @param[in/out] gui   - pointer to todo gui
   * @param[in/out] list  - pointer to todo list
   */
  void shortcut_update(int input, todo_gui * gui, todo_list * list);

}; // namespace td_utils

#endif // #ifndef TODO_SHORTCUTS_HH
