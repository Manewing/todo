#ifndef TODO_GUI_HH
#define TODO_GUI_HH

#include <ncurses.h>
#include "td-widget.h"
#include "td-defs.h"
#include "td-list.h"

#include "td-edit.h"

namespace todo {

  class gui : public widget, public update_if {
    public:
      enum { NO_FOCUS, ITEM_FOCUS, CMD_LINE_FOCUS };
    public:
      /**
       * @brief constructor
       * @param[in/out] list - reference to todo list
       */
      gui();

      /**
       * @brief destructor
       */
      virtual ~gui();

      virtual void callback_handler(int input);
      virtual void return_focus();

      /**
       * @brief updates gui
       */
      virtual void update();


      void print_header(WINDOW * win = stdscr);
      virtual int print(WINDOW * win = stdscr);


      void print_msg(std::string msg);
      void print_msg_u(std::string msg) { m_msg_u = msg; }

      void quit();
      bool is_running();

      static void init();

    private:
      /* disabled */
      gui(const gui&);
      gui operator=(const gui&);

    protected:
      bool         m_quit;        //< true if to quit
      int          m_scroll;      //< current scroll position
      std::string  m_msg_u;

    public:
      list    m_list;        //< reference to the actual todo list data
      edit    m_cmdline_edit;
  };

}; // namespace todo

#endif // #ifndef TODO_GUI_HH
