#include "../include/include.hpp"

bool ft::isFileDescriptorEmpty(int fd) {
  off_t offset = lseek(fd, 0, SEEK_END);  // 파일의 끝으로 오프셋 이동

  if (offset == 0) {
    return true;
  }
  lseek(fd, 0, SEEK_SET);
  return false;
}

bool ft::isFileEmpty(const char *file_name) {
  // 1 stat
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
  return S_ISREG(fileStat.st_mode);
}

bool ft::isDirectory(const char *path) {
  struct stat st;

  stat(path, &st);
  if (S_ISDIR(st.st_mode)) {
    return true;
  }
  return false;
}