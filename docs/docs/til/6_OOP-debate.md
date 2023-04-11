# 객체 지향 프로그래밍을 시도해보기 위한 논의

## 1.

### 1-1. 논의 사항

ServerSocket 클래스(server-socket 생성 및 ip 연결하는 클래스)를 구현한 후, 나머지 부분 클래스를 만들었다. 나머지 부분은 Socket 클래스를 말한다.

Socket 클래스에서 객체에 속성과 기능을 추가하는 단계에 대한 2가지 논의가 있었다.

첫 번째.

1. kqueue 함수로 kq 파일디스크립터 생성
2. kq 에 서버 소켓 연결하여 클라이언트에서 보낸 이벤트 감지해서 kq 집합에 추가

두 번째.

1. kqueue 함수로 kq 파일디스크립터 생성
2. kq 에 서버 소켓을 연결
3. 클라이언트에서 보낸 이벤트 감지해서 kq 집합에 추가

<br>

### 1-2. 결론

논의가 길어져서, request 와 response 객체 구현을 완성한 후에 다시 논의해보기로 했다.
일단 두 번째 방법과 첫 번째 방법의 차이가 코드 몇 줄 밖에 없기 때문에 첫 번째 방법으로 구현해놓기로 했다.

<br>

## 2.

### 2-1. 논의 사항

Client 가 여러개의 서로다른 요청을 보냈을 때 어떻게 관리할 것인가?

<br>

### 2-2. 현재 상황

클라이언트가 보낸 요청을 server에서 read 하여 raw msg 변수에 담아서 출력한다.
지금은 아래처럼 client fd 와 요청을 받는다.
<br>

```c++
map<int, Request *> clients
```

<br>

### 2-3. 정리

논의 사항은 다음과 같다.

1. `Request *` 가 적절한가?

2. 위 형태를 유지할 것인가?

3. 다음과 같은 형태는 어떤가? <br>
   `map<int, map<int, Request *>> clients`

<br>

### 2-3. 결론

요청은 비동기로 받지만, 처리는 비동기가 아닌 동기로 처리하는 웹서버를 만들 것이기 때문에, 위 형태를 유지해도 괜찮을 것 같다.

```c++
map<int, Request *> clients
```

<br>

### 2-4. 참고

#### 2-4-1.

client 생성하는 방법을 아래로 수정했다. <br>

```c++
map<int, std::string> clients

----------------------------->

map<int, Request *> clients
```

<의의>
HTTP request message가 `Server class`에서 `Request class`로 이동했다!

<br>

#### 2-4-2.

클라이언트가 보낸 요청을 server에서 read 하여 raw msg 변수에 담아서 출력했다.
<br>

요청을 받고 터미널에 출력하는 테스트를 다음과 같이 진행했다. <br>

<p align='center'><img src="https://user-images.githubusercontent.com/85930183/231055746-2adb44bf-c1f7-4dea-a954-44f2dec01e9d.png"></p>

1. 브라우저에서 http://localhost:8080/?message=hi 를 입력

2. 서버가 요청 받은 메시지를 출력함

3. 개발자 도구에서 네트워크를 확인하면 메시지가 보내진 걸 볼 수 있음 <br>
   ( 이건 그냥 추가함 )
