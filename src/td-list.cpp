#include "td-list.h"

#include <string.h>
#include <fstream>

#define MAX_LENGTH 512 //TODO into td-item

const std::string todo_list::default_filename = "test.txt";

todo_list::todo_list() : std::list<todo_item>(), m_sel(end()) {
}

todo_list::todo_list(const std::string & file_name)
          : std::list<todo_item>(), m_sel(end()) {
  load(file_name);
}

todo_list::~todo_list() {
}

void todo_list::add_item(todo_item item) {
  // if list is not empty deselect current item
  if(m_sel != end())
    m_sel->set_selected(false);
  // add new item and sort list again
  push_back(item);
  sort();
  // set selection iterator to last entry of list
  m_sel = --end();
  m_sel->set_selected(true);
}

void todo_list::remove_item(todo_item *item) {
  // check if item was selected item
  if(*m_sel == *item) {
    std::list<todo_item>::iterator it = m_sel;
    if(++it != end()) { // not the last item?
      m_sel = it;
      m_sel->set_selected(true);
    }
    else if(m_sel != begin()) { // not the first item?
      --m_sel;
      m_sel->set_selected(true);
    }
    else // the only item
      m_sel = end();
  }
  m_removed_items.push_back(*item);
  remove(*item);
  sort();
}

void todo_list::undo_remove() {
  if(m_removed_items.empty())
    return;
  todo_item item = m_removed_items.back();
  item.set_selected(false);
  m_removed_items.pop_back();
  push_back(item);
  sort();
  if(m_sel == end()) {
    m_sel = begin();
    m_sel->set_selected(true);
  }
}

int todo_list::print(int row, int col, int size_x, int size_y) {
  std::list<todo_item>::iterator it;
  for(it = begin(); it != end(); it++) {
    row = row + it->print(row, col, size_x, size_y);
    //if(row >= size_y) //only print until end of screen
    //  break;
  }
  return row;
}

uint8_t todo_list::load(const std::string & file_name) {
  std::ifstream file;
  file.open(file_name.c_str(), std::ios_base::in);
  if(!file) {
    return 1;
  }
  char sort_mode, priority, state;
  std::string name, comment;
  file >> sort_mode >> priority >> state;
  todo_item::sort_mode(sort_mode - 'A');
  while(!file.eof()) {
    std::getline(file, name);
    name = name.substr(1, name.length()); //get rid of space
    std::getline(file, comment);
    priority -= 'A'; // human readable and writeable
    state -= 'A';
    add_item(todo_item(name, comment, priority, state));
    file >> priority >> state; //read after EOF so flag is set
  }
  file.close();
  return 0;
}

uint8_t todo_list::save(const std::string & file_name) {
  std::ofstream file;
  file.open(file_name.c_str(), std::ios_base::out);
  if(!file)
    return 1;
  file << (char)(todo_item::get_sort_mode() + 'A') << " ";
  std::list<todo_item>::const_iterator it;
  for(it = begin(); it != end(); it++) {
    file << (char)(it->get_priority() + 'A')
         << " " << (char)(it->get_state() + 'A')
         << " " << it->get_name() << std::endl
         << it->get_comment() << std::endl;
  }
  file.close();
  return 0;
}

int todo_list::select_next() {
  if(m_sel != end())
    m_sel->set_selected(false);
  else
    return 0;
  if(++m_sel == end())
    m_sel = --end();
  m_sel->set_selected(true);
  return m_sel->get_bottom_row();
}

int todo_list::select_prev() {
  if(m_sel != end())
    m_sel->set_selected(false);
  else
    return 0;
  if(m_sel != begin())
    m_sel--;
  m_sel->set_selected(true);
  return m_sel->get_top_row();
}

void todo_list::expand_selected() {
  if(m_sel == end())
    return;
  m_sel->set_expanded(!m_sel->is_expanded());
}

todo_item * todo_list::get_selection() {
  if(m_sel == end())
    return NULL;
  return &(*m_sel);
}
