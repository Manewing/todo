#ifndef TODO_CMD_HH
#define TODO_CMD_HH

#include <td-gui.h>
#include <td-list.h>

namespace td_utils {

#define UNDO_FUNCTION(name) int undo_function_ ## name (todo_gui * gui, todo_list * list)

  typedef int (*undo_function)(todo_gui * gui, todo_list * list);
  extern std::list<undo_function> g_executed;

#define CMDLINE_FUNC(func) int cmdline_ ## func (void * params[])
#define CMDLINE_CMD(func, params, help) {#func, &cmdline_ ## func, params, help}

  //TODO add aliases for commands
  typedef struct {
    const char * cmdline_cmd;                //< the actual command
    int (*cmdline_execute)(void * params[]); //< function to execute
    const char * cmdline_args;               //< defines argument list of cmd
    const char * cmdline_doc;                //< documentation of command
  } todo_cmdline;

  void execute_cmdline(const std::string & cmdline, todo_gui * gui,
      todo_list * list);

}; // namespace td_utils

#endif // #ifndef TODO_CMD_HH
