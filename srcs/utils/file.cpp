#include "../include/include.hpp"

bool ft::isFileEmpty(int fd) {
  char buffer[1];
  ssize_t bytesRead = read(fd, buffer, sizeof(buffer));

  if (bytesRead == -1) {
    // 파일 읽기 실패
    return false;
  }

  if (bytesRead == 0) {
    // 파일 비어있음
    return true;
  }
    //  파일이 비어있지 않음
  return false;
}

bool ft::isFileDescriptorValid(int fd) {
  struct stat fileStat;
  if (fstat(fd, &fileStat) == -1) {
    return false;
  }

  return S_ISREG(fileStat.st_mode);
}