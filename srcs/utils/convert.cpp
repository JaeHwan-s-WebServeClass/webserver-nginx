#include "../include/include.hpp"

int ft::hexToInt(const std::string& hex_str) {
  unsigned int dec = 0;

  for (std::string::const_iterator it = hex_str.begin(); it != hex_str.end();
       ++it) {
    if ((dec & 0xF0000000) != 0) {
      throw std::string("Error: hexToInt: overflow");
    }
    char c = *it;

    if (c >= '0' && c <= '9') {
      dec = (dec << 4) + (c - '0');
    } else if (c >= 'a' && c <= 'f') {
      dec = (dec << 4) + (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
      dec = (dec << 4) + (c - 'A' + 10);
    } else {
      throw std::string("Error: hexToInt: invalid character in hex string");
    }
  }
  return dec;
}

std::string ft::intToStr(int num) {
  std::stringstream out;
  out << num;
  return out.str();
}

const char* ft::vecToCharArr(const std::vector<char>& vec) {
  // 벡터가 비어있을 경우 빈 문자열 반환
  if (vec.empty()) {
    return "";
  }
  // 벡터의 첫 번째 요소의 주소 반환
  // c++11 version
  return vec.data();
}