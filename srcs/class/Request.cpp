#include "Request.hpp"

Request::Request() : raw_head(""), is_end_head(0) {}

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
	//std::cout << BLU << this->raw_head << std::endl << DFT;
	
	//std::copy(entity.begin(), entity.end(), std::ostream_iterator<int>(std::cout, "\n"));
	
	std::cout << GRY << "-------------------- start-line --------------------" << DFT << std::endl;
	std::cout << BLU << "method: " << DFT << this->method << std::endl;
	std::cout << BLU << "url: " << DFT << this->url << std::endl;
	std::cout << BLU << "version: " << DFT << this->version << std::endl;
	std::cout << GRY << "---------------------- header ----------------------" << DFT << std::endl;
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); ++it) {
		std::cout << BLU << it->first << ": " << DFT << it->second << std::endl;
	}
	std::cout << GRY << "----------------------------------------------------" << DFT << std::endl;
	//  for (int i = 0; i < entity.size(); i++) {
	// 	std::cout << YLW << entity[i] << std::endl;
	//  }
	// std::cout << DFT << std::endl;
}

void Request::parserHead() {
	std::vector<std::string>	tmp_head;
	std::vector<std::string>	tmp_fl;

	// 개행기준으로 split하고
	tmp_head = ft::split(this->raw_head, '\n');

	// 시작줄은 space 기준으로 split
	tmp_fl = ft::split(tmp_head[0], ' ');
	this->method = tmp_fl[0];
	this->url = tmp_fl[1];
	this->version = tmp_fl[2];

	// header를 처리하는 부분
	// colon(:)을 찾아서 그 전까지는 key에 넣고 뒤의 부분은 value에 넣도록 합시다
	for (std::vector<std::string>::iterator it = tmp_head.begin() + 1; \
		 it != tmp_head.end(); ++it) {
		int pos = it->find(':');
		header[it->substr(0, pos)] = it->substr(pos + 1);
	}
}
// string substr (size_t pos = 0, size_t len = npos) const;
