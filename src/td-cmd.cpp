#include <td-cmd.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <list>

namespace td_utils {

  //< list of undo function from exectued commands
  std::list<undo_function> g_executed;

//< macros to define command line function and command
#define CMDLINE_FUNC(func) int cmdline_ ## func (void * params[])
#define CMDLINE_CMD(func, params, help) {#func, &cmdline_ ## func, params, help}

  //< all functions for command line commands
  CMDLINE_FUNC(help);
  CMDLINE_FUNC(s);
  CMDLINE_FUNC(w);
  CMDLINE_FUNC(sortby);
  CMDLINE_FUNC(add);
  CMDLINE_FUNC(q);

  //< all command line commands
  todo_cmdline cmdline_cmds[] = {
    CMDLINE_CMD(help, "g", "Displays this screen, help <cmd> for detailed information"),
    CMDLINE_CMD(s, "sgl", "Saves todo list to file"),
    CMDLINE_CMD(w, "sgl", "Save todo list to file"), //TODO make alias
    CMDLINE_CMD(sortby, "sl", "Select sort mode for todo list"),
    CMDLINE_CMD(add, "sl", "Adds item to todo list"),
    CMDLINE_CMD(q, "g", "Quit")
  };

  //< count of command line commands
  const unsigned int cmdline_cmds_count = sizeof(cmdline_cmds) / sizeof(cmdline_cmds[0]);

  static char * g_parse_str = NULL;
  static unsigned int g_parse_index = 0;
  static unsigned int g_parse_lm = 0;

  int check_in_parse_cmdline(char c, const char * str, unsigned int str_l) {
    assert(str && str_l);
    for(unsigned int l = 0; l < str_l; l++) {
      if(str[l] == c)
        return true;
    }
    return false;
  }

  char * parse_cmdline(const char * cmd, const char * delim, const char * cmb, int *valid) {
    assert(delim && cmb);
    *valid = 1;
    if(cmd) {
      free(g_parse_str);
      unsigned int cmd_length = strlen(cmd);
      unsigned int delim_length = strlen(delim);
      unsigned int cmb_length = strlen(cmb);
      assert(cmd_length);
      g_parse_str = (char*)malloc(cmd_length);
      memcpy(g_parse_str, cmd, cmd_length+1);
      g_parse_lm = cmd_length;
      g_parse_str[cmd_length] = 0;
      g_parse_index = 0;
      //parse
      for(unsigned int l = 0; l < cmd_length; l++) {
        if(check_in_parse_cmdline(cmd[l], cmb, cmb_length)) {
          //found combine section
          g_parse_str[l] = 0;
          //search for end
          bool end_found = false;
          for(unsigned int m = l+1; m < cmd_length; m++) {
            if(check_in_parse_cmdline(cmd[m], cmb, cmb_length)) {
              //found end
              end_found = true;
              g_parse_str[m] = 0;
              l = m; //next offset (+1 => l++)
              break;
            }
          }
          if(end_found == false) {
            //invalid combine
            *valid = 0;
            return NULL;
          }
        } else if(check_in_parse_cmdline(cmd[l], delim, delim_length)) {
          g_parse_str[l] = 0;
        }
      }
    }
    while(g_parse_str[g_parse_index] == 0 && g_parse_index < g_parse_lm)
      g_parse_index++;
    if(g_parse_index >= g_parse_lm)
      return NULL;
    char * ptr =  g_parse_str + g_parse_index;
    g_parse_index += strlen(g_parse_str+g_parse_index);
    return ptr;
  }

  /**
   * @brief executes command line string
   * @param[in]     cmdline - command line string to execute
   * @param[in/out] gui     - pointer to the todo gui
   * @param[in/out] list    - pointer to the todo list
   */
  void execute_cmdline(const std::string & cmdline, todo_gui * gui,
      todo_list * list) {
    if(cmdline.length() == 0)
      return;
    const char * delim = " \n.:,;";
    int valid = 1;
    char * ptr = parse_cmdline(cmdline.c_str(), delim, "\"", &valid);

    todo_cmdline * cl = NULL;
    for(unsigned int l = 0; l < cmdline_cmds_count; l++) {
      if(strcmp(cmdline_cmds[l].cmdline_cmd, ptr) == 0) {
        cl = &cmdline_cmds[l];
        break;
      }
    }
    if(cl == NULL) { // invalid command line
      gui->print_msg_u("Invalid command...");
      return;
    }
    int argc = strlen(cl->cmdline_args);
    void ** params = NULL;
    if(argc != 0) {
      params = (void**)malloc(sizeof(void*) * (argc + 1));
      int argi = 0;
      while(argi < argc) {
        switch(cl->cmdline_args[argi]) {
          case 's':
            params[argi] = parse_cmdline(NULL, delim, "\"", &valid);
            if(valid == 0)
              goto error_free;
            break;
          case 'g':
            params[argi] = gui;
            break;
          case 'l':
            params[argi] = list;
            break;
          default:
            assert(0);
            break;
        }
        argi++;
      }
      params[argc] = NULL;
    }
    cl->cmdline_execute(params);
error_free:
    free(params);
  }

  int cmdline_help(void * params[]) {
    if(!params)
      return -1;
    if(!params[0])
      return -1;
    todo_gui * gui = (todo_gui*)params[0];
    gui->print_msg_u("HERE SHOULD BE THE HELP SCREEN");
    return 0;
  }

  int cmdline_s(void * params[]) {
    if(!params)
      return -1;
    if(!(params[1] && params[2]))
      return -1;
    const char * filename =
      (params[0] == NULL ? todo_list::default_filename.c_str() : static_cast<const char*>(params[0]));
    todo_gui * gui = static_cast<todo_gui*>(params[1]);
    todo_list * list = static_cast<todo_list*>(params[2]);
    list->save(filename);
    std::string message = "Saved to file: ";
    message += filename;
    gui->print_msg_u(message);
    return 0;
  }

  int cmdline_sortby(void * params[]) {
    if(!params)
      return -1;
    if(!(params[0] && params[1]))
      return -1;
    const char * mode = (const char *)params[0];
    todo_list * list = (todo_list*)params[1];
    if(strcmp(mode, "id") == 0)
      todo_item::sort_mode(todo_item::ID);
    else if(strcmp(mode, "state") == 0 || strcmp(mode, "st") == 0)
      todo_item::sort_mode(todo_item::STATE);
    else if(strcmp(mode, "priority") == 0 || strcmp(mode, "pr") == 0)
      todo_item::sort_mode(todo_item::PRIORITY);
    else if(strcmp(mode, "name") == 0 || strcmp(mode, "nm") == 0)
      todo_item::sort_mode(todo_item::NAME);
    list->sort();
    return 0;
  }

  int cmdline_w(void * params[]) {
    return cmdline_s(params);
  }

  int cmdline_add(void * params[]) {
    if(!params)
      return -1;
    if(!(params[0] && params[1]))
      return -1;
    const char * item_name = (const char *)params[0];
    todo_list * list = (todo_list *)params[1];
    todo_item new_item(item_name, "(empty)");
    list->add_item(new_item);
    return 0;
  }

  int cmdline_q(void * params[]) {
    if(!params || !params[0])
      return -1;
    todo_gui * gui = static_cast<todo_gui*>(params[0]);
    gui->quit();
    return 0;
  }

}; // namespace td_utils
