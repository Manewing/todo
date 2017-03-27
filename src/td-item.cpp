#include "td-item.h"

#include "td-gui.h"
#include "td-except.h"

#include <ncurses.h>

namespace todo {

  uint32_t item::MID = 0;
  uint8_t  item::SORT_BY = ID;

  item::item():
    widget(),
    m_ID(++MID),
    m_text_edit() {
    m_text_edit.visible(false);
  }

  item::item(std::string name,
             std::string comment,
             uint8_t priority,
             uint8_t state):
    widget(),
    m_ID(++MID),
    m_name(name),
    m_priority(priority),
    m_state(state),
    m_exp(false),
    m_sel(false),
    m_top_row(0),
    m_bottom_row(0),
    m_text_edit() {
    m_text_edit.visible(false);
    m_text_edit.set_text(comment);
  }

  item::~item() {
    //nothing todo
  }

  bool item::operator<(const item& it) const {
    switch(SORT_BY) {
      default:
      case ID:
        return (m_ID < it.m_ID);
      case STATE:
        return (m_state < it.m_state);
      case PRIORITY:
        return (m_priority < it.m_priority);
      case NAME:
        return (m_name.compare(it.m_name) < 0);
    }
    return false;
  }

  bool item::operator==(const item& it) const {
    return it.m_ID == m_ID;
  }

  void item::callback(int input) {
#ifdef TD_DEBUG
    widget::log_debug("item", "in callback");
#endif
    m_text_edit.callback(input);
  }

  void item::set_update(exception * e) {
    m_text_edit.set_update(e);
  }

#define PRINT_OFFSET_BACK 19 //TODO dynamic?
#define PRINT_OFFSET_EXP   9
#define PRINT_OFFSET_CMB   3

  int item::print(WINDOW * win) {
    int draw_pos = 0, size_x, tmp, row, col;

    m_bottom_row = m_top_row + 1;
    row = m_top_row;
    col = 0;

    getmaxyx(win, tmp, size_x);

    if(m_sel) //print selected item bold
      wattron(win, A_BOLD);

    //print #ID Name
    mvwprintw(win, row, col, "%s#%04X %n[%c] %s",
        (m_sel ? " >" : "  "), m_ID, &draw_pos, (m_exp ? '-' : '+'), m_name.c_str());

    mvwchgat(win, row, 2, 5, (m_sel ? A_BOLD : A_NORMAL), 4, NULL);
    mvwchgat(win, row, draw_pos, 3, (m_sel ? A_BOLD : A_NORMAL),
        (m_exp ? 2 : 3), NULL); //highlight expand box

    //print priority state
    mvwprintw(win, row, (size_x - col) - PRINT_OFFSET_BACK, " ( %04d )   [%s]",
        m_priority, state2string(m_state).c_str());

    const unsigned int st_color[4] = { 2, 2, 4, 3 };
    mvwchgat(win, row, (size_x - col- 7), 6, (m_sel ? A_BOLD : A_NORMAL), 4, NULL);
    mvwchgat(win, row, (size_x - col- 6), 4, (m_sel ? A_BOLD : A_NORMAL),
        st_color[m_state], NULL);


    if(m_sel) //disabled bold
      wattroff(win, A_BOLD);

    if(m_exp) { //print expanded
      mvwprintw(win, ++row, col+PRINT_OFFSET_EXP, "| Description:");
      mvwprintw(win, ++row, col+PRINT_OFFSET_EXP, "-> ");
      td_screen_pos_t pos = { col + PRINT_OFFSET_EXP + PRINT_OFFSET_CMB, row };
      td_screen_pos_t end = { size_x - PRINT_OFFSET_BACK - 3, row };
      m_text_edit.set_pos(pos);
      m_text_edit.set_end(end);
      m_bottom_row += m_text_edit.print(win) + 1;
    }

    return m_bottom_row - m_top_row;
  }

  void item::sort_mode(uint8_t mode) {
    switch(mode) {
      default:
        break; //only valid modes!
      case ID:
      case STATE:
      case PRIORITY:
      case NAME:
        SORT_BY = mode;
    }
  }

  std::string item::state2string(uint8_t state) {
    switch(state) {
      default:
      case item::INVALID_STATE:
        return "----";
      case item::TODO:
        return "TODO";
      case item::WORK_IN_PRG:
        return "WIPS";
      case item::DONE:
        return "DONE";
    }
    return "INVALID";
  }

  void item::set_expanded(bool expanded) {
    m_exp = expanded;
    m_text_edit.visible(expanded);
  }

  void item::set_selected(bool selected) {
    m_sel = selected;
  }

}; // namespace todo
