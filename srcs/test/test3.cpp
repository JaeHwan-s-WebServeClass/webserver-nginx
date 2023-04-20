#include <sys/fcntl.h>
#include <unistd.h>

#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int chunk_size = 0;
std::string hex_str = "";
std::vector<char> entity;

int hexToInt(const std::string& hex_str) {
  unsigned int dec = 0;
  // std::cout << "hex str : " << hex_str << std::endl;
  for (std::string::const_iterator it = hex_str.begin(); it != hex_str.end();
       ++it) {
    if ((dec & 0xF0000000) != 0) throw std::string("overflow");

    char c = *it;

    if (c >= '0' && c <= '9') {
      dec = (dec << 4) + (c - '0');
    } else if (c >= 'a' && c <= 'f') {
      dec = (dec << 4) + (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
      dec = (dec << 4) + (c - 'A' + 10);
    } else {
      throw std::string("Invalid character in hex string");
    }
  }
  return dec;
}

#define BUFFER_SIZE 6

void addChunkedEntity(char buf[BUFFER_SIZE], size_t read_len) {
  size_t i = 0;

  while (i < read_len) {
    if (chunk_size == 0) {
      hex_str += buf[i];
      if (hex_str.back() == '\n') {
        hex_str.pop_back();
        hex_str.pop_back();
        chunk_size = hexToInt(hex_str);
        if (chunk_size == 0) {
          //  setEntityDone();
          return;
        }
        chunk_size += 2;
        hex_str = "";
      }
    } else {
      entity.push_back(buf[i]);
      --chunk_size;
      if (chunk_size == 0) {
        entity.pop_back();
        entity.pop_back();
      }
    }
    ++i;
  }
}

//"4\r\nWiki\r\n6\r\npedia \r\nE\r\nin \r\n\r\nchunks.\r\n0\r\n\r\n"

int main() {
  int fd = open("test", O_RDONLY);

  int read_len = true;
  char buf[BUFFER_SIZE];
  try {
    while (read_len > 0) {
      read_len = read(fd, buf, BUFFER_SIZE);
      ;
      addChunkedEntity(buf, read_len);
      bzero(buf, BUFFER_SIZE);
    }
  } catch (const std::string e) {
    std::cerr << e << '\n';
  }
  // std::cout << "entity: \n";
  for (std::vector<char>::iterator it = entity.begin(); it != entity.end();
       it++) {
    std::cout << *it;
  }
}
