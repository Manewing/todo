#include <iostream>
#include <stdio.h>
#include <ncurses.h>

#include "td-gui.h"
#include "td-list.h"
#include "td-cmd.h"


int main(int argc, char * argv[]) {
  todo_list list;
  td_utils::todo_gui gui(list);
  list.load(todo_list::default_filename);
  gui.update();

  int input;

  while(gui.is_running()) {
    input = getch();
    switch(gui.get_focus()) {
      case td_utils::todo_gui::NO_FOCUS:
        switch(input) {
          case td_utils::todo_gui::CMDK_ENTER:
            gui.expand_item();
            break;
          case td_utils::todo_gui::CMDK_START_CMD: // ':'
            gui.set_focus(td_utils::todo_gui::CMD_LINE_FOCUS);
            gui.start_cmdline();
            break;
          case td_utils::todo_gui::CMDK_EXIT:  // '0'
            gui.quit();
            break;
          case td_utils::todo_gui::CMDK_DELETE: //'d'
            td_utils::command_start(&td_utils::delete_item_cmd);
            break;
          case td_utils::todo_gui::CMDK_EDIT: // 'e'
            gui.expand_item();
            gui.set_focus(td_utils::todo_gui::ITEM_FOCUS);
            break;
          case td_utils::todo_gui::CMDK_UNDO: // 'u'
            td_utils::command_start(&td_utils::undo_cmd);
            break;
          case 0x73:
            td_utils::command_start(&td_utils::set_item_cmd);
            break;
          case td_utils::todo_gui::CMDK_ARROW_DOWN:
            gui.scroll_down();
            break;
          case td_utils::todo_gui::CMDK_ARROW_UP:
            gui.scroll_up();
            break;
          default:
            break;
        }
        break;
      case td_utils::todo_gui::ITEM_FOCUS:
        switch(input) {
          case td_utils::todo_gui::CMDK_ESCAPE:
            gui.set_focus(td_utils::todo_gui::NO_FOCUS);
            break;
          default:
            if(input >= 0x20 && input <= 0x7E)
              break;
            mvprintw(gui.m_row-1, 0, "0x%003x", input);
            refresh();
            break;
        }
        break;
      case td_utils::todo_gui::CMD_LINE_FOCUS:
        switch(input) {
          case td_utils::todo_gui::CMDK_ENTER:
            execute_cmdline(gui.get_cmdline(), &gui, &list);
            gui.add_cmdline_history();
            gui.clear_cmdline();
            gui.set_focus(td_utils::todo_gui::NO_FOCUS);
            break;
          case td_utils::todo_gui::CMDK_ESCAPE:
            gui.clear_cmdline();
            gui.set_focus(td_utils::todo_gui::NO_FOCUS);
            break;
          case td_utils::todo_gui::CMDK_ARROW_UP:
            gui.get_prev_cmdline();
            break;
          case td_utils::todo_gui::CMDK_ARROW_DOWN:
            gui.get_next_cmdline();
            break;
          case td_utils::todo_gui::CMDK_ARROW_LEFT:
            gui.cleft_cmdline();
            break;
          case td_utils::todo_gui::CMDK_ARROW_RIGHT:
            gui.cright_cmdline();
            break;
          case td_utils::todo_gui::CMDK_BACKSPACE:
            gui.bsp_cmdline();
            break;
          default:
            if(input >= 0x20 && input <= 0x7E)
              gui.add_cmdline((char)(input & 0xFF));
            else {
              mvprintw(gui.m_row-1, 0, "0x%003x", input);
              refresh();
            }
            break;
        }
        break;
      default:
        gui.set_focus(td_utils::todo_gui::NO_FOCUS);
        break;
    }
    td_utils::command_check_list(input, &gui, &list);
  }
}
