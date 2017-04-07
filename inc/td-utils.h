#ifndef TODO_UTILS_HH
#define TODO_UTILS_HH

#include <string>
#include <vector>
#include <cstdio>
#include <memory>

#include <td-defs.h>

namespace todo {

  namespace utils {

    std::string get_homedir();
    std::string get_path(std::string const& dir, std::string const& file);
    bool file_exists(std::string const& path);

    /*
     * \brief Splits string \p str respecting quotes.
     *
     * Example:
     *  'test1 "test2 test3"' -> 'test1', 'test2  test3'
     */
    std::vector<std::string> split_quoted(std::string const& str);

    /*
     * \brief Creates string object from format string and arguments.
     */
    template <typename ... Args>
    std::string format(const char* fmt, Args const& ... args) {
      int size = std::snprintf(nullptr, 0, fmt, args ...) + 1;
      std::unique_ptr<char[]> buf(new char[size]);
      std::snprintf(buf.get(), size, fmt, args ...);
      return std::string(buf.get(), buf.get() + size - 1);
    }

    class word_wrap {
      public:
        typedef std::pair<int, int>                  interval_t;
        typedef std::vector<interval_t>              intervals_t;
        typedef typename intervals_t::const_iterator iterator;

      public:
        word_wrap();
        word_wrap(word_wrap const& ww);
        word_wrap(word_wrap && ww);
        word_wrap(std::string const& str, int line_size);

        word_wrap& operator = (word_wrap const& ww);
        word_wrap& operator = (word_wrap && ww);

        inline iterator begin() const { return m_itvs.begin(); }
        inline iterator end() const { return m_itvs.end(); }
        inline int lines() const { return m_itvs.size(); }
        inline interval_t const& at(int line) const { return m_itvs.at(line); }
        inline const char* c_str() const { return m_str.c_str(); }
        inline operator std::string const& () const { return m_str; }

        int pos(td_screen_pos_t cords) const;
        td_screen_pos_t cords(int pos) const;

      private:
        intervals_t m_itvs;
        std::string m_str;
    };

  }; // namespace utils

}; // namespace todo

#endif // #ifndef TODO_UTILS_HH
