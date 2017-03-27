#ifndef TODO_LIST_HH
#define TODO_LIST_HH

#include <set>
#include <list>
#include <string>
#include <tuple>

#include "td-item.h"
#include "td-frame.h"


namespace todo {

  class list : public frame {
    public:
      typedef std::list<item*>                list_t;
      typedef typename list_t::iterator       iterator;
      typedef typename list_t::const_iterator const_iterator;

      static std::string       current_file;
      static std::string const default_filename;

    private:
      typedef std::set<item*>    buffer_t;
      typedef std::list<list_t>  history_t;

    public:
      list();

      list(list const&) = delete;
      list& operator = (list const&) = delete;

      virtual ~list();

      inline iterator begin() { return m_list.begin(); }
      inline iterator end() { return m_list.end(); }
      inline const_iterator begin() const { return m_list.begin(); }
      inline const_iterator end() const { return m_list.end(); }

      void sort();

      void add(item* i);
      void remove(item* i);

      void undo();
      void redo();

      void select_next();
      void select_prev();
      void expand_selected(bool force = false); //TODO misleading function name
      item* get_selection();

      virtual int print(WINDOW* win = stdscr);

      bool load(std::string const& file_name);
      bool save(std::string const& file_name);

    private:
      void update_history();
      void update_scroll();

    private:
      buffer_t  m_buf;
      list_t    m_list;
      iterator  m_sel;
      history_t m_history;
      int       m_scroll;
  };

};

#endif // #ifndef TODO_LIST_HH
