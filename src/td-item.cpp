#include "td-item.h"
#include "td-gui.h"
#include <ncurses.h>

uint32_t todo_item::MID = 0;
uint8_t  todo_item::SORT_BY = ID;

todo_item::todo_item()
         : m_ID(++MID) {
}
todo_item::todo_item(std::string name,
                     std::string comment,
                     uint8_t priority,
                     uint8_t state)
         : m_ID(++MID), m_name(name), m_comment(comment),
           m_priority(priority), m_state(state), m_exp(false),
           m_sel(false), m_top_row(0), m_bottom_row(0), m_line_size(0) {
}

void todo_item::make(std::string name,
                     std::string comment,
                     uint8_t priority,
                     uint8_t state) {
  m_name = name;
  m_comment = comment;
  m_priority = priority;
  m_state = state;
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

#define PRINT_OFFSET_BACK 19 //TODO dynamic?
#define PRINT_OFFSET_EXP   9
#define PRINT_OFFSET_CMB   2

int todo_item::format_comment(int new_line_size) {
  const char * new_line = "-\n            ";
  if(m_line_size == new_line_size)
    return m_rows;
  int insert_off = 0, pos = 1;
  m_rows = 1;
  m_line_size = new_line_size;
  m_comment_fmt = m_comment;
  for(int l = m_comment.length(); l > m_line_size; l -= m_line_size) {
    m_comment_fmt.insert((pos++)*m_line_size + insert_off, new_line);
    m_rows++;
  }
  return m_rows;
}

int todo_item::print(int row, int col, int size_x,
    int size_y __attribute__ ((unused)) ) {
  int draw_pos = 0;
  m_top_row = row;

  if(m_sel) //print selected item bold
    attron(A_BOLD);

  //print #ID Name
  mvprintw(row++, col, "%s#%04d %n[%c] %s",
      (m_sel ? "->" : "  "), m_ID, &draw_pos, (m_exp ? '-' : '+'), m_name.c_str());

  mvchgat(row-1, 2, 5, (m_sel ? A_BOLD : A_NORMAL), 4, NULL);
  mvchgat(row-1, draw_pos, 3, (m_sel ? A_BOLD : A_NORMAL),
      (m_exp ? 2 : 3), NULL); //highlight expand box

  //print priority state
  mvprintw(row-1, (size_x - col) - PRINT_OFFSET_BACK, " ( %04d )   [%s]",
      m_priority, state2string(m_state).c_str());

  const unsigned int st_color[4] = { 2, 2, 4, 3 };
  mvchgat(row-1, (size_x - col- 7), 6, (m_sel ? A_BOLD : A_NORMAL), 4, NULL);
  mvchgat(row-1, (size_x - col- 6), 4, (m_sel ? A_BOLD : A_NORMAL),
      st_color[m_state], NULL);


  if(m_sel) //disabled bold
    attroff(A_BOLD);

  if(m_exp) { //print expanded
    mvprintw(row++, col+PRINT_OFFSET_EXP, "| Description:");
    //TODO use addstr with param n instead of format function
    int rows = format_comment(size_x - PRINT_OFFSET_CMB - PRINT_OFFSET_EXP - 3);
    mvprintw(row, col+PRINT_OFFSET_EXP, "-> %s", m_comment_fmt.c_str());
    row += rows;
    m_bottom_row = ++row;
    return 2 + rows;
  }
  m_bottom_row = row;
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
  switch(state) { //TODO add color code
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
}

void todo_item::set_selected(bool selected) {
  m_sel = selected;
}

/*void todo_item::edit_write(char ch) {
  std::string str = "";
  str += ch;
  m_comment.instert(m_comment_pos++, str);
  //TODO update gui
  curs_set(1);
}*/
