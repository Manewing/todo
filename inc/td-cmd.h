#ifndef TODO_CMD_HH
#define TODO_CMD_HH

#include <td-gui.h>
#include <td-list.h>

namespace td_utils {

  void execute_cmdline(const std::string & cmdline, todo_gui * gui,
      todo_list * list);

  typedef struct {
    unsigned int * cmd_list;
    unsigned int cmd_size;
    unsigned int cmd_index;
    int (*execute)(todo_gui * gui, todo_list * list, int input);
  } todo_cmd;

  ///< delete item command = "dd"
  extern todo_cmd delete_item_cmd;
  ///< undo last command = "uu"
  extern todo_cmd undo_cmd;
  ///< set item state = "s{d,t,w}"
  extern todo_cmd set_item_cmd;

  /**
   * @brief checks if new input equals command input in list
   * @param[in]     input - new keyboard input
   * @param[in/out] gui   - pointer to todo gui
   * @param[in/out] list  - pointer to todo list
   */
  void command_check_list(int input, todo_gui * gui, todo_list * list);

  /**
   * @brief start command by adding it to list
   * @param[in/out] cmd - pointer to the command to be started
   */
  void command_start(todo_cmd * cmd);

  /**
   * @brief checks if new input equals command input
   * @param[in/out] cmd   - command to check
   * @param[in]     input - new keyboard input
   * @param[in/out] gui   - pointer to todo gui
   * @param[in/out] list  - pointer to todo list
   * @return NULL if commands is executed
   */
  int command_check(todo_cmd * cmd, int input, todo_gui * gui, todo_list * list);

  /**
   * @brief resets command list after command execution
   */
  void command_reset();

}; // namespace td_utils

#endif // #ifndef TODO_CMD_HH
