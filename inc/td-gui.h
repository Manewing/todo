#ifndef TODO_GUI_HH
#define TODO_GUI_HH

#include <ncurses.h>
#include <td-defs.h>
#include <td-list.h>


namespace td_utils {

  class todo_gui {
    public:
      enum { NO_FOCUS, ITEM_FOCUS, CMD_LINE_FOCUS };
      enum { CMDK_ENTER = 0x0A, CMDK_ESCAPE = 0x1B,
             CMDK_EXIT = 0x71, CMDK_START_CMD = 0x3A,
             CMDK_DELETE = 0x64, CMDK_EDIT = 0x65,
             CMDK_UNDO = 0x75, CMDK_ARROW_DOWN = 0x102,
             CMDK_ARROW_UP = 0x103, CMDK_ARROW_LEFT = 0x104,
             CMDK_ARROW_RIGHT = 0x105, CMDK_BACKSPACE = 0x107 };
    public:
      todo_gui(todo_list &list);
      virtual ~todo_gui();
      void update();
      void print();
      void scroll_down();
      void scroll_up();
      void expand_item();
      void print_msg(std::string msg);
      void print_msg_u(std::string msg) { m_msg_u = msg; }

      void start_cmdline();
      void add_cmdline(char ch);
      void bsp_cmdline();
      void clear_cmdline();
      void cleft_cmdline();
      void cright_cmdline();
      void add_cmdline_history();
      void get_prev_cmdline();
      void get_next_cmdline();

      std::string get_cmdline();
      void set_focus(int focus) { m_focus = focus; }
      int get_focus() const { return m_focus; }
      void quit();
      bool is_running();
    public:
      int m_row;
      int m_col;
    private:
      bool         m_quit;
      int          m_scroll;
      int          m_focus;
      todo_list   &m_list;
      std::string  m_msg_u;
      std::string  m_cmdline;
      unsigned int m_cmdline_pos;
      std::list<std::string> m_cmdline_history;
      std::list<std::string>::iterator m_cmdline_histptr;
  };

}; // namespace td_utils

#endif // #ifndef TODO_GUI_HH
