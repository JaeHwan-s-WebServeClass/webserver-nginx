#include "../include/include.hpp"

bool ft::findSuffix(std::string filename, std::string suffix) {
  if (filename.length() <= suffix.length()) {
    return false;
  }
	std::string::size_type pos = filename.rfind(suffix);
  return ((pos != std::string::npos) && (pos == filename.length() - suffix.length()));
}
