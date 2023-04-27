# 에코 서버 객체화 하기

☝🏻 멤버 변수 default 는 private

<br>

***

## 1. ServerSocket Class - 서버 소켓 관리자
☝🏻 소켓 생성(socket) + 소켓과 IP 연결(address struct, bind, listen)

### 1-1. 멤버 변수
``` C++
#include <arpa/inet.h>
#include <sys/socket.h>

int                        server_socket
struct sockaddr_in         server_addr
```

### 1-2. 멤버 함수
``` C++
void    safeSocket(int domain, int type, int protocol);
void    safeListen(int backlog);
void    safeBind(void);
int     safeAccept(void);

void    setNonBlock(int socket_fd);
int     getServerSocket(void);
```

### 1-3. 생성자
``` C++
ServerSocket(sa_family_t address_family, int port);
```

<br>

*** 

## 2. Server Class - 서버 관리자

### 2-1. 멤버 변수
```C++
private:
    std::map<int, std::string>  clients
    int 					    kq
    std::vector<struct kevent>  change_list
    struct kevent 				event_list[8]

    ServerSocket                *server_socket;
```

### 2-2. 멤버 함수
```C++
public:
    int		kqueue(void);

    // 내부에서 EV_SET 및 change_list 에 kevent 를 push_back 함
    void	setChangeList(std::vector<struct kevent> &change_list, 
                            uintptr_t                ident,
                            int16_t                  filter,
                            uint16_t                 flags,
                            uint32_t                 fflags,
                            intptr_t                 data,
                            void                     *udata);

    void    run(void);
    void    disconnectClient(int client_fd, std::map<int, std::string> &clients);
    
    int safeKevent(int nevents, const timespec *timeout);
    int safeRecv(int fd, char *buf);
    int safeSend(int fd);

```

### 2-3. 생성자
``` C++
Server(ServerSocket &server_socket);

```

<br>

***

## 3. Request Class - 요청자

### 3-1. 멤버 변수

```c++
#define POST 0
#define GET 1
#define DELETE 2

private:
    std::string							raw_msg;

	bool                                 head_done;
	std::string                          method;
	std::string                          url;
	std::string                          version;
	std::map<std::string, std::string>   header;
	std::vector<std::string> 		     entity;
```


### 3-2. 멤버 함수

```c++
public:
    // setMethod();
    // setUrl();
    // setHeader();
    // setEntity();

	void        setRawMsg(const std::string &);
	std::string getRawMsg();
	void        clearSetRawMsg();
    void        parserHead();
    void        toString();
```

### 3-3. 생성자
```c++
Request();
```



<br>

***

## 4. Response Class - 응답자

### 4-1. 멤버 변수

```c++
#define POST 0
#define GET 1
#define DELETE 2

private:
    
```


### 4-2. 멤버 함수

```c++
public:
    
```

### 4-3. 생성자
```c++
Response();
```



<br>

***

