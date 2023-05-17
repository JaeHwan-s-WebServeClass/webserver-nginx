#include "../include/include.hpp"

bool ft::isFileEmpty(const char *file_name) {
  struct stat st;
  stat(file_name, &st);
  if (st.st_size == 0) {
    return true;
  }
  return false;
}

bool ft::isFileDescriptorValid(int fd) {
  struct stat fileStat;

  if (fstat(fd, &fileStat) == -1) {
    return false;
  }
  return true;
}

bool ft::isDirectory(std::string file_path) {
  DIR *dir;

  if ((dir = opendir(file_path.c_str())) == NULL) {
    return false;
  } else {
    closedir(dir);
    return true;
  }
}