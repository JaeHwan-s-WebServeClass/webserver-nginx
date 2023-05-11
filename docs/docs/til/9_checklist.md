# webserv

***

- [평가표 번역](https://github.com/FareWellWebServer/webserv/wiki/%ED%8F%89%EA%B0%80%ED%91%9C-%EB%B2%88%EC%97%AD)
- [42 wiki](https://yeosong1.github.io/webserv_eval)
- [webserv tutorial](https://42seoul.gitbook.io/webserv/)
- [팔만코딩경](https://80000coding.oopy.io/2944b11b-6729-4cd8-b545-c5dcdcb0c386)
- [siege test 관련](https://trend21c.tistory.com/1438)

***

## mandatory part

### basic explanation

- [ ] HTTP server
- [ ] functions used for I/O multiplexing
    - [ ] kqueue and `kevent()`

***

### basic checklist

- [ ] kqueue and `kevent()`
    - [ ] 하나의 `kqueue()`만이 있어야한다.
    - [ ] fd에 대한 READ/WRITE가 `kqueue()`를 거치지 않고 수행하지 말아야한다.
    - [ ] 하나의 클라이언트에 대해 `kqueue()`당 하나의 `read()`와 하나의 `write()`가 존재해야한다.
- [ ] read/recv/write/send
    - [ ] 에러가 발생할 경우 client가 제거되어야 한다.
    - [ ] the returned value is well checked. (checking only -1 or 0 is not good you should check both)
    - [ ] errono에 대한 검사가 위 함수 이후에 진행되지 않아야 한다.

***

### configuration

- [ ] HTTP response status code가 잘못되어서는 안된다.
- [ ] multiple servers with:
    - [ ] different port
    - [ ] different hostname
        - [ ] ex) `: curl --resolve example.com:80:127.0.0.1
http://example.com/`
- [ ] default error page <- [ ] 404 error page를 변경해 보도록 한다.
- [ ] client body size
    - [ ] ex) ` curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something shorteror longer than body limit"`
- [ ] directory and file
    - [ ] 서로 다른 directory(route -> location?) 설정해본다.
    - [ ] directory 요청 시의 default file이 있어야한다.
    - [ ] 특정 location에 대한 method를 설정해보고, method를 실행해본다.

***

### basic features

- [ ] method (이때 status code도 함께 체크해보도록 한다.)
    - [ ] `GET`: 반드시 작동해야 한다.
    - [ ] `POST`: 반드시 작동해야 한다.
    - [ ] `DELETE`: 반드시 작동해야 한다.
    - [ ] `UNKNOWN`: crash가 일어나지 않아야한다.
    - [ ] upload some file to the server and get it back 
- [ ] siege test (availibility higher than 99.5%)

***

### check with browser

- [ ] 특정 브라우저에서 서버로의 접속이 가능해야한다.
- [ ] request header와 response header를 확인해본다.
- [ ] 정적 웹사이트(fully static website)와 호환이 되어야한다.
- [ ] directory list -> auto-index?
- [ ] redirected URL
- [ ] 그 외의 여러가지를 시도해볼 수 있다.

***

### port issues

- [ ] multiple ports에 대해 제대로 동작해야한다.
- [ ] 같은 포트를 사용하려고 시도하더라도 동작하지 않아야한다. 
    - [ ]  In the configuration try to setup the same port multiple times. It should not work. 
- [ ] 서로 다른 conf를 가지지만 같은 port를 사용하는 여러 서버들을 실행했을 때, 서버가 만약 작동한다면 그 이유에 대해서도 설명할 수 있어야한다.

***

### siege/stress test

- [ ] siege
    - [ ] `siege -b`:빈페이지를 얻는 test에서 99.5% 이상의 availibility를 보여야한다.
    - [ ] 서버를 재시작하지 않고 siege를 계속 사용할 수 있어야한다.
- [ ] memory leaks가 있어서는 안된다.
- [ ] hanging connection이 없어야한다...? 

***

## bonus part

- noooooooo
