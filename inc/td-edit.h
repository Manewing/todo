#ifndef TODO_GUI_EDIT_HH
#define TODO_GUI_EDIT_HH

#include <string>
#include <list>
#include <map>

#include "td-defs.h"

namespace td_utils {

#define TD_EDIT_MULTILINE   0x00
#define TD_EDIT_SINGLELINE  0x01
#define __TD_EDIT_HISTORY   0x02
#define TD_EDIT_HISTORY     ( TD_EDIT_SINGLELINE | __TD_EDIT_HISTORY )

#define TD_EDIT_VALID_CHAR_START 0x20
#define TD_EDIT_VALID_CHAR_END   0x7E
#define TD_EDIT_TRIGGERED_CB     0xFF

  class todo_edit {
    public:
      /**
       * @brief constructor, creates new todo text edit
       * @param[in] style  - style of text edit
       * @param[in] prefix - prefix that shall be added to text
       */
      todo_edit(int style, std::string prefix = "");

      /**
       * @brief copy constructor, creates new todo text edit
       * @param[in] edit - edit to copy
       */
      todo_edit(const todo_edit & edit);

      /**
       * @brief destructor
       */
      virtual ~todo_edit();

      /**
       * @brief callback if new input has occurred
       * @param[in] input - new user input
       */
      void callback(int input);

      /**
       * @brief sets callback for given input
       * @param[in] cbwrapper - callback wrapper to trigger on input, takes owner ship
       * @param[in] input     - the input to trigger callback
       */
      void set_callback(td_callback_wrapper_t * cbwrapper, int input);

      //< if edit is visible prints edit to screen
      int print();

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

    private:
      //todo_edit operator=(const todo_edit&);

    protected:
      bool m_visible;
      int  m_style;
      td_screen_pos_t m_pos;
      td_screen_pos_t m_end;
      std::string m_prefix;
      std::string m_text;
      unsigned int m_cursor_pos;
      std::list<std::string> m_history;
      std::map<int, td_callback_wrapper_t*> m_callbacks;

  };

}; // namespace td_utils

#endif // #ifndef TODO_GUI_EDIT_HH
