# 오늘 한것
<p alignt="center"> <img src="https://user-images.githubusercontent.com/85930183/230856253-efe93407-63a8-464c-9054-d343aa335ea3.png"  width="80%">
</p>

***
## request 클래스 구현

- 클라이언트가 보낸 요청을 server에서 read 하여 raw msg 변수에 담아서 출력함
브라우저에서 http://localhost:8080/?message=hi 를 입력하면서 테스트 함
- client 생성 시 `std::string -> Request *`로 수정 ( leak 체크 함 )

- <의의> HTTP request message가 `Server class`에서 `Request class`로 이동했다!

***
## 해야될 것?!

- request : `raw_msg` parsing

- issue 
    - Client 가 여러개의 서로다른 request msg 를 보냈을 때 어떻게 관리할 것인가?
        1. `map<int, Request *> clients` 부분에서 `Request *` 에 대해 논의 필요.   
	    2. `map<int, map<int, Request *>> client` (?)