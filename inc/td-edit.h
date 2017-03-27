#ifndef TODO_GUI_EDIT_HH
#define TODO_GUI_EDIT_HH

#include <string>
#include <list>
#include <map>

#include "td-widget.h"
#include "td-except.h"
#include "td-update.h"
#include "td-defs.h"

namespace todo {

  class edit_base : public widget,
                    public update_if {
    public:
      /**
       * @brief constructor, creates new todo text edit
       */
      edit_base();

      edit_base(edit_base const&) = delete;

      /**
       * @brief destructor
       */
      virtual ~edit_base();

      /**
       * @brief callback handler if new input has occurred
       * @param[in] input - new user input
       */
      virtual void callback_handler(int input);

      void set_callback(exception* except, int input);

      //< if edit is visible prints edit to screen
      virtual int print(WINDOW * win) = 0;

      //< clears edit
      void clear();

      //< sets text of edit
      void set_text(std::string text);
      //< get text of edit
      std::string const& get_text() const { return m_text; }


      //< sets text edit pos
      void set_pos(td_screen_pos_t pos) { m_pos = pos; }
      //< get text edit pos
      td_screen_pos_t get_pos() const { return m_pos; }

      //< sets text edit size
      void set_end(td_screen_pos_t end) { m_end = end; }
      //< get text edit pos
      td_screen_pos_t get_end() const { return m_end; }

      //< sets visibility of text edit
      bool visible(bool visible) { m_visible = visible;  return visible; }
      //< get visibilty of text edit
      bool visible() const { return m_visible; }

    protected:

      //< checks wether input is a valid character
      bool is_valid(int input);

      /**
       * @brief adds character to edit at current cursor position
       * @param[in] ch - character to add
       */
      void add_char(char ch);

      /**
       * @brief deletes character from edit at current cursor position
       * @param[in] at - if to delete character at current position or behind
       */
      void del_char(bool at);

    protected:
      bool m_visible;
      td_screen_pos_t m_pos;
      td_screen_pos_t m_end;
      std::string m_text;
      unsigned int m_cursor_pos;
      std::map<int, exception*> m_callbacks;
  };

  class edit : public edit_base {
    public:
      /**
       * @brief constructor, creates new todo text edit (single line)
       * @param[in] prefix - prefix that shall be added to text
       */
      edit(std::string prefix = "");

      edit(const edit &) = delete;

      /**
       * @brief callback handler if new input has occurred
       * @param[in] input - new user input
       */
      virtual void callback_handler(int input);

      //< if edit is visible prints edit to screen
      virtual int print(WINDOW * win);

    private:
      edit operator=(const edit&);

    protected:
      std::string m_prefix;
      std::list<std::string> m_history;
      std::list<std::string>::iterator m_history_ptr;
  };

  class multiline_edit : public edit_base {
    public:
      multiline_edit();
      multiline_edit(const multiline_edit&) = delete;

      /**
       * @brief callback if new input has occurred
       * @param[in] input - new user input
       */
      virtual void callback_handler(int input);

      //< if edit is visible prints edit to screen
      virtual int print(WINDOW * win);
    private:
      multiline_edit operator=(const multiline_edit&);
  };

}; // namespace todo

#endif // #ifndef TODO_GUI_EDIT_HH
