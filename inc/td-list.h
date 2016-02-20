#ifndef TODO_LIST_HH
#define TODO_LIST_HH

#include <list>

#include "td-item.h"

class todo_list : public std::list<todo_item> {
  public:
    static const std::string default_filename; //< default filename
    static std::string current_file;           //< path to current file
  public:
    /**
     * @brief constructor
     */
    todo_list();
    todo_list(const std::string &file_name);

    ~todo_list();

    /**
     * @brief add a todo item to the list
     * @param[in] item - pointer to the item, todo-list takes ownership
     */
    void add_item(todo_item item);
    void remove_item(todo_item *item);
    void undo_remove();

    /**
     * @brief prints todo list to screen
     * @param[in] row    - the row to start printing
     * @param[in] column - the column to start printing
     * @param[in] size_x - the size of the screen in X direction
     * @param[in] size_y - the size of the screen in Y direction
     * @return the last row printed to
     */
    int print(int row, int col, int size_x, int size_y);

    uint8_t load(const std::string &file_name);
    uint8_t save(const std::string &file_name);

    int select_next();
    int select_prev();
    void expand_selected();
    todo_item * get_selection();

  private:
    /**
      * @brief disabled copy constructor
      */
    todo_list(const todo_list&);

    /**
      * @brief disabled assignment operator
      */
    void operator = (const todo_list&);

  protected:
    std::list<todo_item>::iterator m_sel;
    std::list<todo_item> m_removed_items;
};

#endif
