#ifndef TODO_ITEM_HH
#define TODO_ITEM_HH

#include <stdint.h>
#include <iostream>
#include <string>

#include "td-edit.h"


class todo_item {

  public:
    typedef enum { INVALID_STATE, TODO, WORK_IN_PRG, DONE } TD_STATE;

    typedef enum { INVALID_PR, LOW, NORMAL, HIGH } TD_PRIORITY;

    typedef enum { ID, STATE, PRIORITY, NAME } SB_TYPE;

  public:
    /**
     * @brief constructor
     */
    todo_item();

    todo_item(std::string name,
              std::string comment,
              uint8_t priority = NORMAL,
              uint8_t state = TODO);
    todo_item(const todo_item & item);
    virtual ~todo_item();

    /**
     * @brief
     */
    void make(std::string name,
              std::string comment,
              uint8_t priority = NORMAL,
              uint8_t state = TODO);

    bool operator < (const todo_item& it) const;
    bool operator == (const todo_item& it) const { return m_ID == it.m_ID; }


    int format_comment(int line_size);

    /**
     * @brief prints item
     * @param[in] row    - the row to start printing the item
     * @param[in] col    - the column to start printing the item
     * @param[in] size_x - the size of the screen in X direction
     * @param[in] size_y - the size of the screen in Y direction
     * @return the amount of rows printed
     */
    int print(int row, int col, int size_x, int size_y);

    static void sort_mode(uint8_t mode);
    static uint8_t get_sort_mode() { return SORT_BY; }

    static std::string state2string(uint8_t state);

    uint32_t get_id() const { return m_ID; }

    void set_name(std::string name) { m_name = name; }
    std::string get_name() const { return m_name; }

    void set_comment(std::string comment) { m_comment = comment; }
    std::string get_comment() const { return m_comment; }

    void set_priority(uint8_t priority) { m_priority = priority; }
    uint8_t get_priority() const { return m_priority; }

    void set_state(uint8_t state) { m_state = state; }
    uint8_t get_state() const { return m_state; }

    void set_expanded(bool expanded);
    bool is_expanded() const { return m_exp; }

    void set_selected(bool selected);
    bool is_selected() const { return m_sel; }

    int get_top_row() const { return m_top_row; }
    int get_bottom_row() const { return m_bottom_row; }

  public:
    td_utils::todo_edit m_text_edit;

  protected:
    static uint32_t MID;        ///< max ID
    static uint8_t SORT_BY;     ///< sets sort mode
    uint32_t m_ID;              ///< ID of this object
    std::string m_name;         ///< name
    std::string m_comment;      ///< comment
    std::string m_comment_fmt;  ///< formated comment
    uint8_t m_priority;         ///< priority
    uint8_t m_state;            ///< state
    bool m_exp;                 ///< true if expanded
    bool m_sel;
    int  m_top_row;
    int  m_bottom_row;
    int  m_line_size;
    int  m_rows;

};

#endif
