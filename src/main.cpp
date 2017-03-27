#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include <assert.h>

#include "td-gui.h"

#include "sys-utils.h"


int main(int argc, char * argv[]) {
  int input;
  char * path;
  const char * homedir;

  homedir = get_home_directory();
  if(!homedir) {
    std::cerr << "error: could not get home directory" << std::endl;
    return -1;
  }
  path = get_path(homedir, todo::list::default_filename.c_str());
  if(!path) {
    std::cerr << "error: could not get path to file: " << todo::list::default_filename
              << " in " << homedir << std::endl;
    return -1;
  }
  if(!file_exists(path)) {
    std::cerr << "file: " << path << " does not exist, TODO create new" << std::endl;
    todo::list::current_file = path;
  }

  todo::gui::init();

  todo::gui gui;
  gui.lst().load(path);
  gui.update();


  while(gui.is_running()) {
    input = getch();
    gui.callback(input);
  }

  free(path);
}
