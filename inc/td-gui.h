#ifndef TODO_GUI_HH
#define TODO_GUI_HH

#include <ncurses.h>

#include "td-defs.h"
#include "td-widget.h"
#include "td-edit.h"
#include "td-list.h"
#include "td-utils.h"

namespace todo {

  class gui_header : public frame {
    public:
      gui_header();
      virtual ~gui_header();

      virtual int print(WINDOW* win);
  };

  class gui : public widget, public update_if {
    public:
      static td_screen_pos_t cursor_pos;

      static gui& get();
      static void free();

    private:
      static gui* m_instance;

      gui();
      virtual ~gui();

    public:

      virtual void callback_handler(int input);
      virtual void return_focus();
      virtual void update();

      virtual int print(WINDOW * win = stdscr);

      template <typename ... Args>
      inline void print_msg_u(const char* fmt, Args ... args) {
        m_msg_u = ::todo::utils::format(fmt, args ...);
      }

      inline void quit() { m_quit = true; }
      inline bool is_running() const { return !m_quit; }
      inline list& lst() { return m_list; }

      void set_frame(frame* fr = NULL);

    private:
      /* disabled */
      gui(const gui&);
      gui operator=(const gui&);

    protected:
      list         m_list;
      bool         m_quit;        //< true if to quit
      std::string  m_msg_u;
      gui_header   m_header;
      frame*       m_frame;
      edit         m_cmdline_edit;
  };

}; // namespace todo

#endif // #ifndef TODO_GUI_HH
