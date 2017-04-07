#include <iostream>
#include <ncurses.h>

#include "td-gui.h"

int main(int argc, char * argv[]) {
  int input;
  std::string path, homedir;

  // get home directory
  homedir = ::todo::utils::get_homedir();
  if(homedir.length() == 0) {
    std::cerr << "error: could not get home directory" << std::endl;
    return -1;
  }

  // get path to default todo list file
  path = ::todo::utils::get_path(homedir, ::todo::list::default_filename);

  // check if default file already exists
  if(!::todo::utils::file_exists(path)) {
    std::cerr << "creating new file: " << path << std::endl;

    // set current path
    ::todo::list::current_file = path;

    // just write empty list to file
    ::todo::list lst;
    lst.save(path);
  }

  // TODO: argparsing
  if (argc == 2) {
    if (!::todo::utils::file_exists(argv[1])) {
      std::cout << argv[0] << ": can not open: " << argv[1] << std::endl;
      return -1;
    }

    path = argv[1];
  }

  todo::gui& gui = todo::gui::get();
  gui.lst().load(path);
  gui.update();


  while(gui.is_running()) {
    input = getch();
    gui.callback(input);
  }

  todo::gui::free();
}
