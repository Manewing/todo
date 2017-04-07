#include <td-utils.h>

namespace todo {

  namespace utils {

    std::vector<std::string> split_quoted(std::string const& str) {
      std::vector<std::string> splitted;

      size_t pos = 0, last = 0;
      while (pos < str.size()) {
        if (str[pos] == ' ') {
          // only add new string if we already read characters
          // other than whitespace
          if (last != pos)
            splitted.push_back(str.substr(last, pos - last));
          last = pos + 1;
        } else if (str[pos] == '\"') {
          last = ++pos;
          // run until next quotation mark
          while (str[pos++] != '\"');
          splitted.push_back(str.substr(last, pos - last - 1));
          last = pos + 1;
        }
        pos++;
      }

      //add leftover
      if (last != pos)
        splitted.push_back(str.substr(last, pos - last));

      return splitted;
    }

    word_wrap::word_wrap(std::string const& str, int line_size):
      m_itvs(),
      m_str(str) {

      // reserve least amount of lines needed
      m_itvs.reserve(str.size() / line_size + 1);

      size_t pos = 0, tmp = 0, size = 0, last = 0;
      while (pos < str.size()) {

        // not a whitespace character?
        if (str[pos] != ' ' && str[pos] != '\n') {
          // yes, increase current word size
          tmp += 1;
        } else {
          // current line size with last word 'tmp' and
          // whitespace greater than line size?
          if ( size + tmp + 1 > line_size) {
            // add interval
            m_itvs.push_back( interval_t {last, size } );

            // set end of last line and reset current line size
            last += size;
            size = 0;
          }

          // add started word to current line
          size += ++tmp;
          tmp = 0;

          if (str[pos] == '\n') {
            // got new line -> forced cut
            m_itvs.push_back( interval_t {last, size } );
            m_str[last + size - 1] = ' ';
            last += size;
            size = 0;
          }
        }
        // increase reading pos
        pos++;
      }

      if (size + tmp + 1 > line_size) {
        m_itvs.push_back( interval_t {last, size} );
        last += size;
        size = 0;
      }

      size += tmp;
      m_itvs.push_back(interval_t {last, size + 1} );
      m_str.push_back(' ');
    }

    word_wrap::word_wrap():
      m_itvs(),
      m_str() {
      /* nothing todo */
    }

    word_wrap::word_wrap(word_wrap const& ww):
      m_itvs(ww.m_itvs),
      m_str(ww.m_str) {
      /* nothing todo */
    }

    word_wrap::word_wrap(word_wrap && ww):
      m_itvs(std::forward<intervals_t>(ww.m_itvs)),
      m_str(std::forward<std::string>(ww.m_str)) {
      /* nothing todo */
    }

    word_wrap& word_wrap::operator = (word_wrap const& ww) {
      m_itvs = ww.m_itvs;
      m_str = ww.m_str;
    }

    word_wrap& word_wrap::operator = (word_wrap && ww) {
      m_itvs = std::forward<intervals_t>(ww.m_itvs);
      m_str = std::forward<std::string>(ww.m_str);
    }

    int word_wrap::pos(td_screen_pos_t cords) const {
      auto const& p = m_itvs.at(cords.scr_y);
      int __pos =  p.first;
      __pos += cords.scr_x < p.second ? cords.scr_x : p.second;
      return __pos;
    }

    td_screen_pos_t word_wrap::cords(int pos) const {
      td_screen_pos_t cords = {0, 0};

      // find the interval belonging to the position
      for (auto const& it : m_itvs) {

        // position in current interval?
        if (it.first <= pos && pos < it.first + it.second) {
          // we found the coordinates
          cords.scr_x = pos - it.first;
          break;
        }
        cords.scr_y++;
      }
      return cords;
    }

  }; // namespace utils

}; // namespace todo
