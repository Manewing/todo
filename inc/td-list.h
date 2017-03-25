#ifndef TODO_LIST_HH
#define TODO_LIST_HH

#include <list>

#include "td-frame.h"
#include "td-item.h"
#include "td-update.h"

namespace todo {
  class list : public std::list<item>,
               public frame,
               public update_if {
    public:
      static const std::string default_filename; //< default filename
      static std::string current_file;           //< path to current file
    public:
      /**
       * @brief constructor
       */
      list();
      list(const std::string &file_name);

      virtual ~list();

      /**
       * @brief add a todo item to the list
       * @param[in] item - pointer to the item, todo-list takes ownership
       */
      void add_item(item item);
      void remove_item(item *item);
      void undo_remove();

      virtual void callback_handler(int input);

      /**
       * @brief prints todo list to screen
       * @param[in] row    - the row to start printing
       * @param[in] column - the column to start printing
       * @param[in] size_x - the size of the screen in X direction
       * @param[in] size_y - the size of the screen in Y direction
       * @return the last row printed to
       */
      void print_items();
      virtual int print(WINDOW * win = stdscr);

      uint8_t load(const std::string &file_name);
      uint8_t save(const std::string &file_name);

      int select_next();
      int select_prev();
      void expand_selected();
      item * get_selection();

    private:
      /**
        * @brief disabled copy constructor
        */
      list(const list&);

      /**
        * @brief disabled assignment operator
        */
      void operator = (const list&);

    protected:
      std::list<item>::iterator m_sel;
      std::list<item> m_removed_items;
  };

}; // namespace todo

#endif
