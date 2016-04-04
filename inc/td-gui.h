#ifndef TODO_GUI_HH
#define TODO_GUI_HH

#include <ncurses.h>
#include "td-defs.h"
#include "td-list.h"

#include "td-edit.h"

namespace td_utils {

  class todo_gui {
    public:
      enum { NO_FOCUS, ITEM_FOCUS, CMD_LINE_FOCUS };
    public:
      /**
       * @brief constructor
       * @param[in/out] list - reference to todo list
       */
      todo_gui(todo_list &list);

      /**
       * @brief destructor
       */
      virtual ~todo_gui();

      /**
       * @brief updates gui
       */
      void update();


      void print();
      void scroll_down();
      void scroll_up();
      void expand_item();
      void print_msg(std::string msg);
      void print_msg_u(std::string msg) { m_msg_u = msg; }

      std::string get_cmdline();
      void set_focus(int focus) { m_focus = focus; }
      int get_focus() const { return m_focus; }
      void quit();
      bool is_running();

    private:
      /* disabled */
      todo_gui(const todo_gui&);
      todo_gui operator=(const todo_gui&);

    protected:
      bool         m_quit;        //< true if to quit
      int          m_scroll;      //< current scroll position
      int          m_focus;       //< current focus type
      todo_list   &m_list;        //< reference to the actual todo list data
      std::string  m_msg_u;

    public:
      int m_row;
      int m_col;
      todo_edit    m_cmdline_edit;
  };

}; // namespace td_utils

#endif // #ifndef TODO_GUI_HH
