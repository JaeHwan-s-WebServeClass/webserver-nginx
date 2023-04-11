#include "Request.hpp"


Request::Request():raw_head(""), is_end_head(0) { }

// void Request::setRawMsg(const std::string &update) {
// 	while (std::getline(ss, str, '\n')) {

// 	 if (분리한 문자열.length() == 0 || (분리한 문자열.length() == 1 && (분리한 문자열 == "\r")))
// 	 	1. 헤드 끝났다 플래그 켜라
// 		2. 헤드 파싱 gogo
// 	 else if (헤더 안끝남?)
// 	 	raw_head += str;   
// 	 else
// 	 	body_vec.push_back(str);
// 	}
// }

//---- getter/setter --------------------------------------------
void Request::setRawMsg(const char * read_msg) {
	std::istringstream buf;
	buf.str(read_msg);

	std::string line;
	while(std::getline(buf, line, '\n')) {
		if (line.length() == 0 || line == "\r") {
			this->is_end_head = true;
			this->parserHead();
		} else if (!this->is_end_head) {
			this->raw_head += line;
			if(!buf.eof()) {
				this->raw_head += '\n';
			}
		} else {
			this->entity.push_back(line);
		}
	}
}

std::string Request::getRawMsg() {
	return this->raw_head;
}

void Request::clearSetRawMsg() {
	this->raw_head.clear();
}

void Request::toString() {
	std::cout << BLU << this->raw_head << std::endl << DFT;
	//std::copy(entity.begin(), entity.end(), std::ostream_iterator<int>(std::cout, "\n"));
	 for (int i = 0; i < entity.size(); i++) {
		std::cout << YLW << entity[i] << std::endl;
	 }
	std::cout << DFT << std::endl;
}

void Request::parserHead() {
	
	// if (endmsg) {

	// }
}
