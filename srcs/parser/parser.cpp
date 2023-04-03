#include "../include/include.hpp"

void	parser(std::string config_file) {
		
	std::ifstream	file_stream(config_file);
	std::string		line;
	bool 			server_flag = false;

	if (!file_stream.is_open()) {
		errorHandler("No file!");
	}
	while (1) {
		std::getline(file_stream, line);
		//	만약 주석 추가하면 # 로 시작하는지 체크해서 건너뛰기
		//	white space 건너뛰기
		//	글자의 마지막이 '{' || ';' 로 끝나는 line 찾기 
		// { 로 끝나면 block 시작
		//	';' 로 끝나면 directive 시작
		//	둘 다 아니면 일단 error
		if (isComment(line) || isEmptyLine(line)) {
			continue;
		} else if (isBlock(line, &server_flag)) {
			//	서버를 파싱
		} else {
			errorHandler("Invaild configuration file!");
		}
		// line.erase(0, line.find_first_not_of(" \t\v\f\r"));
        // while (isspace(line[line.length() - 1]))
        //     line.erase(line.find_last_not_of(" \t\v\f\r") + 1, line[cmd.length() - 1]);
		// 글자 나올떄까지 패스;
		// if (file_content == server) 무적권 server가 먼저 나와야한다!
		// else 에러처리
	}


}

int	isServer(std::string line, bool *server_flag) {
	server_flag = true;
	//	if (첫번째 단어 == server) {
}
int	isBlock(std::string line, ifstream &file) {
	//	line 을 받아서 '{' 로 끝나는지 검사
	//	'{' 있는 줄까지 일고 다음줄
	// '}' 를 찾아야 함.
	//	isDirective() || isBlock
	//	
	
}
int	isDirective(std::string line){
	//	line 을 받아서 ';' 로 끝나는지 검사
}


std::string getBlock() {

}
std::string getDirective();


// void p2(std::string file_content)
// {
// 	if (server) block() 
// 	else directive();	
// 	// 한줄 파싱해서 구조체에 넣음
// }

// void directive() {

// }

// void block() {
// if ()
// }

// map<string(/), map<string, vector<string> >()>() location

// location / {
//     proxt_dir blab음a blabla2;
//     abcd efg;
//     .
//     .
//     .
// }

// location /image {
//     proxy_dir ssss;
//     eeeeee fffff;
//     .
//     .
//     .
// }