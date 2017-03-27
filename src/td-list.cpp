#include "td-list.h"

#include <string.h>
#include <sstream>
#include <fstream>
#include <algorithm>

#define TD_LIST_TAG 0xB0

#define TD_LIST_FILE_HEADER_INITIALIZER \
  { TD_LIST_TAG, 0, item::ID, item::get_mid(), {0} }

#define TD_ITEM_TAG 0x13

#define TD_ITEM_FILE_HEADER_INITIALIZER \
  { TD_ITEM_TAG, item::NORMAL, item::TODO, 0, 0, 0, {0} }

namespace {

  typedef struct {
    uint8_t __tag;
    uint32_t __item_count;
    uint8_t __sort_mode;
    uint32_t __mid;
    uint8_t __reserved[22];
  } __attribute__ ((packed)) td_list_file_header;

  typedef struct {
    uint8_t __tag;
    uint8_t __prio;
    uint8_t __state;
    uint8_t __name_len;
    uint16_t __desc_len;
    uint32_t __id;
    uint8_t __reserved[22];
  } __attribute__ ((packed)) td_item_file_header;

};


namespace todo {

  std::string list::current_file = "";
  std::string const list::default_filename = ".list.td";

  list::list():
    frame(),
    m_buf(),
    m_list(),
    m_sel(m_list.end()),
    m_history(),
    m_scroll(0) {

    /* nothing todo */

  }

  list::~list() {
    // cleanup
    for (auto it : m_buf)
      delete it;
  }

  void list::sort() {

    // sort items in list depending on item sorting mode
    m_list.sort(
      [](item* i1, item* i2) {
        return *i1 < *i2;
      }
    );

  }

  void list::add(item* i) {
    // update history before adding item
    update_history();

    // if list is not empty deselect current item
    if (m_sel != end())
      (*m_sel)->set_selected(false);

    // add new item and sort list again
    m_buf.insert(i);
    m_list.push_back(i);
    m_sel = --end();
    (*m_sel)->set_selected(true);

    // sort list
    sort();
  }

  void list::remove(item * i) {
    // find item
    auto it = std::find(m_list.begin(), m_list.end(), i);
    if (it == m_list.end())
      return;

    // update history before removing item
    update_history();

    // make sure item is unselected
    (*it)->set_selected(false);

    // remove item and update selection
    m_sel = m_list.erase(it);
    if (m_sel != m_list.end())
      (*m_sel)->set_selected(true);
    else if (m_sel != m_list.begin())
      (*--m_sel)->set_selected(true);
  }

  void list::undo() {
    if (m_sel != end())
      (*m_sel)->set_selected(false);

    m_list = m_history.back();
    m_sel = m_list.begin();
    (*m_sel)->set_selected(true);
    m_history.pop_back();
  }

  void list::select_next() {
    // item selected?
    if (m_sel == end())
      return;

    (*m_sel)->set_selected(false);
    if(++m_sel == end())
      m_sel = --end();
    (*m_sel)->set_selected(true);

    update_scroll();
  }

  void list::select_prev() {
    if(m_sel == end())
      return;

    (*m_sel)->set_selected(false);
    if(m_sel != begin())
      m_sel--;
    (*m_sel)->set_selected(true);

    update_scroll();
  }

  void list::expand_selected(bool force) {
    if(m_sel == end())
      return;
    (*m_sel)->set_expanded(force ? true : !(*m_sel)->is_expanded());
  }

  item * list::get_selection() {
    if(m_sel == end())
      return NULL;
    return *m_sel;
  }

  int list::print(WINDOW* win) {
    wclear(m_win);

    int top = 1 + m_scroll;
    for (auto it : m_list) {
      it->set_pos(top);
      top += it->print(m_win);
    }

    return frame::print(win);
  }

  bool list::load(const std::string & file_name) {
    td_list_file_header l_fheader;
    td_item_file_header i_fheader;
    std::ifstream file;

    file.open(file_name.c_str(), std::ios::binary | std::ios::in);
    if(!file) return false;

    // get char pointer of list header and read it in directly
    file.read((char*)(&l_fheader), sizeof(td_list_file_header));
    if(l_fheader.__tag != TD_LIST_TAG) return -2; //file does not represent todo list

    // set sort mode
    item::sort_mode(l_fheader.__sort_mode);

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

      // create item and add item
      item* i = new item(name, comment, i_fheader.__prio, i_fheader.__state);
      i->set_id(i_fheader.__id);
      add(i);

      delete [] name;
      delete [] comment;
    }

    // mid gets change due to constructor set after all items are created
    item::set_mid(l_fheader.__mid);

    file.close();
    list::current_file = file_name;
    return true;
  }

  bool list::save(const std::string & file_name) {
    td_list_file_header l_fheader = TD_LIST_FILE_HEADER_INITIALIZER;
    td_item_file_header i_fheader = TD_ITEM_FILE_HEADER_INITIALIZER;
    std::ofstream file;

    file.open(file_name.c_str(), std::ios::binary | std::ios::out);
    if(!file) return false;

    // set data in header to write
    l_fheader.__item_count = m_list.size();
    l_fheader.__sort_mode = item::get_sort_mode();
    l_fheader.__mid = item::get_mid();

    // write header data to file
    file.write((const char*)(&l_fheader), sizeof(td_list_file_header));

    for (auto it : m_list) {
      i_fheader.__prio = it->get_priority();
      i_fheader.__state = it->get_state();
      i_fheader.__name_len = it->get_name().size();
      i_fheader.__desc_len = it->get_comment().size();
      i_fheader.__id = it->get_id();
      file.write((const char*)(&i_fheader), sizeof(td_item_file_header));
      file.write(it->get_name().c_str(), i_fheader.__name_len);
      file.write(it->get_comment().c_str(), i_fheader.__desc_len);
    }
    file.close();
    return true;
  }

  void list::update_history() {
    m_history.push_back(m_list);
  }

  void list::update_scroll() {
    const int top_row = (*m_sel)->get_top_row();
    const int bottom_row = (*m_sel)->get_bottom_row() + 1;
    const int rows = m_bottom.scr_y - m_top.scr_y;

    if (top_row < 1)
      m_scroll -= top_row - 1;
    else if (bottom_row >= rows)
      m_scroll -= bottom_row - rows;
  }

}; // namespace todo
