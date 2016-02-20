#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>

const char * get_home_directory() {
  uid_t uid = getuid();
  struct passwd * pw = getpwuid(uid);
  if(!pw) return NULL;
  return pw->pw_dir;
}

char * get_path(const char * dir, const char * file) {
  size_t dir_len, file_len;
  char * path = NULL;
  dir_len = strlen(dir);
  file_len = strlen(file);
  path = (char*)malloc(dir_len + file_len + 2);
  if(!path) return NULL;
  strcpy(path, dir);
  path[dir_len] = '/';
  strcpy(path+dir_len+1, file);
  return path;
}

int file_exists(const char * path) {
  return access(path, F_OK) != -1;
}
