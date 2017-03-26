#ifndef TODO_LIST_HH
#define TODO_LIST_HH

#include <list>
#include <string>

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

    public:
      list();
      list(std::string const& file_name);

      list(list const&) = delete;
      list& operator = (list const&) = delete;

      virtual ~list();

      inline iterator begin() { return m_list.begin(); }
      inline iterator end() { return m_list.end(); }
      inline const_iterator begin() const { return m_list.begin(); }
      inline const_iterator end() const { return m_list.end(); }

      void sort();

      void add_item(item* i);
      void remove_item(item* i);
      void undo_remove();

      void select_next();
      void select_prev();
      void expand_selected(bool force = false); //TODO misleading function name
      item* get_selection();

      virtual int print(WINDOW* win = stdscr);

      bool load(std::string const& file_name);
      bool save(std::string const& file_name);

    private:
      void update_scroll();

    private:
      list_t    m_list;
      iterator  m_sel;
      list_t    m_removed_items;

      int       m_scroll;
  };

};

#endif // #ifndef TODO_LIST_HH
