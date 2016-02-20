#include "td-list.h"

#include <string.h>
#include <fstream>

#define MAX_LENGTH 512 //TODO into td-item

#define TD_LIST_TAG 0xB0

typedef struct {
  uint8_t __tag;
  uint32_t __item_count;
  uint8_t __sort_mode;
} __attribute__ ((packed)) td_list_file_header;

#define TD_LIST_FILE_HEADER_INITIALIZER \
  { TD_LIST_TAG, 0, todo_item::ID }

#define TD_ITEM_TAG 0x13

typedef struct {
  uint8_t __tag;
  uint8_t __prio;
  uint8_t __state;
  uint8_t __name_len;
  uint16_t __desc_len;
} __attribute__ ((packed)) td_item_file_header;

#define TD_ITEM_FILE_HEADER_INITIALIZER \
  { TD_ITEM_TAG, todo_item::NORMAL, todo_item::TODO, 0, 0 }

const std::string todo_list::default_filename = ".list.td";
std::string todo_list::current_file = "";

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
  td_list_file_header l_fheader;
  td_item_file_header i_fheader;
  std::ifstream file;

  file.open(file_name.c_str(), std::ios::binary | std::ios::in);
  if(!file) return -1;

  // get char pointer of list header and read it in directly
  file.read((char*)(&l_fheader), sizeof(td_list_file_header));
  if(l_fheader.__tag != TD_LIST_TAG) return -2; //file does not represent todo list

  // set sort mode
  todo_item::sort_mode(l_fheader.__sort_mode);

  // for all items in list
  for(uint32_t l = 0; l < l_fheader.__item_count && !file.eof(); l++) {
    // read in item header
    file.read((char*)(&i_fheader), sizeof(td_item_file_header));

    // allocate memory for name and comment
    char * name = new char[i_fheader.__name_len+1];
    char * comment = new char[i_fheader.__desc_len+1];
    // read in name and comment
    file.read(name, i_fheader.__name_len);
    file.read(comment, i_fheader.__desc_len);
    name[i_fheader.__name_len] = 0;
    comment[i_fheader.__desc_len] = 0;
    add_item(todo_item(name, comment, i_fheader.__prio, i_fheader.__state));
    delete [] name;
    delete [] comment;
  }
  file.close();
  todo_list::current_file = file_name;
  return 0;
}

uint8_t todo_list::save(const std::string & file_name) {
  td_list_file_header l_fheader = TD_LIST_FILE_HEADER_INITIALIZER;
  td_item_file_header i_fheader = TD_ITEM_FILE_HEADER_INITIALIZER;
  std::ofstream file;

  file.open(file_name.c_str(), std::ios::binary | std::ios::out);
  if(!file) return -1;

  // set data in header to write
  l_fheader.__item_count = size();
  l_fheader.__sort_mode = todo_item::get_sort_mode();

  // write header data to file
  file.write((const char*)(&l_fheader), sizeof(td_list_file_header));

  std::list<todo_item>::const_iterator it;
  for(it = begin(); it != end(); it++) {
    i_fheader.__prio = it->get_priority();
    i_fheader.__state = it->get_state();
    i_fheader.__name_len = it->get_name().size();
    i_fheader.__desc_len = it->get_comment().size();
    file.write((const char*)(&i_fheader), sizeof(td_item_file_header));
    file.write(it->get_name().c_str(), i_fheader.__name_len);
    file.write(it->get_comment().c_str(), i_fheader.__desc_len);
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
