#include "../include/include.hpp"

int hexToInt(const std::string& hex_str) {
  int dec = 0;

  for (std::string::const_iterator it = hex_str.begin(); it != hex_str.end();
       ++it) {
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