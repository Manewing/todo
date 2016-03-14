#include "td-edit.h"
#include "td-gui.h"

#include <string.h>


namespace td_utils {

  /**
   * @brief constructor, creates new todo text edit
   * @param[in] style  - style of text edit
   * @param[in] prefix - prefix that shall be added to text
   */
  todo_edit::todo_edit(int style, std::string prefix)
           : m_visible(true), m_style(style), m_prefix(prefix),
             m_text(), m_cursor_pos(0), m_history(), m_callbacks() {
    m_pos.scr_x = 0;
    m_pos.scr_y = 0;
    m_end.scr_x = 0;
    m_end.scr_y = 0;
  }

  todo_edit::todo_edit(const todo_edit & edit)
           : m_visible(edit.m_visible), m_style(edit.m_style), m_pos(edit.m_pos),
             m_end(edit.m_end), m_prefix(edit.m_prefix), m_text(edit.m_text),
             m_cursor_pos(edit.m_cursor_pos), m_history(edit.m_history),
             m_callbacks() {
  }

  /**
   * @brief destructor
   */
  todo_edit::~todo_edit() {
    std::map<int, td_callback_wrapper_t*>::iterator it;
    for(it = m_callbacks.begin(); it != m_callbacks.end(); it++) {
      if(it->second)
        delete [] it->second->params;
      delete it->second;
    }
  }

  /**
   * @brief adss callback for given input
   * @param[in] cbwrapper - callback wrapper to trigger on input
   * @param[in] input     - the input to trigger callback
   */
  void todo_edit::callback(int input) {
    if(m_style & TD_EDIT_SINGLELINE) { // singleline edit
      switch(input) {
        case td_utils::todo_gui::CMDK_ARROW_UP:
          if(m_style & __TD_EDIT_HISTORY) {
          }
          break;
        case td_utils::todo_gui::CMDK_ARROW_DOWN:
          if(m_style & __TD_EDIT_HISTORY) {
          }
          break;
        case td_utils::todo_gui::CMDK_ARROW_LEFT:
          if(m_cursor_pos > 1)
            m_cursor_pos--;
          break;
        case td_utils::todo_gui::CMDK_ARROW_RIGHT:
          if(m_cursor_pos < m_text.length())
            m_cursor_pos++;
          break;
        case td_utils::todo_gui::CMDK_DELETE:
          del_char(true);
          break;
        case td_utils::todo_gui::CMDK_BACKSPACE:
          del_char(false);
          break;
        default:
          if(is_valid(input))
            add_char(input & 0xFF);
          break;
      }
    } else { //multiline edit
      //TODO implement
      switch(input) {
        case td_utils::todo_gui::CMDK_ARROW_UP:
          if((int)m_cursor_pos - (m_end.scr_x - m_pos.scr_x) > 0)
            m_cursor_pos -= (m_end.scr_x - m_pos.scr_x - 1) ;
          break;
        case td_utils::todo_gui::CMDK_ARROW_DOWN:
          if((int)m_cursor_pos + (m_end.scr_x - m_pos.scr_x) < (int)m_text.length())
            m_cursor_pos += (m_end.scr_x - m_pos.scr_x - 1);
          break;
        case td_utils::todo_gui::CMDK_ARROW_LEFT:
          if(m_cursor_pos > 1)
            m_cursor_pos--;
          break;
        case td_utils::todo_gui::CMDK_ARROW_RIGHT:
          if(m_cursor_pos < m_text.length())
            m_cursor_pos++;
          break;
        case td_utils::todo_gui::CMDK_DELETE:
          del_char(true);
          break;
        case td_utils::todo_gui::CMDK_BACKSPACE:
          del_char(false);
          break;
        default:
          if(is_valid(input))
            add_char(input & 0xFF);
          break;
      }
    }

    // trigger callback if set
    if(m_callbacks[input])
      m_callbacks[input]->callback_wrapper(m_callbacks[input]->params);
    if(m_callbacks[TD_EDIT_TRIGGERED_CB])
      m_callbacks[TD_EDIT_TRIGGERED_CB]->callback_wrapper(m_callbacks[TD_EDIT_TRIGGERED_CB]->params);

    // update screen
    print();
  }

  /**
   * @brief sets callback for given input
   * @param[in] cbwrapper - callback wrapper to be triggered on input, takes ownership
   * @param[in] input     - the input to trigger callback
   */
  void todo_edit::set_callback(td_callback_wrapper_t * cbwrapper, int input) {
    if(!cbwrapper) return;
    if(!cbwrapper->callback_wrapper) return;
    m_callbacks[input] = cbwrapper;
  }

  //< if edit is visible prints edit to screen
  int todo_edit::print() {
    int row_count = 0;
    if(!m_visible) return 0;
    std::string str = m_prefix + m_text + " ";
    const int size_x = m_end.scr_x - m_pos.scr_x - 1;
    if(m_style & TD_EDIT_SINGLELINE) { // singleline edit
      unsigned int actual_cursor_pos = m_cursor_pos + m_prefix.length() + 1;
      int start_pos = ((int)actual_cursor_pos - size_x) > 0 ?
        ((int)actual_cursor_pos - size_x) : 0;
      mvaddnstr(m_pos.scr_y, m_pos.scr_x, str.c_str() + start_pos, size_x);
      str = m_prefix + m_text.substr(0, m_cursor_pos);
      mvaddnstr(m_pos.scr_y, m_pos.scr_x, str.c_str() + start_pos, size_x);
      row_count = 1;
    } else { //multiline edit
      row_count = str.length() / size_x + 1;
      for(int l = 0; l < row_count; l++)
        mvaddnstr(m_pos.scr_y + l, m_pos.scr_x, str.c_str() + (l * size_x), size_x);
      unsigned int cursor_row_pos = m_cursor_pos / size_x;
      unsigned int cursor_col_pos = m_cursor_pos % size_x;
      mvaddnstr(m_pos.scr_y + cursor_row_pos, m_pos.scr_x,
          str.c_str() + (cursor_row_pos * size_x), cursor_col_pos);
    }
    refresh();
    curs_set(1);
    return row_count;
  }

  //< clears edit
  void todo_edit::clear() {
    m_text = "";
    m_cursor_pos = 0;
  }

  //< checks wether input is a valid character
  bool todo_edit::is_valid(int input) {
    return (input >= TD_EDIT_VALID_CHAR_START && input <= TD_EDIT_VALID_CHAR_END);
  }

  /**
   * @brief adds character to edit at current cursor position
   * @param[in] ch - character to add
   */
  void todo_edit::add_char(char ch) {
    m_text.insert(m_cursor_pos, 1, ch);
    m_cursor_pos++;
  }

  /**
   * @brief deletes character from edit at current cursor position
   * @param[in] at - if to delete character at current position or behind
   */
  void todo_edit::del_char(bool at) {
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

}; // namespace td_utils