#include "td-list.h"

#include <string.h>
#include <fstream>

#define MAX_LENGTH 512 //TODO into td-item

#define TD_LIST_TAG 0xB0

typedef struct {
  uint8_t __tag;
  uint32_t __item_count;
  uint8_t __sort_mode;
  uint32_t __mid;
  uint8_t __reserved[22];
} __attribute__ ((packed)) td_list_file_header;

#define TD_LIST_FILE_HEADER_INITIALIZER \
  { TD_LIST_TAG, 0, item::ID, item::get_mid(), {0} }

#define TD_ITEM_TAG 0x13

typedef struct {
  uint8_t __tag;
  uint8_t __prio;
  uint8_t __state;
  uint8_t __name_len;
  uint16_t __desc_len;
  uint32_t __id;
  uint8_t __reserved[22];
} __attribute__ ((packed)) td_item_file_header;

#define TD_ITEM_FILE_HEADER_INITIALIZER \
  { TD_ITEM_TAG, item::NORMAL, item::TODO, 0, 0, 0, {0} }

namespace todo {

  const std::string list::default_filename = ".list.td";
  std::string list::current_file = "";

  class list_update_exception : public exception {
    public:
      list_update_exception(widget * notifier)
        : exception(notifier) {}
      virtual void process(widget * handler) {
        list * l = dynamic_cast<list*>(handler);
        //m_notifier->print(list->get_win());
        handler->print(stdscr);
      }
  };

  list::list()
      : frame(), std::list<item>(), m_sel(end()) {
    set_update(new urecall_exception(this));
  }

  list::list(const std::string & file_name)
      : frame(), std::list<item>(), m_sel(end()) {
    load(file_name);
    set_update(new urecall_exception(this));
  }

  list::~list() {
  }

  void list::add_item(item item) {
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

  void list::remove_item(todo::item * item) {
    // check if item was selected item
    if(*m_sel == *item) {
      std::list<todo::item>::iterator it = m_sel;
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

  void list::undo_remove() {
    if(m_removed_items.empty())
      return;
    item item = m_removed_items.back();
    item.set_selected(false);
    m_removed_items.pop_back();
    push_back(item);
    sort();
    if(m_sel == end()) {
      m_sel = begin();
      m_sel->set_selected(true);
    }
  }

  void list::callback_handler(int input) {
    widget::log_debug("list", "in callback_handler");
    switch(input) {
      /*case CMDK_ARROW_UP:
        select_prev();
        break;
      case CMDK_ARROW_DOWN:
        select_next();
        break;
      case CMDK_ENTER:
        expand_selected();
        break; */
      case CMDK_EDIT:
        widget::log_debug("list", "Edit...");
        set_focus(get_selection());
        if(m_sel == end())
          m_sel->set_expanded(true);
        break;
    }
    update();
  }

  void list::print_items() {
    std::list<item>::iterator it;
    td_screen_pos_t top = { 0, 1 };
    for(it = begin(); it != end(); it++) {
      it->set_pos(top);
      top.scr_y += it->print(m_win);
    }
  }

  int list::print(WINDOW * win) {
    wclear(m_win);
    print_items();
    return frame::print(win);
  }

  uint8_t list::load(const std::string & file_name) {
    td_list_file_header l_fheader;
    td_item_file_header i_fheader;
    std::ifstream file;

    file.open(file_name.c_str(), std::ios::binary | std::ios::in);
    if(!file) return -1;

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
      todo::item item(name, comment, i_fheader.__prio, i_fheader.__state);
      item.set_id(i_fheader.__id);
      add_item(item);
      delete [] name;
      delete [] comment;
    }

    // mid gets change due to constructor set after all items are created
    item::set_mid(l_fheader.__mid);

    file.close();
    list::current_file = file_name;
    return 0;
  }

  uint8_t list::save(const std::string & file_name) {
    td_list_file_header l_fheader = TD_LIST_FILE_HEADER_INITIALIZER;
    td_item_file_header i_fheader = TD_ITEM_FILE_HEADER_INITIALIZER;
    std::ofstream file;

    file.open(file_name.c_str(), std::ios::binary | std::ios::out);
    if(!file) return -1;

    // set data in header to write
    l_fheader.__item_count = size();
    l_fheader.__sort_mode = item::get_sort_mode();
    l_fheader.__mid = item::get_mid();

    // write header data to file
    file.write((const char*)(&l_fheader), sizeof(td_list_file_header));

    std::list<item>::const_iterator it;
    for(it = begin(); it != end(); it++) {
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
    return 0;
  }

  int list::select_next() {
    if(m_sel != end())
      m_sel->set_selected(false);
    else
      return 0;
    if(++m_sel == end())
      m_sel = --end();
    m_sel->set_selected(true);
    return m_sel->get_bottom_row();
  }

  int list::select_prev() {
    if(m_sel != end())
      m_sel->set_selected(false);
    else
      return 0;
    if(m_sel != begin())
      m_sel--;
    m_sel->set_selected(true);
    return m_sel->get_top_row();
  }

  void list::expand_selected() {
    if(m_sel == end())
      return;
    m_sel->set_expanded(!m_sel->is_expanded());
  }

  item * list::get_selection() {
    if(m_sel == end())
      return NULL;
    return &(*m_sel);
  }

}; // namespace todo
