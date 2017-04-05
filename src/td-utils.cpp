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

  }; // namespace utils

}; // namespace todo
