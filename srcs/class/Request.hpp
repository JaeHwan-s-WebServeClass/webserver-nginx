#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>

#define POST 0
#define GET 1
#define DELETE 2

/*
Request msg 형식
    <메서드> <요청 URL> <버전> 
	<헤더>
    
	<엔터티 본문>

ex)
    <메서드> <요청 URL> <버전>
    GET /test/hi-there.txt HTTP/1.1
    
    <헤더>
    Accept: text/
    Host: www.joes-hardware.com	

    <본문>
    없음.
*/

// webserv에서 구현해야하는 method: GET, HOST, DELETE

class Request {
private:
    // 여러가지 요청이 동시에 들어오고, 동시에 처리해야 하는 상황이 있을까?
    // 하나씩 처리한다면 raw_msg 를 clear 하면서 진행하면 되지만, 병렬처리라면 어떻게 해야하는가?
	std::string							raw_msg;

	// int                                  method;     // 파싱할 때 compare해서 define해둔 int 값 사용
	// std::string                          url;
	// std::string                          version;
	// std::map<std::string, std::string>   header;
	// std::vector<std::string> 		    entity;

public:
    Request();
    // setMethod();
    // setUrl();
    // setHeader();
    // setEntity();

    /// @brief set Raw Msg
    /// @param update
    /// @return 
	void setRawMsg(const std::string &);

    /// @brief get Raw Msg
    /// @param 
    /// @return rawmsg 
	std::string getRawMsg();

    /// @brief clear Raw Msg buf
    /// @param 
    /// @return 
	void clearSetRawMsg();
};

#endif
