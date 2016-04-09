#include "td-item.h"

#include "td-gui.h"
#include "td-except.h"

#include <ncurses.h>

uint32_t todo_item::MID = 0;
uint8_t  todo_item::SORT_BY = ID;

class item_submit_exception : public td_utils::todo_exception {
  public:
    virtual bool handle(td_utils::todo_widget * handler) {
      todo_item * item = dynamic_cast<todo_item*>(handler);
      td_utils::todo_edit * edit = dynamic_cast<td_utils::todo_edit*>(m_notifier);
      item->set_comment(edit->get_text());
      item->set_focus(NULL);
      return true;
    }
};

todo_item::todo_item(td_utils::todo_widget * parent)
         : td_utils::todo_widget(parent), m_ID(++MID), m_text_edit(this) {
  m_text_edit.visible(false);
}
todo_item::todo_item(td_utils::todo_widget * parent, std::string name,
                     std::string comment, uint8_t priority, uint8_t state)
         : td_utils::todo_widget(parent), m_ID(++MID), m_name(name), m_comment(comment),
           m_priority(priority), m_state(state), m_exp(false),
           m_sel(false), m_top_row(0), m_bottom_row(0),
           m_text_edit(this) {
  m_text_edit.visible(false);
}

todo_item::todo_item(const todo_item & item)
         : todo_widget(item.m_parent), m_ID(item.m_ID), m_name(item.m_name), m_comment(item.m_comment),
           m_priority(item.m_priority), m_state(item.m_state), m_exp(item.m_exp),
           m_sel(item.m_sel), m_top_row(item.m_top_row), m_bottom_row(item.m_bottom_row),
           m_text_edit(this) {
}

todo_item::~todo_item() {
  //nothing todo
}

bool todo_item::operator<(const todo_item& it) const {
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

int todo_item::callback(int input) {
  try {
    m_text_edit.callback(input);
  } catch(td_utils::todo_exception * except) {
    except->handle(this);
  }
  return 0;
}

#define PRINT_OFFSET_BACK 19 //TODO dynamic?
#define PRINT_OFFSET_EXP   9
#define PRINT_OFFSET_CMB   3

void todo_item::set_pos(td_screen_pos_t top) {
  m_top = top;
}

int todo_item::print(WINDOW * win) {
  int draw_pos = 0, size_x, tmp, row, col;

  row = m_top.scr_y;
  col = m_top.scr_x;
  getmaxyx(win, tmp, size_x);

  if(m_sel) //print selected item bold
    wattron(win, A_BOLD);

  //print #ID Name
  mvwprintw(win, row++, col, "%s#%04X %n[%c] %s",
      (m_sel ? "->" : "  "), m_ID, &draw_pos, (m_exp ? '-' : '+'), m_name.c_str());

  mvwchgat(win, row-1, 2, 5, (m_sel ? A_BOLD : A_NORMAL), 4, NULL);
  mvwchgat(win, row-1, draw_pos, 3, (m_sel ? A_BOLD : A_NORMAL),
      (m_exp ? 2 : 3), NULL); //highlight expand box

  //print priority state
  mvwprintw(win, row-1, (size_x - col) - PRINT_OFFSET_BACK, " ( %04d )   [%s]",
      m_priority, state2string(m_state).c_str());

  const unsigned int st_color[4] = { 2, 2, 4, 3 };
  mvwchgat(win, row-1, (size_x - col- 7), 6, (m_sel ? A_BOLD : A_NORMAL), 4, NULL);
  mvwchgat(win, row-1, (size_x - col- 6), 4, (m_sel ? A_BOLD : A_NORMAL),
      st_color[m_state], NULL);


  if(m_sel) //disabled bold
    wattroff(win, A_BOLD);

  if(m_exp) { //print expanded
    mvwprintw(win, row++, col+PRINT_OFFSET_EXP, "| Description:");
    mvwprintw(win, row, col+PRINT_OFFSET_EXP, "-> ");
    td_screen_pos_t pos = { col + PRINT_OFFSET_EXP + PRINT_OFFSET_CMB, row };
    td_screen_pos_t end = { size_x - PRINT_OFFSET_BACK - 3, row };
    m_text_edit.set_pos(pos);
    m_text_edit.set_end(end);
    m_text_edit.set_text(m_comment);
    int rows = m_text_edit.print(win);
    row += rows;
    return 2 + rows;
  }
  return 1; //used one row
}

void todo_item::sort_mode(uint8_t mode) {
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

std::string todo_item::state2string(uint8_t state) {
  switch(state) {
    default:
    case todo_item::INVALID_STATE:
      return "----";
    case todo_item::TODO:
      return "TODO";
    case todo_item::WORK_IN_PRG:
      return "WIPS";
    case todo_item::DONE:
      return "DONE";
  }
  return "INVALID";
}

void todo_item::set_expanded(bool expanded) {
  m_exp = expanded;
  m_text_edit.visible(expanded);
}

void todo_item::set_selected(bool selected) {
  m_sel = selected;
}
