#include "../class/Transaction.hpp"
#include "../include/include.hpp"

int ft::hexToInt(const std::string& hex_str) {
  unsigned int dec = 0;

  for (std::string::const_iterator it = hex_str.begin(); it != hex_str.end();
       ++it) {
    if ((dec & 0xF0000000) != 0) {
      ft::printError("Error: hexToInt: overflow");
      throw Transaction::ErrorPageDefaultException();
    }
    char c = *it;

    if (c >= '0' && c <= '9') {
      dec = (dec << 4) + (c - '0');
    } else if (c >= 'a' && c <= 'f') {
      dec = (dec << 4) + (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
      dec = (dec << 4) + (c - 'A' + 10);
    } else {
      ft::printError("Error: hexToInt: invalid character in hex string");
      throw Transaction::ErrorPageDefaultException();
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
  const size_t size = vec.size();
  char* charArr = new char[size + 1];

  for (size_t i = 0; i < size; ++i) {
    charArr[i] = vec[i];
  }
  charArr[size] = '\0';
  return charArr;
}