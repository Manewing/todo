#ifndef TODO_UTILS_HH
#define TODO_UTILS_HH

#include <string>
#include <vector>
#include <cstdio>
#include <memory>

namespace todo {

  namespace utils {

    std::vector<std::string> split_quoted(std::string const& str);

    template <typename ... Args>
    std::string format(const char* fmt, Args const& ... args) {
      int size = std::snprintf(nullptr, 0, fmt, args ...) + 1;
      std::unique_ptr<char[]> buf(new char[size]);
      std::snprintf(buf.get(), size, fmt, args ...);
      return std::string(buf.get(), buf.get() + size - 1);
    }

  }; // namespace utils

}; // namespace todo

#endif // #ifndef TODO_UTILS_HH
