# 목표

C++ 98 로 HTTP 웹서버 만들기

<br>
<br>

## 프로그램 실행 방법
./webserv [configuration file] 

<br>
<br>

## 요구 사항
### P5
* 프로그램은 configuration file 을 인자로 받거나 기본 경로를 사용해야 한다.
<br>

* 서버는 절대로 차단해서는 안되며, 필요에 따라 클라이언트가 다른 서버로 리다이렉션 될 수 있다.
  * ☝🏻 "서버가 클라이언트의 요청을 처리하기 어렵거나 오랜 시간이 걸리는 경우" 라고 해석함
<br>

* 클라이언트 - 서버 간의 모든 I/O 연산에 대해, 1개의 poll() 만을 사용해야 한다.
  * ❗️ 찾아보니, 이제는 poll() 함수를 안쓴다. select(), kqueue(), epoll() 을 사용하자.
  * ☝🏻 poll() 이 어떤건지 아직 잘 모르겠지만, 소캣을 연결하는 함수인 것 같다.
<br>

* poll() 은 읽기와 쓰기를 동시에 확인해야한다.
<br>

* poll() 을 거치지 않고 읽기 또는 쓰기 작업을 해서는 안된다.
<br>

* 읽기 또는 쓰기 작업 후 errno 값을 확인하는 것은 엄격히 금지되어 있다.
<br>

* configuration file 을 읽기 전에 poll() 을 사용하지 말자.
<br>

* ❗️ poll() 을 사용하지 않고 파일 디스크립터에서 읽기/받기 또는 쓰기/전송을 시도하면 안된다.
<br>

* FD_SET, FD_CLR, FD_ISSET, FD_ZERO 매크로를 사용할 수 있다.
<br>

* 서버에 대한 요청이 중단되는 안된다.
<br>

* 서버는 선택한 웹 브라우저와 호환되어야 한다.
<br>

* Nginx 는 HTTP 1.1 을 준수하며, 프로젝트를 진행하며 헤더와 응답 동작을 비교할 때 활용할 수 있다.
<br>

* HTTP 응답 상태 코드가 정확해야 한다.
  * 200, 300 등 상태 코드를 의미하ㄴ다.
<br>

* 기본 오류 페이지가 제공되어야 한다.
<br>

* 클라이언트는 파일을 업로드 할 수 있어야 한다.
<br>

* 최소한 GET, POST, DELETE 메소드가 필요하다.
<br>

* 서버에 대한 부하(stress) 테스트를 해야하며, 서버는 항상 사용 가능한 상태를 유지해야한다.
<br>

* 서버는 여러 포트를 수신할 수 있어야 한다.

### P6
* MacOS 는 다른 유닉스 OS 와 같은 방식으로 write() 를 구현하지 않는다. 대신 fcntl() 을 사용할 수 있다.
  ❗️ 다만 nonblock 일 때만 사용가능하다. ex. `fcntl(fd, F_SETFL, O_NONBLOCK);`

#### Configuration file
⭐️ nginx conf 파일의 'server' 부분에서 영감을 얻을 수 있다.
<br>

* 각 "server" 의 port 와 host 를 선택한다.
<br>

* server_names 을 설정할지 여부를 결정한다.
<br>

* host:port 의 첫 번째 서버가 이 host:port 의 기본값이 된다. (즉, 다른 서버에 속하지 않은 모든 요청에 응답한다는 의미).
  <p align='center'> <img src="https://user-images.githubusercontent.com/85930183/231663371-cbff03bb-184b-4c50-836a-c012408f6dfc.png" width='90%'></p>
<br>

* 기본 오류 페이지를 설정한다.
<br>

* 클라이언트 본문 크기를 제한한다.
<br>

* 다음 규칙/구성 중 하나 또는 여러 개를 사용하여 경로를 설정한다.
  * 경로에 허용되는 HTTP 메서드 목록을 정의
  * HTTP 리디렉션을 정의
  * 파일을 검색할 디렉토리 또는 파일을 정의한다.
    * 예 <br>
      url /kapouet이 /tmp/www에 루팅된 경우 <br>
      url /kapouet/pouic/toto/pouet -> /tmp/www/pouic/toto/pouet
<br>

  * 디렉토리 목록을 on / off 할 수 있다.
    * autoindex 설정을 이야기한다.
  <br>
  
  * 요청이 디렉토리인 경우 응답할 기본 파일을 설정한다.
  <br>

  * 특정 파일 확장자(ex: .php)를 기준으로 CGI를 실행한다.
  <br>

  * POST 및 GET 메소드와 함께 작동하도록 설정한다.
  <br>

  * 업로드된 파일을 허용하고 저장할 위치를 설정할 수 있는 경로를 만든다.
    * CGI 를 직접 호출하지 않으므로 전체 경로를 PATH_INFO 로 사용한다.
    * 청크된 요청의 경우 서버가 청크를 언청크해야 하며, CGI 는 본문의 끝을 EOF 로 파악한다.
    * CGI 출력도 마찬가지다. CGI 에서 content_length 가 없으면, EOF 가 데이터의 끝이다.
    * 프로그램은 요청된 파일을 첫 번째 인자로 사용하여 CGI 를 호출해야 한다.
      * ☝🏻 Configuration 파일에 적힌 CGI 를 불러온다는 것으로 이해함.
    * CGI 는 상대 경로 파일 액세스를 위해 올바른 디렉토리에서 실행되어야 한다.
    * 서버는 하나의 CGI (php-CGI, Python 등)으로 작동해야 한다.
  <br>

### P8
추가 기능을 구현할 계획이 있다면,
* 쿠키 및 세션 관리 지원하기
* 여러 개의 CGI를 처리할 수 있도록 하기

