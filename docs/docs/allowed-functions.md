# 사용할 함수에 대한 질문 ( feat. chatGPT )
## 🅀 gai_strerror 가 뭐지?

### 🄰
gai_strerror() 함수는 네트워크 함수 중에 getaddrinfo()나 getnameinfo()와 함께 사용되며, 이 함수들이 반환하는 오류 코드를 문자열로 변환하는 역할을 합니다.<br>
즉, gai_strerror() 함수는 인자로 받은 error 코드에 해당하는 오류 메시지를 반환합니다. 이 함수는 <netdb.h> 헤더 파일에 선언되어 있습니다.<br>
예를 들어, getaddrinfo() 함수가 실패하면 해당 함수는 -1을 반환하고, 오류 정보는 gai_strerror() 함수를 호출하여 확인할 수 있습니다.

<br>

***

## 🅀 htons 가 뭐야?

### 🄰
htons() 함수는 "Host to Network Short"의 약어로, 16비트 short 형식의 데이터를 호스트 바이트 오더로 표현된 데이터에서 네트워크 바이트 오더로 변환하는 함수입니다.

네트워크 바이트 오더는 빅 엔디안으로 정의되어 있기 때문에, htons() 함수는 호스트에서 사용하는 리틀 엔디안 바이트 오더를 네트워크에서 사용하는 빅 엔디안 바이트 오더로 변환합니다. 이 함수는 <arpa/inet.h> 헤더 파일에 선언되어 있습니다.

예를 들어, short 형식의 변수인 port가 리틀 엔디안 바이트 오더로 표현되어 있다면, htons() 함수를 사용하여 네트워크 바이트 오더로 변환할 수 있습니다.

```c
#include <arpa/inet.h>

short port = 8080;
unsigned short net_port = htons(port);
```
위 코드에서 net_port는 16비트 short 형식의 port 변수를 네트워크 바이트 오더로 변환한 결과를 저장하고 있습니다. 이렇게 변환된 데이터는 네트워크 상에서 전송되는 데이터 형식과 호환되기 때문에, 네트워크 프로그래밍에서 매우 중요한 역할을 합니다.

네트워크 프로그래밍에서 htons() 함수를 사용하는 간단한 예제를 보여드리겠습니다. 이 예제는 클라이언트-서버 모델을 이용하여 클라이언트가 서버로 메시지를 전송하는 예제입니다.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char message[256];

    // 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // 메시지 전송
    printf("Enter a message: ");
    fgets(message, sizeof(message), stdin);
    write(sock, message, strlen(message));

    // 소켓 종료
    close(sock);
    return 0;
}
```
위 코드에서 htons() 함수는 server_addr 구조체의 sin_port 멤버에 사용되었습니다. 클라이언트는 SERVER_PORT 상수 값을 리틀 엔디안으로 표현하고 있지만, htons() 함수를 사용하여 이를 빅 엔디안으로 변환하여 server_addr 구조체의 sin_port 멤버에 할당하였습니다. 이렇게 빅 엔디안으로 표현된 sin_port 값은 connect() 함수에서 서버와의 연결 시 사용됩니다.


<br>

***

## 🅀 select, poll, epoll, kqueue 의 특징과 차이점이 뭐여?

### 🄰
select, poll, epoll, kqueue은 모두 I/O 멀티플렉싱 기법을 구현하는 시스템 콜입니다. 이러한 시스템 콜은 하나의 프로세스가 여러 개의 파일 디스크립터를 모니터링하고, 여러 개의 파일 디스크립터 중에서 I/O 이벤트가 발생한 것을 감지하여 이를 처리할 수 있도록 도와줍니다.

하지만 select, poll, epoll, kqueue 각각의 특징과 차이점은 다음과 같습니다.

**↘︎ select** <br>
select는 가장 오래된 I/O 멀티플렉싱 기법으로, 파일 디스크립터의 상태 변화를 감시하기 위해 fd_set이라는 비트맵 자료구조를 사용합니다. select는 상대적으로 간단하고, 대부분의 운영 체제에서 지원됩니다. 그러나 비트맵의 크기가 제한되어 있기 때문에, 모니터링할 수 있는 파일 디스크립터의 수에도 제한이 있습니다. 또한, select는 모든 파일 디스크립터를 순차적으로 탐색하므로, 대용량의 파일 디스크립터 집합에서는 성능 문제가 발생할 수 있습니다.

**↘︎ poll** <br>
poll은 select와 마찬가지로 비트맵을 사용하여 파일 디스크립터의 상태 변화를 감시합니다. 그러나 select와 달리 파일 디스크립터 수의 제한이 없으며, 성능도 select보다 우수합니다. 하지만 여전히 파일 디스크립터를 순차적으로 탐색하므로, 대용량의 파일 디스크립터 집합에서도 성능 문제가 발생할 수 있습니다.

**↘︎ epoll** <br>
epoll은 리눅스에서 사용되는 I/O 멀티플렉싱 기법으로, 이벤트 기반으로 동작합니다. epoll은 select와 poll의 성능 문제를 개선하기 위해 고안되었습니다. epoll은 이벤트가 발생한 파일 디스크립터만 반환하기 때문에, 대용량의 파일 디스크립터 집합에서도 빠르고 효율적으로 이벤트를 처리할 수 있습니다. 또한, epoll은 수천, 수십만 개 이상의 파일 디스크립터를 모니터링할 수 있습니다.

**↘︎ kqueue** <br>
kqueue은 FreeBSD와 macOS에서 사용되는 I/O 멀티플렉싱 실행 방식은 epoll과 유사하지만, kqueue은 epoll보다 더 많은 이벤트 유형을 지원하고, 이벤트 발생 시 데이터를 반환할 수 있습니다. 또한, kqueue은 파일 디스크립터와 다른 리소스(예: 프로세스, 타이머 등)를 동시에 관리할 수 있으므로, 다양한 용도로 활용될 수 있습니다. kqueue은 epoll보다 상대적으로 느릴 수 있지만, 매우 높은 처리량을 갖는 어플리케이션에서는 더 높은 성능을 발휘할 수 있습니다.

이러한 select, poll, epoll, kqueue의 특징과 차이점을 고려하여, 개발하려는 어플리케이션의 요구사항과 운영 체제 환경에 따라 적절한 I/O 멀티플렉싱 기법을 선택할 수 있습니다.

<br>

***

## 🅀 getaddrinfo, freeaddrinfo에 대해서 설명해주렴

### 🄰
getaddrinfo는 호스트 이름과 서비스 이름(혹은 포트 번호)을 입력 받아 해당 호스트와 서비스에 대한 주소 정보를 가져오는 함수입니다. 이 함수는 IPv4와 IPv6를 모두 지원하며, 여러 개의 주소 정보를 반환할 수 있습니다. 이러한 다양한 주소 정보는 struct addrinfo 구조체의 형태로 반환됩니다.

getaddrinfo 함수를 사용하기 전에는 반드시 해당 구조체를 초기화해야 합니다. 또한, getaddrinfo 함수를 사용한 후에는 반환된 addrinfo 구조체를 메모리에서 해제해주어야 하는데, 이를 위해 freeaddrinfo 함수를 사용합니다.

freeaddrinfo 함수는 getaddrinfo 함수가 반환한 addrinfo 구조체의 메모리를 해제하는 역할을 합니다. 이 함수는 getaddrinfo 함수가 반환한 addrinfo 구조체를 인자로 받아 해당 구조체의 메모리를 해제하며, 이때 구조체 내부에 있는 ai_next 포인터도 재귀적으로 해제합니다. 이를 통해 메모리 누수를 방지할 수 있습니다.

예를 들어, 아래는 getaddrinfo와 freeaddrinfo를 사용하여 구글의 DNS 서버 주소 정보를 가져오고 메모리를 해제하는 예시 코드입니다.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main()
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int status;

    /* 초기화 */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; /* IPv4 또는 IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* UDP 소켓 */
    hints.ai_flags = AI_PASSIVE; /* 호스트 이름이 없는 경우 */

    /* 구글의 DNS 서버 주소 정보 가져오기 */
    status = getaddrinfo("8.8.8.8", "53", &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    /* 가져온 주소 정보 출력 */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        char host[NI_MAXHOST], service[NI_MAXSERV];
        status = getnameinfo(rp->ai_addr, rp->ai_addrlen,
                             host, NI_MAXHOST,
                             service, NI_MAXSERV,
                             NI_NUMERICHOST | NI_NUMERICSERV);
        if (status != 0) {
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(status));
            exit(EXIT_FAILURE);
        }
        printf("IP address: %s, Port number: %s\n", host, service);
    }

    /* addrinfo 구조체의 메모리 해제 */
    freeaddrinfo(result);

    return 0;
}
```

getaddrinfo 함수는 다음과 같은 네 개의 인자를 받습니다.

* node: 호스트 이름 또는 IP 주소를 지정하는 문자열입니다. 이 인자는 필수적이며, NULL이나 빈 문자열("")을 전달할 수 있습니다. 만약 NULL이나 빈 문자열("")을 전달한 경우에는 local IP 주소가 반환됩니다.

* service: 서비스 이름 또는 포트 번호를 지정하는 문자열입니다. 이 인자는 NULL이나 빈 문자열("")을 전달할 수 있습니다. 만약 NULL이나 빈 문자열("")을 전달한 경우에는 0이 반환됩니다.

* hints: struct addrinfo 구조체의 포인터입니다. 이 구조체를 사용하여 반환받고자 하는 주소 정보의 특성을 지정할 수 있습니다. 이 구조체를 NULL로 전달하는 경우, getaddrinfo 함수는 기본값을 사용합니다.

* res: struct addrinfo 구조체의 포인터입니다. 이 포인터가 가리키는 구조체에는 getaddrinfo 함수가 반환한 주소 정보가 저장됩니다. 이 포인터가 NULL인 경우, getaddrinfo 함수는 반환된 주소 정보를 무시합니다.

hints 구조체의 멤버 변수로는 ai_family, ai_socktype, ai_protocol, ai_flags 등이 있으며, 이들을 통해 반환받고자 하는 주소 정보의 특성을 지정할 수 있습니다. 자세한 내용은 man getaddrinfo를 참고하시기 바랍니다.

<br>

***

## 🅀 setsockopt, getsockname, getprotobyname 함수에 대해 설명해주련?

### 🄰 각 함수에 대해 간단한 설명을 드리겠습니다.

**↘︎ setsockopt** <br>
setsockopt 함수는 소켓의 옵션 값을 설정합니다. 이 함수를 사용하여 소켓의 송수신 버퍼 크기, 브로드캐스트 가능 여부, 소켓의 타입 등을 설정할 수 있습니다. 함수의 프로토타입은 다음과 같습니다.

```c
#include <sys/types.h>
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen);
```

* sockfd: 소켓 파일 디스크립터입니다. <br>
* level: 옵션의 프로토콜 수준입니다. 일반적으로 SOL_SOCKET을 사용합니다. <br>
* optname: 설정하려는 옵션의 이름입니다. 예를 들어 소켓의 송수신 버퍼 크기를 설정하려면 SO_SNDBUF 또는 SO_RCVBUF를 사용합니다. <br>
* optval: 설정하려는 옵션 값입니다. <br>
* optlen: optval의 크기입니다. <br>

**↘︎ getsockname** <br>
getsockname 함수는 소켓의 로컬 주소 정보를 얻습니다. 함수의 프로토타입은 다음과 같습니다.


```c
#include <sys/types.h>
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr,
                socklen_t *addrlen);
```

* sockfd: 소켓 파일 디스크립터입니다.
* addr: 소켓의 로컬 주소 정보를 저장할 sockaddr 구조체의 포인터입니다.
* addrlen: addr의 크기입니다. 함수를 호출하기 전에 이 값은 addr의 크기로 초기화되어야 합니다.


**↘︎ getprotobyname** <br>
getprotobyname 함수는 프로토콜 이름에 해당하는 프로토콜 번호를 얻습니다. 함수의 프로토타입은 다음과 같습니다.

```c
#include <netdb.h>

struct protoent *getprotobyname(const char *name);
```
name: 프로토콜 이름입니다.
getprotobyname 함수는 struct protoent 구조체의 포인터를 반환합니다. 이 구조체에는 프로토콜 이름, 프로토콜 번호, 프로토콜의 멀티캐스트 주소 등의 정보가 저장됩니다.


<br>

***
## 🅀 getprotobyname의 예시를 한 번 보여줘. 프로토콜 번호는 어떻게 얻니?

### 🄰 getprotobyname 함수는 프로토콜 이름을 이용하여 struct protoent 구조체를 얻습니다. 이 구조체에는 프로토콜 이름, 프로토콜 번호, 프로토콜의 멀티캐스트 주소 등의 정보가 저장됩니다. 따라서 getprotobyname 함수를 사용하면 프로토콜 이름을 이용하여 프로토콜 번호를 얻을 수 있습니다.

다음은 getprotobyname 함수를 사용하여 TCP 프로토콜의 번호를 얻는 예시입니다.

```c
#include <netdb.h>
#include <stdio.h>

int main() {
    struct protoent *proto = getprotobyname("tcp");

    if (proto == NULL) {
        perror("getprotobyname");
        return 1;
    }

    printf("Protocol name: %s\n", proto->p_name);
    printf("Protocol number: %d\n", proto->p_proto);
    printf("Aliases:\n");
    for (char **alias = proto->p_aliases; *alias != NULL; alias++) {
        printf("- %s\n", *alias);
    }

    return 0;
}
```

위 예시는 getprotobyname 함수를 사용하여 tcp 프로토콜의 정보를 얻고 출력하는 예시입니다. 실행 결과는 다음과 같습니다.

```yaml
Protocol name: tcp
Protocol number: 6
Aliases:
- tcp6
```

struct protoent 구조체에 저장된 정보를 이용하여 프로토콜 번호를 얻을 수 있습니다. 위 예시에서는 proto->p_proto를 이용하여 tcp 프로토콜의 번호인 6을 출력하고 있습니다.

<br>

***

## 🅀 위 예시에서 alias가 뭔지 알려줘보렴

### 🄰
struct protoent 구조체에는 프로토콜에 대한 다양한 정보들이 포함됩니다. p_aliases 멤버는 해당 프로토콜에 대한 별명(alias)들의 배열을 가리키는 포인터입니다. 즉, 프로토콜에 대해 여러 별명이 존재할 경우 이 배열에 저장됩니다.

예를 들어, getprotobyname("tcp") 함수를 호출하여 TCP 프로토콜의 정보를 얻은 경우 proto->p_aliases 배열에는 "tcp6"과 같은 별명이 저장될 수 있습니다. 위 예시에서는 이러한 별명(alias)들을 반복문을 통해 출력하고 있습니다.

<br>

***

## 🅀 fcntl 에 대해 설명해줄 수 있겠니?

### 🄰
fcntl 함수는 파일 디스크립터에 대한 제어를 수행하는 함수입니다. 파일 디스크립터에 대한 여러가지 동작을 수행할 수 있으며, 주로 다음과 같은 목적으로 사용됩니다.

* 파일 디스크립터의 플래그(상태)를 변경하기 위해 사용됩니다. 예를 들어, non-blocking 모드로 파일 디스크립터를 설정할 수 있습니다. <br>
* 파일 디스크립터의 속성을 변경하기 위해 사용됩니다. 예를 들어, 파일 디스크립터의 close-on-exec 플래그를 설정할 수 있습니다. <br>
* 파일 디스크립터를 동기화하기 위해 사용됩니다. 예를 들어, 파일 디스크립터의 락(잠금)을 설정할 수 있습니다. <br>
<br>
fcntl 함수의 기본적인 사용법은 다음과 같습니다.

```c
#include <fcntl.h>

int fcntl(int fd, int cmd, ... /* arg */);
```

* fd: 파일 디스크립터입니다. <br>
* cmd: 수행할 동작을 지정하는 제어 명령입니다. <br>
* arg: 제어 명령에 따라 사용될 인자입니다. <br>
* fcntl 함수는 제어 명령에 따라 arg 인자가 필요하지 않을 수도 있습니다. 따라서 arg 인자는 가변 인자입니다. <br>

예를 들어, 다음은 파일 디스크립터를 non-blocking 모드로 설정하는 예시입니다.

```c
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

int set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL)");
        return -1;
    }

    return 0;
}
```

위 예시에서 fcntl(fd, F_GETFL, 0)를 통해 파일 디스크립터 fd의 플래그를 가져오고, 이를 F_SETFL 제어 명령과 O_NONBLOCK 플래그를 이용하여 non-blocking 모드로 설정하고 있습니다.

<br>

***

## 🅀 fcntl(fd, F_SETFL, O_NONBLOCK) 부분의 의미를 좀 더 자세하게 설명해주렴

### 🄰 fcntl(fd, F_SETFL, O_NONBLOCK)는 파일 디스크립터 fd를 non-blocking 모드로 설정하는 코드입니다.

일반적으로, 파일 디스크립터는 blocking 모드로 동작합니다. 이 말은, 파일 디스크립터를 읽거나 쓸 때, 해당 작업이 완료될 때까지 대기하게 된다는 뜻입니다. 이러한 대기 시간은 응용 프로그램에서 처리할 다른 작업들을 지연시키는 원인이 될 수 있습니다.

따라서, non-blocking 모드로 파일 디스크립터를 설정하면, 해당 파일 디스크립터를 읽거나 쓸 때, 대기하지 않고 즉시 반환하게 됩니다. 이러한 동작은 대개 select, poll, epoll과 같은 비동기 입출력 함수와 함께 사용됩니다.

위의 코드에서 fcntl 함수는 fd에 대한 제어 명령으로 F_SETFL을 지정하고, O_NONBLOCK 플래그를 추가하여 non-blocking 모드로 설정하도록 합니다. F_SETFL은 파일 디스크립터의 플래그를 설정하기 위한 제어 명령 중 하나이며, O_NONBLOCK은 non-blocking 모드를 나타내는 상수입니다.

따라서 fcntl(fd, F_SETFL, O_NONBLOCK)는 파일 디스크립터 fd를 non-blocking 모드로 설정하는 코드입니다.

<br>

***

## 🅀 fcntl(fd, F_SETFL, O_NONBLOCK)은 언제 -1을 반환하니

### 🄰 fcntl(fd, F_SETFL, O_NONBLOCK) 함수가 -1을 반환하는 경우는 일반적으로 다음과 같은 경우입니다.

* fd가 유효하지 않은 파일 디스크립터인 경우 <br>
* * F_SETFL 명령이 지원되지 않는 경우 <br>
* O_NONBLOCK 플래그가 지원되지 않는 경우 <br>
* 다른 프로세스가 이미 fd를 non-blocking 모드로 설정한 경우 <br>

또한, fcntl() 함수가 실패하면 errno 전역 변수에 실패 원인이 저장됩니다. 따라서, fcntl(fd, F_SETFL, O_NONBLOCK) 함수가 -1을 반환할 경우, errno 값을 확인하여 실패 원인을 파악할 수 있습니다.

<br>

***

## 🅀 fcntl 으로 여러 파일 디스크립터를 non-blocking 모드로 설정하고 실행하는 예시를 보여줄 수 있겠니?

### 🄰
네, 당연히요! 아래 예시 코드는 파일 디스크립터 0, 1, 2를 non-blocking 모드로 설정한 후, select() 함수를 사용하여 각 파일 디스크립터에서 입력을 받아들이는 예시입니다.

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

int main()
{
    // 파일 디스크립터 0, 1, 2를 non-blocking 모드로 설정
    int fd_flags;
    for (int i = 0; i < 3; i++) {
        fd_flags = fcntl(i, F_GETFL, 0);
        if (fd_flags == -1) {
            perror("fcntl");
            exit(1);
        }
        if (fcntl(i, F_SETFL, fd_flags | O_NONBLOCK) == -1) {
            perror("fcntl");
            exit(1);
        }
    }

    fd_set readfds;
    char buf[1024];

    while (1) {
        // select() 함수를 사용하여 입력을 받아들임
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(1, &readfds);
        FD_SET(2, &readfds);
        if (select(3, &readfds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }

        // 입력이 들어온 파일 디스크립터에서 데이터를 읽음
        for (int i = 0; i < 3; i++) {
            if (FD_ISSET(i, &readfds)) {
                ssize_t nread = read(i, buf, sizeof(buf));
                if (nread == -1) {
                    perror("read");
                    exit(1);
                }
                printf("fd=%d, read %zd bytes: %.*s\n", i, nread, (int)nread, buf);
            }
        }
    }

    return 0;
}
```

위 코드에서는 먼저 파일 디스크립터 0, 1, 2를 non-blocking 모드로 설정하고 있습니다. 그 다음에는 select() 함수를 사용하여 파일 디스크립터에서 입력을 받아들이고, 입력이 들어온 파일 디스크립터에서 데이터를 읽어 출력합니다.

참고로, 위 코드에서는 파일 디스크립터 0, 1, 2를 대상으로 non-blocking 모드를 설정하고 있지만, 실제로는 이 외에도 다른 파일 디스크립터에 대해서도 non-blocking 모드를 설정할 수 있습니다.

<br>

***

## 🅀 select(3, &readfds, NULL, NULL, NULL) 의 인자에 대해 설명해주렴^^

### 🄰 select() 함수는 I/O 멀티플렉싱을 수행하기 위한 시스템 콜 중 하나로, 여러 개의 파일 디스크립터에서 데이터를 읽고 쓰는 작업을 한 번에 처리할 수 있게 해줍니다. select() 함수는 다음과 같은 형태를 가지고 있습니다.

```c
#include <sys/select.h>

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

* nfds: 검사할 파일 디스크립터 수(+1 된 최대값) <br>
* readfds: 읽기 가능한 상태를 검사할 파일 디스크립터 집합 <br>
* writefds: 쓰기 가능한 상태를 검사할 파일 디스크립터 집합 <br>
* exceptfds: 예외가 발생한 상태를 검사할 파일 디스크립터 집합 <br>
* timeout: select() 함수가 블로킹되는 시간을 지정하는 값. NULL이면 무한정 블로킹된다. <br>

위의 함수 호출에서는 readfds만 지정하고 나머지 인자들은 NULL을 전달하였습니다. 이는 readfds 집합에 등록된 파일 디스크립터 중에서 읽을 수 있는 것이 있는지를 검사하고자 하기 때문입니다. 이와 같이 검사할 I/O 상태를 설정하는 부분은 다음과 같은 방법으로 이루어집니다.

```c
fd_set readfds;

FD_ZERO(&readfds);
FD_SET(fd1, &readfds);
FD_SET(fd2, &readfds);
```

먼저 fd_set 타입의 변수 readfds를 선언하고, FD_ZERO() 함수를 사용하여 readfds 집합의 모든 비트를 0으로 초기화합니다. 그리고 FD_SET() 함수를 사용하여 fd1과 fd2라는 두 개의 파일 디스크립터를 readfds 집합에 등록합니다.

select() 함수는 지정된 파일 디스크립터 중 하나 이상의 I/O 상태 변화가 발생할 때까지 블로킹되며, I/O 상태 변화가 발생하면 해당 상태를 검사한 후 readfds에 등록된 파일 디스크립터 중에서 읽을 수 있는 것이 있는지를 검사하여 읽기 가능한 상태를 확인합니다. 따라서, select() 함수 호출 이후에는 readfds에 설정된 파일 디스크립터 중에서 읽기 가능한 상태인 것이 있으면 그 파일 디스크립터를 이용하여 데이터를 읽을 수 있게 됩니다.

<br>

***

## 🅀 위 예시에서 등장하는 FD_ZERO(&readfds), FD_SET(0, &readfds), FD_ISSET(i, &readfds)들은 함수니?

### 🄰 네, 맞습니다.

* FD_ZERO(&readfds): readfds 집합을 초기화하는 함수입니다. readfds 집합 내의 모든 파일 디스크립터를 제거하고 초기 상태로 만듭니다. <br>
* FD_SET(0, &readfds): 파일 디스크립터 0을 readfds 집합에 추가하는 함수입니다. 이 경우 파일 디스크립터 0은 표준 입력을 나타냅니다. <br>
* FD_ISSET(i, &readfds): 파일 디스크립터 i가 readfds 집합에 포함되어 있는지 여부를 확인하는 함수입니다. 반환값은 0 또는 1입니다. <br>

<br>

***
## 🅀 fd가 읽을 수 있는 상태인지 검증하는 코드의 예시를 보여주렴^^

### 🄰
fd가 읽을 수 있는 상태인지 검증하는 코드는 다음과 같이 작성할 수 있습니다.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

int main(void) {
    fd_set rfds;
    struct timeval tv;
    int retval;

    // 파일 디스크립터 0을 모니터링하기 위해 rfds 집합 초기화
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    // 타이머 설정
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    // 파일 디스크립터 0이 읽을 수 있는 상태가 될 때까지 대기
    retval = select(1, &rfds, NULL, NULL, &tv);
    if (retval == -1) {
        perror("select()");
        exit(EXIT_FAILURE);
    } else if (retval) {
        printf("Data is available now.\n");
    } else {
        printf("No data within five seconds.\n");
    }

    return 0;
}
```

위 코드에서 FD_ZERO 함수를 사용하여 rfds 집합을 초기화합니다. 그리고 FD_SET 함수를 사용하여 파일 디스크립터 0 (표준 입력)을 rfds 집합에 추가합니다. 이제 select 함수를 호출하여 rfds 집합에 포함된 파일 디스크립터 중에서 읽을 수 있는 상태가 되기를 기다립니다. select 함수의 마지막 인자 &tv는 select 함수가 최대 몇 초 동안 대기해야 하는지를 지정합니다. select 함수는 rfds 집합에 있는 파일 디스크립터 중에서 읽을 수 있는 상태가 되면 1을 반환합니다. 만약 타임아웃이 발생하면 0을 반환합니다. 그리고 에러가 발생하면 -1을 반환하고 perror 함수를 호출하여 에러 메시지를 출력합니다.




