#ifndef TODO_GUI_EDIT_HH
#define TODO_GUI_EDIT_HH

#include <string>
#include <list>
#include <map>

#include "td-defs.h"

namespace td_utils {

#define TD_EDIT_VALID_CHAR_START 0x20
#define TD_EDIT_VALID_CHAR_END   0x7E
#define TD_EDIT_TRIGGERED_CB     0xFF

  class todo_edit_base {
    public:
      /**
       * @brief constructor, creates new todo text edit
       */
      todo_edit_base();

      /**
       * @brief copy constructor, creates new todo text edit
       * @param[in] edit - edit to copy
       */
      todo_edit_base(const todo_edit_base&);

      /**
       * @brief destructor
       */
      virtual ~todo_edit_base();

      /**
       * @brief callback if new input has occurred
       * @param[in] input - new user input
       */
      virtual void callback(int input);

      /**
       * @brief sets callback for given input
       * @param[in] cbwrapper - callback wrapper to trigger on input, takes owner ship
       * @param[in] input     - the input to trigger callback
       */
      void set_callback(td_callback_wrapper_t * cbwrapper, int input);

      //< if edit is visible prints edit to screen
      virtual int print() const = 0;

      //< clears edit
      void clear();

      //< sets text of edit
      void set_text(std::string text) { m_text = text; m_cursor_pos = m_text.length(); }
      //< get text of edit
      std::string get_text() { return m_text; }


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
      std::map<int, td_callback_wrapper_t*> m_callbacks;
  };

  class todo_edit : public todo_edit_base {
    public:
      /**
       * @brief constructor, creates new todo text edit (single line)
       * @param[in] prefix - prefix that shall be added to text
       */
      todo_edit(std::string prefix = "");

      /**
       * @brief copy constructor, creates new todo text edit
       * @param[in] edit - edit to copy
       */
      todo_edit(const todo_edit & edit);

      /**
       * @brief callback if new input has occurred
       * @param[in] input - new user input
       */
      virtual void callback(int input);

      //< if edit is visible prints edit to screen
      virtual int print() const;

    private:
      todo_edit operator=(const todo_edit&);

    protected:
      std::string m_prefix;
      std::list<std::string> m_history;
      std::list<std::string>::iterator m_history_ptr;
  };

  class todo_multiline_edit : public todo_edit_base {
    public:
      /**
       * @brief callback if new input has occurred
       * @param[in] input - new user input
       */
      virtual void callback(int input);

      //< if edit is visible prints edit to screen
      virtual int print() const;
    private:
      todo_multiline_edit operator=(const todo_multiline_edit&);
  };

}; // namespace td_utils

#endif // #ifndef TODO_GUI_EDIT_HH
