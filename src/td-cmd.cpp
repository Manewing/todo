#include <td-cmd.h>

#include <td-gui.h>
#include <td-utils.h>

namespace {

  const char* cmdline_help_doc =  "Displays this screen, help <cmd> for detailed information";
  int cmdline_help(::todo::command_line::args_t const& args) {
    (void)args;
    ::todo::gui::get().print_msg_u("HERE SHOULD BE THE HELP SCREEN");
    return 0;
  }

  const char* cmdline_save_doc = "Save todo list to file";
  int cmdline_save(::todo::command_line::args_t const& args) {
    auto& gui = ::todo::gui::get();
    auto& lst = gui.lst();

    // get filename
    auto filename = args.size() == 2 ? args[1] : ::todo::list::current_file;

    std::string message;
    if(lst.save(filename))
      message = "Saved to file: ";
    else
      message = "Could not save: ";
    message += filename;
    gui.print_msg_u(message);
    return 0;
  }

  const char* cmdline_sortby_doc = "Select sort mode for todo list";
  int cmdline_sortby(::todo::command_line::args_t const& args) {
    if (args.size() != 2)
      return 3;

    // get mode from command line
    auto const& mode = args[2];

    if(mode == "id")
      ::todo::item::sort_mode(::todo::item::ID);
    else if(mode == "state" || mode == "st")
      ::todo::item::sort_mode(::todo::item::STATE);
    else if(mode == "priority" || mode == "pr")
      ::todo::item::sort_mode(::todo::item::PRIORITY);
    else if(mode == "name" || mode == "nm")
      ::todo::item::sort_mode(::todo::item::NAME);
    ::todo::gui::get().lst().sort();
    return 0;
  }

  const char* cmdline_add_doc = "Adds item to todo list";
  int cmdline_add(::todo::command_line::args_t const& args) {
    if (args.size() != 2)
      return 3;

    // create and add new item
    ::todo::item* new_item = new ::todo::item(args[1], "(empty)");
    ::todo::gui::get().lst().add(new_item);

    return 0;
  }

  const char* cmdline_quit_doc = "Quit";
  int cmdline_quit(::todo::command_line::args_t const& args) {
    (void)args;
    ::todo::gui::get().quit();
    return 0;
  }


};

namespace todo {

  #define CMD(name) { #name, {#name, cmdline_ ##name ##_doc, &cmdline_ ##name } }
  #define ACMD(alias, name) { #alias, { #alias, cmdline_ ##name ##_doc, &cmdline_ ##name } }
  command_line command_line::m_instance = command_line::commands_t ({
      CMD(help),
      CMD(save),
      ACMD(s, save),
      ACMD(w, save),
      ACMD(write, save),
      CMD(sortby),
      CMD(add),
      ACMD(a, add),
      CMD(quit),
      ACMD(q, quit)
  });
  #undef ACMD
  #undef CMD

  command_line::command_line(commands_t && cmds):
    m_commands(std::forward<commands_t>(cmds)) {
    /* nothing todo */
  }

  command_line::~command_line() {
    /* nothing todo */
  }

  command_line& command_line::get() {
    return command_line::m_instance;
  }

  /**
   * @brief executes command line string
   * @param[in]     cmdline - command line string to execute
   */
  int command_line::execute(std::string const& cmdline) {
    if(!cmdline.length()) return 0;

    std::vector<std::string> splitted;
    try {
      // try to parse the command line string
      splitted = ::todo::utils::split_quoted(cmdline);
    } catch (std::out_of_range const& oor) {
      // could not parse command line
      (void)oor;
      ::todo::gui::get().print_msg_u("Invalid expression...");
      return 1;
    }

    // get command string
    auto const& cmd_str = splitted[0];

    // get command
    if (m_commands.find(cmd_str.c_str()) == end()) {
      // not found
      std::string msg = "Invalid command: ";
      msg += cmd_str;
      ::todo::gui::get().print_msg_u(msg);
      return 2;
    }
    auto const& cl = m_commands[cmd_str.c_str()];

    // execute command
    return cl.execute(splitted);
  }

}; // namespace todo
