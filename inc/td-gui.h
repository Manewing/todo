#ifndef TODO_GUI_HH
#define TODO_GUI_HH

#include <ncurses.h>

#include "td-defs.h"
#include "td-widget.h"
#include "td-edit.h"
#include "td-list.h"

namespace todo {

  class gui_header : public frame {
    public:
      gui_header();
      virtual ~gui_header();

      virtual int print(WINDOW* win);
  };

  class gui : public widget, public update_if {
    public:
      enum { NO_FOCUS, ITEM_FOCUS, CMD_LINE_FOCUS };
      static td_screen_pos_t cursor_pos;

    public:
      gui();
      virtual ~gui();

      virtual void callback_handler(int input);
      virtual void return_focus();
      virtual void update();

      virtual int print(WINDOW * win = stdscr);

      void print_msg(std::string msg);
      void print_msg_u(std::string msg) { m_msg_u = msg; }

      void quit();
      bool is_running();

      static void init();

      //TODO
      inline list& lst() { return m_list; }

    private:
      /* disabled */
      gui(const gui&);
      gui operator=(const gui&);

    protected:
      list         m_list;
      bool         m_quit;        //< true if to quit
      std::string  m_msg_u;
      gui_header   m_header;

    public:
      edit    m_cmdline_edit;
  };

}; // namespace todo

#endif // #ifndef TODO_GUI_HH
