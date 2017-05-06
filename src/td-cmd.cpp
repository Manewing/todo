#include <td-cmd.h>

#include <td-gui.h>
#include <td-utils.h>

#define CMD(name) \
  { #name, \
      { #name, \
        cmdline_ ##name ##_arg, \
        cmdline_ ##name ##_doc, \
        false, \
        &cmdline_ ##name \
      } \
  }

#define CMDA(name, alias) \
  { #name, \
    { #alias ", " #name, \
      cmdline_ ##name ##_arg, \
      cmdline_ ##name ##_doc, \
      false, \
      &cmdline_ ##name} \
  }, \
  { #alias, \
    { NULL, \
      NULL, \
      NULL, \
      true, \
      &cmdline_ ##name \
    } \
  }

#define CMD_DESC(name, arg_str, doc_str) \
  const char* cmdline_ ##name ##_arg = arg_str; \
  const char* cmdline_ ##name ##_doc = doc_str;

namespace {

  class help_screen : public ::todo::frame {
    public:
      help_screen():
        ::todo::frame(),
        m_scroll(0) {
      }

      void callback_handler(int input) {
        switch (input) {
          case CMDK_TRIGGERED:
            break;
          case CMDK_ARROW_UP:
            m_scroll--;
            break;
          case CMDK_ARROW_DOWN:
            m_scroll++;
            break;
          case CMDK_EXIT:
          case CMDK_ESCAPE:
            ::todo::gui::get().set_frame();
            return_focus();
            break;
          default:
            break;
        }
      }

      int print(WINDOW* win) {
        int mx, my;
        getmaxyx(win, my, mx);

        const int sx   = 5, sy = 5;
        const int ex   = (mx - 5) < 0 ? 0 : (mx - 5);
        const int ey   = (my - 5) < 0 ? 0 : (my - 5);
        const int midx = (ex - sx) / 2;

        // set position
        set_pos(win, {sx, sy}, {ex, ey});

        int scroll = 4 + m_scroll;

        // print commands and documentation
        for (auto const& it : ::todo::command_line::get()) {
          if (it.second.is_alias)
            continue;

          wattron(m_win, A_BOLD);
          mvwprintw(m_win, scroll++, 4, ":%s ", it.second.cmd_str);
          wattroff(m_win, A_BOLD);

          if (it.second.arg_str) {
            wattron(m_win, A_UNDERLINE);
            wprintw(m_win, "%s", it.second.arg_str);
            wattroff(m_win, A_UNDERLINE);
          }

          ::todo::utils::word_wrap ww(it.second.doc_str, mx - 16);
          for (auto const& it : ww) {
            mvwaddnstr(m_win, scroll++, 8, ww.c_str()+it.first, it.second);
          }
          scroll += 1;

        }

        // print title
        const char help_title[] = "TODO - HELP";
        const int htsize        = sizeof(help_title);
        wattron(m_win, A_BOLD | A_UNDERLINE);
        mvwprintw(m_win, 2, midx - htsize/2, help_title);
        wattroff(m_win, A_BOLD | A_UNDERLINE);

        return ::todo::frame::print(win);
      }

      virtual ~help_screen() {
      }

    private:
      int m_scroll;
  };

  CMD_DESC(help, NULL, "Displays this screen, help <cmd> for detailed information");
  int cmdline_help(::todo::command_line::args_t const& args) {
    (void)args;
    ::todo::gui::get().set_frame(new help_screen);
    return 0;
  }

  CMD_DESC(save, "filename", "Save todo list to file");
  int cmdline_save(::todo::command_line::args_t const& args) {
    auto& gui = ::todo::gui::get();
    auto& lst = gui.lst();

    // get filename
    auto filename = args.size() == 2 ? args[1] : ::todo::list::current_file;

    bool res = lst.save(filename);
    gui.print_msg_u("%s: %s", (res ? "Saved to file: " : "Could not save"),
        filename.c_str());
    return 0;
  }

  CMD_DESC(sortby, "mode", "Select sort mode for todo list");
  int cmdline_sortby(::todo::command_line::args_t const& args) {
    if (args.size() != 2)
      return 3;

    // get mode from command line
    auto const& mode = args[1];

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

  CMD_DESC(add, "item name", "Adds item to todo list");
  int cmdline_add(::todo::command_line::args_t const& args) {
    if (args.size() != 2)
      return 3;

    // create and add new item
    ::todo::item* new_item = new ::todo::item(args[1], "(empty)");
    ::todo::gui::get().lst().add(new_item);

    return 0;
  }

  CMD_DESC(quit, NULL, "Quit");
  int cmdline_quit(::todo::command_line::args_t const& args) {
    (void)args;
    ::todo::gui::get().quit();
    return 0;
  }


};

namespace todo {

  command_line command_line::m_instance = command_line::commands_t ({
      CMDA(help, h),
      CMDA(save, w),
      CMDA(sortby, s),
      CMDA(add, a),
      CMDA(quit, q)
  });

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
      ::todo::gui::get().print_msg_u("Invalid expression: %s", cmdline.c_str());
      return 1;
    }

    // get command string
    auto const& cmd_str = splitted[0];

    // get command
    if (m_commands.find(cmd_str.c_str()) == end()) {
      // not found
      ::todo::gui::get().print_msg_u("Invalid command: %s", cmd_str.c_str());
      return 2;
    }
    auto const& cl = m_commands[cmd_str.c_str()];

    // execute command
    return cl.execute(splitted);
  }

}; // namespace todo
