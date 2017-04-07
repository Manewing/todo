#include "td-edit.h"
#include "td-gui.h"

#include <string.h>
#include <sstream>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace todo {

  /**
   * @brief constructor, creates new todo text edit
   */
  edit_base::edit_base()
           : widget(), m_visible(true), m_text(),
             m_cursor_pos(0), m_callbacks() {
    m_pos.scr_x = 0;
    m_pos.scr_y = 0;
    m_end.scr_x = 0;
    m_end.scr_y = 0;
    set_update(new urecall_exception);
  }

  /**
   * @brief destructor
   */
  edit_base::~edit_base() {
    for (auto it : m_callbacks)
      delete it.second;
  }

  /**
   * @brief callback handler for given input
   * @param[in] input - the input to trigger callback
   */
  void edit_base::callback_handler(int input) {
#ifdef TD_DEBUG
    std::stringstream ss("");
    ss << "in callback_hanlder: input:  0x" << std::hex
       << input << ", " << std::dec << input << ", text: " << m_text;
    widget::log_debug("edit_base", ss.str());
#endif

    // trigger callback if set
    if(m_callbacks[input])
      throw m_callbacks[input];

    update();
  }

  /**
   * @brief sets callback for given input
   * @param[in] input - the input to trigger callback
   */
  void edit_base::set_callback(exception* except, int input) {
    if (m_callbacks[input])
      delete m_callbacks[input];
    m_callbacks[input] = &except->notifier(this);
  }

  //< clears edit
  void edit_base::clear() {
    m_text = "";
    m_cursor_pos = 0;
  }

  void edit_base::set_text(std::string text) {
    m_text = text;
    m_cursor_pos = m_text.length();
  }

  //< checks wether input is a valid character
  bool edit_base::is_valid(int input) {
    return (input >= CMDK_VALID_START && input <= CMDK_VALID_END);
  }

  /**
   * @brief adds character to edit at current cursor position
   * @param[in] ch - character to add
   */
  void edit_base::add_char(char ch) {
    m_text.insert(m_cursor_pos, 1, ch);
    m_cursor_pos++;
  }

  /**
   * @brief deletes character from edit at current cursor position
   * @param[in] at - if to delete character at current position or behind
   */
  void edit_base::del_char(bool at) {
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

  edit::edit(std::string prefix):
    edit_base(),
    m_prefix(prefix),
    m_history(),
    m_history_ptr(m_history.begin()) {
  }

  void edit::callback_handler(int input) {
    switch(input) {
      case CMDK_ARROW_LEFT:
        if(m_cursor_pos > 1)
          m_cursor_pos--;
        break;
      case CMDK_ARROW_RIGHT:
        if(m_cursor_pos < m_text.length())
          m_cursor_pos++;
        break;
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
        del_char(true);
        m_history_ptr = m_history.end();
        break;
      case CMDK_BACKSPACE:
        del_char(false);
        m_history_ptr = m_history.end();
        break;
      case CMDK_ESCAPE:
        return_focus();
        break;
      default:
        if(is_valid(input)) {
          add_char(input & 0xFF);
          m_history_ptr = m_history.end();
        }
        break;
    }

    edit_base::callback_handler(input);
  }

  int edit::print(WINDOW * win) {
    // do not need to do anything if not visible
    if(!m_visible) return 0;

    std::string str = m_prefix + m_text;
    const int size_x = m_end.scr_x - m_pos.scr_x - 1;

    unsigned int actual_cursor_pos = m_cursor_pos + m_prefix.length() + 1;
    int start_pos = ((int)actual_cursor_pos - size_x) > 0 ?
      ((int)actual_cursor_pos - size_x) : 0;

    wmove(win, m_pos.scr_y, m_pos.scr_x);
    clrtoeol();

    mvwaddnstr(win, m_pos.scr_y, m_pos.scr_x, str.c_str() + start_pos, size_x);
    if (has_focus()) {
      gui::cursor_pos.scr_x = m_pos.scr_x + actual_cursor_pos - 1;
      gui::cursor_pos.scr_y = m_pos.scr_y;
    }
    return 1;
  }

  multiline_edit::multiline_edit():
    edit_base(),
    m_cursor_cord({0, 0}),
    m_ww(),
    m_update_wr(true),
    m_update_cp(true) {
  }

  void multiline_edit::callback_handler(int input) {
#ifdef TD_DEBUG
    widget::log_debug("multiline_edit", "in callback_handler");
#endif

    switch(input) {
      case CMDK_ARROW_LEFT:
        if (m_cursor_cord.scr_x >= 1) {
          m_cursor_cord.scr_x--;
          m_update_cp = true;
        }
        break;
      case CMDK_ARROW_RIGHT:
        if (m_cursor_cord.scr_x < m_ww.at(m_cursor_cord.scr_y).second - 1) {
          m_cursor_cord.scr_x++;
          m_update_cp = true;
        }
        break;
      case CMDK_ARROW_UP:
        if (m_cursor_cord.scr_y > 0) {
          m_cursor_cord.scr_y--;
          int size_x = m_ww.at(m_cursor_cord.scr_y).second;
          m_cursor_cord.scr_x = MIN(m_cursor_cord.scr_x, size_x - 1);
          m_update_cp = true;
        }
        break;
      case CMDK_ARROW_DOWN:
        if (m_cursor_cord.scr_y < m_ww.lines() - 1) {
          m_cursor_cord.scr_y++;
          int size_x = m_ww.at(m_cursor_cord.scr_y).second;
          m_cursor_cord.scr_x = MIN(m_cursor_cord.scr_x, size_x - 1);
          m_update_cp = true;
        }
        break;
      case CMDK_ENTER:
        add_char('\n');
        m_update_wr = true;
        break;
      case CMDK_DELETE:
        del_char(true);
        m_update_wr = true;
        break;
      case CMDK_BACKSPACE:
        del_char(false);
        m_update_wr = true;
        break;
      case CMDK_ESCAPE:
        return_focus();
        break;
      default:
        if(is_valid(input)) {
          add_char(input & 0xFF);
          m_update_wr = true;
        }
        break;
    }

    edit_base::callback_handler(input);
  }

  int multiline_edit::print(WINDOW * win) {
    // do not need to do anything if not visible
    if(!m_visible) return 0;

    if (m_update_wr) {
      size_t const size_x = m_end.scr_x - m_pos.scr_x - 1;
      m_ww = utils::word_wrap(m_text, size_x);
      m_cursor_cord = m_ww.cords(m_cursor_pos);
      m_update_wr = false;
    }

    if (m_update_cp) {
      m_cursor_pos = m_ww.pos(m_cursor_cord);
      m_update_cp = false;
    }

    int row_count = 0;
    for (auto const& it : m_ww) {
      mvwaddnstr(win, m_pos.scr_y + row_count++,
          m_pos.scr_x, m_ww.c_str()+it.first, it.second);
    }

    if (has_focus()) {
      gui::cursor_pos.scr_x = m_pos.scr_x + m_cursor_cord.scr_x;
      gui::cursor_pos.scr_y = m_pos.scr_y + m_cursor_cord.scr_y + 2;
    }
    return row_count;
  }

}; // namespace todo
