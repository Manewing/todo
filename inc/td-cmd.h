#ifndef TODO_CMD_HH
#define TODO_CMD_HH

#include <map>
#include <vector>

namespace todo {

  class command_line {
    public:

      typedef std::vector<std::string>            args_t;

      typedef struct {
        const char * cmd_str;            //< description of command
        const char * arg_str;            //< description of arguments
        const char * doc_str;            //< documentation of command
        bool         is_alias;
        int (*execute)(args_t const& a); //< function to execute
      } command;

      typedef std::map<std::string, command>      commands_t;
      typedef typename commands_t::const_iterator const_iterator;

    private:
      command_line(commands_t && cmds);
      virtual ~command_line();

      command_line() = delete;
      command_line(command_line const&) = default;
      command_line(command_line &&) = default;

      ///< singleton instance
      static command_line m_instance;

    public:
      static command_line& get();

    public:

      inline const_iterator begin() const { return m_commands.begin(); }
      inline const_iterator end() const { return m_commands.end(); }

      /**
       * @brief executes command line string
       * @param[in] cmdline - command line string to execute
       */
      int execute(std::string const& cmdline);

    private:
      commands_t m_commands;
  };

}; // namespace todo

#endif // #ifndef TODO_CMD_HH
