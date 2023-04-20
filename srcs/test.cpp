#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// using namespace std;
// #define CHUNK_SIZE false
// #define CHUNK_ENTITY true

int chunk_size = 0;
std::string hex_str = "";
std::vector<char> tmp_entity;
bool CR = false;
bool LF = false;
bool CRLF = false;
bool ENTITY_DONE = false;

int hexToInt(const std::string& hex_str) {
  unsigned int dec = 0;
  std::cout << "hex str : " << hex_str << std::endl;
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

void addChunkedEntity(char* buf, int read_len) {
  int i = 0;

  while (i < read_len) {
    std::cout << "\nindex(outer loop): " << i << std::endl;
    // std::cout << "buf: " << buf[i] << std::endl;
    // 1. checking chunk size
    if (chunk_size == 0 || ENTITY_DONE == false) {
      while (buf[i]) {
        std::cout << "\nindex(inner loop): " << i << std::endl;
        
        if (CR && i == 0) {
          if (buf[0] == '\n') {
            LF = true;
            CR = true;
            // break;
          } else {
            throw std::string("error : LF is not set when index == 0 \n");
          }
        }
        if (!CR) {
          if (buf[i] == '\r') {
            CR = true;
            if (i + 1 < read_len) {
              if (buf[i + 1] == '\n') {
                LF = true;
                CR = true;
                i++;
                // break;
              } else {
                throw std::string("error : LF is not set when index = i + 1\n");
              }
            } else {
              break;
            }
          } else {
            hex_str += buf[i];
          }
        }
		if (LF == true) {
          chunk_size = hexToInt(hex_str) + 2;
          hex_str = "";
          std::cout << "chunk size : " << chunk_size << std::endl;
          LF = false;
          CR = false;
          break;
        }
        i++;
      }
    } else {  // 2. checking chunk content
      while (buf[i] && chunk_size--) {
        if (chunk_size == 2 && !(buf[i] == '\r'))
          throw std::string("error : Not CR after entity done\n");
        else if (chunk_size == 1 && !(buf[i] == '\n'))
          throw std::string("error : Not LF after entity done\n");
        else
          tmp_entity.push_back(buf[i]);
        i++;
      }
    }
    i++;
  }
}

#define BUFFER_SIZE 7

int main() {
  char buf[1024] =
      //"4\r\nWiki    \r\n6\r\npe    dia \r\nE    \r\nin \r\n    \r\nchunk
      // s.\r\n0\r\n    \r\n";
      "4\r\nWiki\r\n6\r\npedia \r\nE\r\nin \r\n\r\nchunks.\r\n0\r\n\r\n";
  int i = 0;
  try {
    while (i < 5) {
      addChunkedEntity(buf + (BUFFER_SIZE * i), BUFFER_SIZE);
      //   std::cout << "\n-------------------------------------" << std::endl;
      i++;
    }
  } catch (const std::string e) {
    std::cerr << e << '\n';
  }
  std::cout << "tmp_entity: ";
  for (std::vector<char>::iterator it = tmp_entity.begin();
       it != tmp_entity.end(); it++) {
    std::cout << *it;
  }
}

/*
Encoded data :
4\r\n        (bytes to send)
Wiki\r\n     (data)
6\r\n        (bytes to send)
pedia \r\n   (data)
E\r\n        (bytes to send)
in \r\n\r\nchunks.\r\n  (data)
0\r\n        (final byte - 0)
\r\n         (end message)

Decoded data :
Wikipedia in

chunks.
*/

/*
        1. 정상적으로 다 받고 chunk-content에서 buf가 끝난 경우 (일단 문제
   없는걸로)
        2. chunk-size를 받는 도중에 buf가 끝난 경우
                 ex) 3A\r\n
                 2-1. 3까지 읽었는데 buf가 끝난 경우
                 2-2. 3A까지 읽었는데 buf가 끝난 경우
                 2-3. \r까지 읽었는데 buf가 끝난 경우
        3. chunk를 받는 중 끝난 경우 (chunk_size 로 체크)
*/