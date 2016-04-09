#include "td-edit.h"
#include "td-gui.h"

#include <string.h>


namespace td_utils {

  /**
   * @brief constructor, creates new todo text edit
   */
  todo_edit_base::todo_edit_base(todo_widget * parent)
           : todo_widget(parent), m_visible(true), m_text(),
             m_cursor_pos(0), m_callbacks() {
    m_pos.scr_x = 0;
    m_pos.scr_y = 0;
    m_end.scr_x = 0;
    m_end.scr_y = 0;
  }

  todo_edit_base::todo_edit_base(const todo_edit_base & edit)
           : todo_widget(edit.m_parent), m_visible(edit.m_visible),
             m_pos(edit.m_pos), m_end(edit.m_end), m_text(edit.m_text),
             m_cursor_pos(edit.m_cursor_pos), m_callbacks() {
  }

  /**
   * @brief destructor
   */
  todo_edit_base::~todo_edit_base() {
    std::map<int, todo_exception*>::iterator it;
    for(it = m_callbacks.begin(); it != m_callbacks.end(); it++) {
      delete it->second;
    }
  }

  /**
   * @brief adss callback for given input
   * @param[in] cbwrapper - callback wrapper to trigger on input
   * @param[in] input     - the input to trigger callback
   */
  int todo_edit_base::callback(int input) {
    switch(input) {
      case CMDK_ARROW_LEFT:
        if(m_cursor_pos > 1)
          m_cursor_pos--;
        break;
      case CMDK_ARROW_RIGHT:
        if(m_cursor_pos < m_text.length())
          m_cursor_pos++;
        break;
      case CMDK_DELETE:
        del_char(true);
        break;
      case CMDK_BACKSPACE:
        del_char(false);
        break;
      case CMDK_ESCAPE:
        return_focus();
        break;
      default:
        if(is_valid(input))
          add_char(input & 0xFF);
        break;
    }

    // trigger callback if set
    if(m_callbacks[input])
      throw m_callbacks[input];
    else if(m_callbacks[CMDK_TRIGGERED])
      throw m_callbacks[CMDK_TRIGGERED];
    return 0;
  }

  /**
   * @brief sets callback for given input

   * @param[in] input     - the input to trigger callback
   */
  void todo_edit_base::set_callback(todo_exception * except, int input) {
    if(!except) return;
    if(m_callbacks[input])
      delete m_callbacks[input];
    m_callbacks[input] = except;
  }

  //< clears edit
  void todo_edit_base::clear() {
    m_text = "";
    m_cursor_pos = 0;
  }

  //< checks wether input is a valid character
  bool todo_edit_base::is_valid(int input) {
    return (input >= CMDK_VALID_START && input <= CMDK_VALID_END);
  }

  /**
   * @brief adds character to edit at current cursor position
   * @param[in] ch - character to add
   */
  void todo_edit_base::add_char(char ch) {
    m_text.insert(m_cursor_pos, 1, ch);
    m_cursor_pos++;
  }

  /**
   * @brief deletes character from edit at current cursor position
   * @param[in] at - if to delete character at current position or behind
   */
  void todo_edit_base::del_char(bool at) {
    // can not delte character if text is empty or cursor at front
    if(m_text.length() == 0 || (m_cursor_pos == 0 && at == false))
      return;
    if(at == false) {
      // like backspace
      m_text.erase(m_cursor_pos-1, 1);
      m_cursor_pos--;
    } else {
      // like delete
      m_text.erase(m_cursor_pos, 1);
    }
  }

  todo_edit::todo_edit(todo_widget * parent, std::string prefix)
           : todo_edit_base(parent), m_prefix(prefix),
             m_history(), m_history_ptr(m_history.begin()) {
  }

  todo_edit::todo_edit(const todo_edit & edit)
           : todo_edit_base(edit), m_prefix(edit.m_prefix),
             m_history(edit.m_history), m_history_ptr(m_history.begin()) {
  }

  int todo_edit::callback(int input) {
    if(todo_widget::callback(input) == 0) {
      switch(input) {
        case CMDK_ARROW_UP:
          if(m_history_ptr != m_history.begin())
            m_history_ptr--;
          m_text = *m_history_ptr;
          m_cursor_pos = m_text.length();
          break;
        case CMDK_ARROW_DOWN:
          if(m_history_ptr == m_history.end() ||
              m_history_ptr == --m_history.end()) {
            m_history_ptr = m_history.end();
            m_text = "";
          }
          else
            m_text = *(++m_history_ptr);
          m_cursor_pos = m_text.length();
          break;
        case CMDK_ENTER:
          m_history.push_back(m_text);
          m_history_ptr = m_history.end();
          break;
        case CMDK_DELETE:
        case CMDK_BACKSPACE:
          m_history_ptr = m_history.end();
          break;
        default:
          if(is_valid(input)) {
            m_history_ptr = m_history.end();
          }
          break;
      }
      todo_edit_base::callback(input);
    }
    return 0;
  }

  int todo_edit::print(WINDOW * win) {
    // do not need to do anything if not visible
    if(!m_visible) return 0;

    int row_count = 0;
    std::string str = m_prefix + m_text + " ";
    const int size_x = m_end.scr_x - m_pos.scr_x - 1;

    unsigned int actual_cursor_pos = m_cursor_pos + m_prefix.length() + 1;
    int start_pos = ((int)actual_cursor_pos - size_x) > 0 ?
      ((int)actual_cursor_pos - size_x) : 0;

    // clear line TODO check for better way!
    wmove(win, m_pos.scr_y, m_pos.scr_x);
    for(int l = 0; l < size_x; l++)
      waddch(win, ' ');

    mvwaddnstr(win, m_pos.scr_y, m_pos.scr_x, str.c_str() + start_pos, size_x);
    str = m_prefix + m_text.substr(0, m_cursor_pos);
    mvwaddnstr(win, m_pos.scr_y, m_pos.scr_x, str.c_str() + start_pos, size_x);
    row_count = 1;
    curs_set(1);
    return row_count;
  }

  todo_multiline_edit::todo_multiline_edit(todo_widget * parent)
                     : todo_edit_base(parent) {
  }

  int todo_multiline_edit::callback(int input) {
    if(todo_widget::callback(input) == 0) {
      switch(input) {
        case CMDK_ARROW_UP:
          if((int)m_cursor_pos - (m_end.scr_x - m_pos.scr_x) > 0)
            m_cursor_pos -= (m_end.scr_x - m_pos.scr_x - 1) ;
          break;
        case CMDK_ARROW_DOWN:
          if((int)m_cursor_pos + (m_end.scr_x - m_pos.scr_x) < (int)m_text.length())
            m_cursor_pos += (m_end.scr_x - m_pos.scr_x - 1);
          break;
      }
      todo_edit_base::callback(input);
    }
    return 0;
  }

  int todo_multiline_edit::print(WINDOW * win) {
    // do not need to do anything if not visible
    if(!m_visible) return 0;

    int row_count = 0;
    std::string str = m_text + " ";
    const int size_x = m_end.scr_x - m_pos.scr_x - 1;

    row_count = str.length() / size_x + 1;
    for(int l = 0; l < row_count; l++)
      mvwaddnstr(win, m_pos.scr_y + l, m_pos.scr_x, str.c_str() + (l * size_x), size_x);
    unsigned int cursor_row_pos = m_cursor_pos / size_x;
    unsigned int cursor_col_pos = m_cursor_pos % size_x;
    mvwaddnstr(win, m_pos.scr_y + cursor_row_pos, m_pos.scr_x,
        str.c_str() + (cursor_row_pos * size_x), cursor_col_pos);
    curs_set(1);
    return row_count;
  }

}; // namespace td_utils
