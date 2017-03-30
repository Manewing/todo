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
    if (has_focus()) {
      gui::cursor_pos.scr_x = m_pos.scr_x + actual_cursor_pos - 1;
      gui::cursor_pos.scr_y = m_pos.scr_y;
    }
    return 1;
  }

  multiline_edit::multiline_edit():
    edit_base(),
    m_cursor_cord({0, 0}),
    m_wrapped_text(),
    m_update_wr(true),
    m_update_cp(true) {
  }

  void multiline_edit::callback_handler(int input) {
#ifdef TD_DEBUG
    widget::log_debug("multiline_edit", "in callback_handler");
#endif

    auto const& itvs = m_wrapped_text.first;
    switch(input) {
      case CMDK_ARROW_LEFT:
        if (m_cursor_cord.scr_x >= 1) {
          m_cursor_cord.scr_x--;
          m_update_cp = true;
        }
        break;
      case CMDK_ARROW_RIGHT:
        if (m_cursor_cord.scr_x < itvs.at(m_cursor_cord.scr_y).second - 1) {
          m_cursor_cord.scr_x++;
          m_update_cp = true;
        }
        break;
      case CMDK_ARROW_UP:
        if (m_cursor_cord.scr_y > 0) {
          m_cursor_cord.scr_y--;
          size_t size_x = itvs.at(m_cursor_cord.scr_y).second;
          m_cursor_cord.scr_x = MIN(m_cursor_cord.scr_x, size_x - 1);
          m_update_cp = true;
        }
        break;
      case CMDK_ARROW_DOWN:
        if (m_cursor_cord.scr_y < m_wrapped_text.first.size() - 1) {
          m_cursor_cord.scr_y++;
          size_t size_x = itvs.at(m_cursor_cord.scr_y).second;
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
      m_wrapped_text = word_wrap(m_text, size_x);
      m_cursor_cord = wrapped_cords(m_wrapped_text.first, m_cursor_pos);
      m_update_wr = false;
    }

    if (m_update_cp) {
      m_cursor_pos = wrapped_pos(m_wrapped_text.first, m_cursor_cord);
      m_update_cp = false;
    }

    auto const& cuts = m_wrapped_text.first;
    auto const& disp_str = m_wrapped_text.second;

    int row_count = 0;
    for (auto const& it : cuts) {
      mvwaddnstr(win, m_pos.scr_y + row_count++,
          m_pos.scr_x, disp_str.c_str()+it.first, it.second);
    }

    if (has_focus()) {
      gui::cursor_pos.scr_x = m_pos.scr_x + m_cursor_cord.scr_x;
      gui::cursor_pos.scr_y = m_pos.scr_y + m_cursor_cord.scr_y + 2;
    }
    return row_count;
  }

  size_t multiline_edit::wrapped_pos(intervals_t const& itvs,
                                     td_screen_pos_t cords) {
    auto const& p = itvs.at(cords.scr_y);
    size_t pos = p.first;
    pos += cords.scr_x < p.second ? cords.scr_x : p.second;
    return pos;
  }

  td_screen_pos_t multiline_edit::wrapped_cords(intervals_t const& itvs,
                                                size_t pos) {
    td_screen_pos_t cords = {0, 0};
    for (auto const& it : itvs) {
      if (it.first <= pos && pos < it.first + it.second) {
        cords.scr_x = pos - it.first;
        break;
      }
      cords.scr_y++;
    }
    return cords;
  }

  multiline_edit::wrapped_t multiline_edit::word_wrap(std::string const& str,
                                                      size_t const line_size) {
    wrapped_t wr;
    intervals_t& cuts = wr.first;
    std::string& disp_str = wr.second;

    // copy string
    disp_str = str;

    // reserve for least amount of lines
    cuts.reserve(str.size() / line_size + 1);

    size_t pos = 0, tmp = 0, size = 0, last = 0;
    while (pos < str.size()) {
      if (str[pos] != ' ' && str[pos] != '\n') {
        // add non whitespace char to tmp
        tmp++;
      } else {
        // current line size with tmp and
        // whitespace greater than line size?
        if (size + tmp + 1 > line_size) {
          cuts.push_back( interval_t {last, size} );
          last += size;
          size = 0;
        }

        size += ++tmp;
        tmp = 0;

        if (str[pos] == '\n') {
          // got new line -> forced cut
          cuts.push_back( interval_t {last, size} );
          disp_str[last + size - 1] = ' ';
          last += size;
          size = 0;
        }
      }
      pos++;
    }

    if (size + tmp + 1 > line_size) {
      cuts.push_back( interval_t {last, size} );
      last += size;
      size = 0;
    }
    size += tmp;
    cuts.push_back(interval_t {last, size + 1} );
    disp_str.push_back(' ');

    return wr;
  }


}; // namespace todo
