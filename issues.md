# 에코 서버 객체화 하기

☝🏻 멤버 변수 default 는 private

<br>

***

## 1. 서버 소켓 관리자
☝🏻 소켓 생성(socket) + 소켓과 IP 연결(address struct, bind, listen)

### 1-1. 멤버 변수
``` C++
int                        server_socket
struct sockaddr_in         server_addr
```

### 1-2. 멤버 함수
``` C++
int     socket(int, int, int);
int     listen(int, int) __DARWIN_ALIAS(listen);
int     bind(int, const struct sockaddr *, socklen_t);
int     accept(int, struct sockaddr *, socklen_t *);
```

<br>

*** 

## 2. 서버 관리자

### 2-1. 멤버 변수
```C++
int                         c_fd
std::map<int, std::string>  clients
```

### 2-2. 멤버 함수

<br>

***

## 3. 이벤트 관리자 (kqueue)
* 서버를 상속 받은 이벤트 객체...?

<br>

***

