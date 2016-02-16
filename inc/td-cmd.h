#ifndef TODO_CMD_HH
#define TODO_CMD_HH

#include <td-gui.h>
#include <td-list.h>

namespace td_utils {

//< macro to define a undo function
#define UNDO_FUNCTION(name) int undo_function_ ## name (todo_gui * gui, todo_list * list)

  //< typedef of undo function
  typedef int (*undo_function)(todo_gui * gui, todo_list * list);

  //< list of undo function from exectued commands
  extern std::list<undo_function> g_executed;


  /**
   * @brief todo command line
   */
  //TODO add aliases for commands
  typedef struct {
    const char * cmdline_cmd;                //< the actual command
    int (*cmdline_execute)(void * params[]); //< function to execute
    const char * cmdline_args;               //< defines argument list of cmd
    const char * cmdline_doc;                //< documentation of command
  } todo_cmdline;

  /**
   * @brief executes command line string
   * @param[in]     cmdline - command line string to execute
   * @param[in/out] gui     - pointer to the todo gui
   * @param[in/out] list    - pointer to the todo list
   */
  void execute_cmdline(const std::string & cmdline, todo_gui * gui,
      todo_list * list);

}; // namespace td_utils

#endif // #ifndef TODO_CMD_HH
