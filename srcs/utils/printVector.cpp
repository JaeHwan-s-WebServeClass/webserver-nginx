#include "../include/include.hpp"

void	ft::printVector(const std::vector<std::string> &vec) {
    std::vector<std::string>::const_iterator it;

    for (it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}
