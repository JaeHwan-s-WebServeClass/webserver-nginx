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

***
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

### 2-4. 결론

요청은 비동기로 받지만, 처리는 비동기가 아닌 동기로 처리하는 웹서버를 만들 것이기 때문에, 위 형태를 유지해도 괜찮을 것 같다.

```c++
map<int, Request *> clients
```

<br>

### 2-5. 참고

#### 2-5-1.

client 생성하는 방법을 아래로 수정했다. <br>

```c++
map<int, std::string> clients

----------------------------->

map<int, Request *> clients
```

<의의>
HTTP request message가 `Server class`에서 `Request class`로 이동했다!

<br>

#### 2-5-2.

클라이언트가 보낸 요청을 server에서 read 하여 raw msg 변수에 담아서 출력했다.
<br>

요청을 받고 터미널에 출력하는 테스트를 다음과 같이 진행했다. <br>

<p align='center'><img src="https://user-images.githubusercontent.com/85930183/231055746-2adb44bf-c1f7-4dea-a954-44f2dec01e9d.png"></p>

1. 브라우저에서 http://localhost:8080/?message=hi 를 입력

2. 서버가 요청 받은 메시지를 출력함

3. 개발자 도구에서 네트워크를 확인하면 메시지가 보내진 걸 볼 수 있음 <br>
   ( 이건 그냥 추가함 )

***
## 3. [2023.04.12(수)]

### 3-1. 논의 사항

트랙잭션 (요청, 응답) 객체 구조에 대해 논의했다. 필요한 객체에는 트랜잭션, 요청, 응답 3가지가 있었다.

트랜잭션 클래스와 (요청, 응답) 클래스를 중첩 클래스 방식으로 설계할지 컴포지션 방식으로 설계할지가 고민이었다.

#### 3-1-1. 중첩

```c++
class '트랜잭션' {
   private:
      class '요청' { }
      class '응답' { }
   public:
      ...
}
```

<br>

#### 3-1-2. 컴포지션

```c++
class '요청' { }
class '응답' { }

class '트랜잭션' {
   private:
      '요청' request;
      '응답' response;
   public:
      ...
}
```

### 3-1-3. 장단점

#### 3-1-3-1. 중첩 클래스 방식의 장단점

-   장점

    1. 클래스 간의 관계가 명확해지므로 코드 가독성이 좋아집니다.

    2. 중첩 클래스는 외부 클래스의 private 멤버에 접근할 수 있으므로, 외부 클래스의 멤버 함수에서 중첩 클래스를 더욱 유연하게 사용할 수 있습니다.

-   단점

    1. 중첩 클래스를 외부에서 사용할 수 없기 때문에, 코드의 재사용성이 떨어집니다.

    2. 중첩 클래스를 변경하게 되면, 외부 클래스의 코드도 함께 변경해야 할 수 있으므로 유지보수성이 낮아집니다.

#### 3-1-3-2. 컴포지션 방식의 장단점

-   장점

    1. 클래스 간의 결합도가 낮아져, 코드의 재사용성이 높아집니다.

    2. 독립 클래스는 외부에서 자유롭게 사용할 수 있으므로, 코드의 재사용성이 높아집니다.

-   단점

    1. 클래스 간의 관계가 명확하지 않아, 코드 가독성이 떨어집니다.

    2. 외부 클래스의 private 멤버에 직접 접근할 수 없기 때문에, 코드의 유연성이 떨어집니다.

### 3-2. 결론

이론적으로는 중첩 클래스가 적절한 처리라고 생각했다.

트랜잭션 클래스 밖에서 request, response 클래스의 사용 가능성을 근본적으로 차단할 수 있고 request, response 객체 사이에서 공유 변수들을 설정하기 편하기 때문이다.

하지만 사용하는 측면에서 유지보수 측면, 클래스를 재구조 하는 것에 따르는 오버헤드가 증가하게 되어서 컴포지션 방식으로 유지하는 것으로 결정했다.

* [ 참고 ] 오버헤드?

   오버헤드는 어떤 처리를 하기 위해 들어가는 간접적인 처리 시간 · 메모리 등을 말한다. 예를 들어 A라는 처리를 단순하게 실행한다면 10초 걸리는데, 안전성을 고려하고 부가적인 B라는 처리를 추가한 결과 처리시간이 15초 걸렸다면, 오버헤드는 5초가 된다


***
## 4. [2023.04.13(목)]

### 4-1. 상황
요청 메시지의 '메서드(method)와 리소스(url) 유효성 검사'를 진행하고, 각 메서드에 맞는 요청과 응답을 처리하는 과정을 구현해야한다.

### 4-2. 논의 사항
1. method, resource, GET DELETE POST 까지 코드 진행하며 상태코드를 int 로 반환 후 switch case 로 한번에 처리
2. method, resource, GET DELETE POST 각 줄을 진행하며 객체에 상태값과 메시지를 세팅하며 바로 바로 처리

   <p align='center'><img src="https://user-images.githubusercontent.com/85930183/231719510-85329c7e-39da-4a93-87f0-eb4dcb18aa0e.jpeg" width='95%'></p>

### 4-2. 결론
method, resource 유효성 검사 코드와 GET DELETE POST 코드를 진행하면 각 상태 코드가 나오는데,
상태 코드를 함수 안에서 한번에 처리하여 트랜잭션의 흐름과 상태 코드 처리를 한 눈에 볼 수 있게 했다.

즉, 상태 코드 반환하는 부분과 객체에 세팅하는 부분을 한 곳으로 모았다.<br>
<b>↘︎ switch case 구문으로 처리 계획</b>

***
## 5 [2023.04.14(금)]
### 5-1. 상황
트랜잭션 객체를 통해서 요청, 응답을 처리하기로 했다. 하지만 현재 코드는 트랜잭션 객체는 있지만 요청, 응답을 직접 처리하기 때문에 트랜잭션 객체를 통하도록 수정이 필요하다. 현재 코드 진행 상황은 이렇다.

<p align='center'><img src="https://user-images.githubusercontent.com/85930183/232207851-e04cfad6-9d77-4b7f-9b99-175a3d612936.png" width='80%'></p>

1. READ 는 클라이언트 요청을 감지하고, 요청 메시지를 받고 처리한다.<br>
2. WRITE 는 응답 메시지를 만들고, 클라이언트에게 보낸다.

앞서 얘기했듯이 READ 의 첫 줄을 보면, `new transaction` 을 생성하지만 transaction 객체를 통해 요청 메시지를 처리하지 않고, request 객체의 멤버 함수인 setRawMsg() 를 통해 메시지를 처리한다.


### 5-2. 진행 계획
다음 그림처럼 server 에서 READ 로 요청을 감지하면 Transaction 객체로 넘기도록 구현하자.<br>이후 Transaction 객체는 Request 객체의 parse 를 호출하도록 한다.<br>❗️ 그림의 parse() 는 setRawMsg() 이다.

<p align='center'><img src="https://user-images.githubusercontent.com/85930183/232208099-d8f976f2-0ad2-4ddf-9ba2-2df8b24cb608.png" width='80%'></p>

### 5-3. 결론
`executeTransaction()` 를 만들었다. 해당 멤버 함수를 통해 request 객체에 접근하여 요청 메시지를 받을 수 있게 구현했다.

### 5-4. 그 때 그 순간

<img src="https://user-images.githubusercontent.com/85930183/232208805-d5f2d6c4-aaeb-4638-b58d-81ceb116cae0.png" width='30%' float='left'>
<img src="https://user-images.githubusercontent.com/85930183/232208809-502d22d0-cb72-4863-a90e-54db9162b619.png" width='33%' float='left'>
<img src="https://user-images.githubusercontent.com/85930183/232208811-a5bf519f-4ddb-486f-b050-e9f4248ff8eb.png" width='35%'>


***

## 6 [2023.04.14(금)]
### 6-1. 상황
요청 메시지의 head (시작줄 & 헤더) 파싱을 마무리하고, entity 파싱을 해야할 차례다..! 쉬울 줄 알았던 entity 파싱... 생각치 못한 entity 의 특징이 있었다. <br>

#### 특징 1. entity 의 내용은 두 가지 종류로 나뉜다. 
   - Chunked

      Chunked 로 들어올 경우, Content-Length 에 대한 정보가 없다. 그리고 chunk 의 값이 다르다.

      <img src="https://user-images.githubusercontent.com/85930183/232204903-3d1af4f5-b1ee-4f9e-860f-b24b356f9d94.png" width='50%'>
   
   - Content-Length

      length 로 들어올 경우, Chunked 가 없다.

      <img src="https://user-images.githubusercontent.com/85930183/232205330-c1b3fabc-7be5-4529-ad5f-fd965150c516.png" width='50%'>

<br>

#### 특징 2. chunked 로 올 경우, 내용을 역순으로 받아온다.
   - 참고<br>
      <img src="https://user-images.githubusercontent.com/85930183/232204412-d04d520b-2184-44eb-bc38-18532a18deea.png" width='80%'>

<br>

### 6-2. 우리가 생각한 entity parsing

#### step 1. 분기
- entity를 읽어올 때부터 chunked / content-length 두 종류로 분기해야 함
#### step 2. content_length
   - content-length 만큼 읽는다.
   - 다 읽고, parsing 후 done=true
#### step 3. chunked
   - chunked를 읽고, 그 사이즈만큼 read 해야함.
   - 다 읽고, parsing 후 done=true

<br>

### 6-3. 고민 사항
<br>

#### 6-3-1. entity 를 read 할 위치
- 우리가 생각한 entity parsing 을 한다면 server.cpp 의 109 line 분기문이 (read 하는 부분) Transaction 으로 넘겨져야할 것 같다.

   왜냐하면 Chunked 로 entity 가 들어온다면 Chunk size 에 따라 read 할 buffer size 를 조절해야 파싱하기 수월한데, 현 구조에서는 read 가 Transaction 밖에 있어서, chunk 에 따라 buffer 의 size 를 조절하지 못하기 때문이다.

   즉, Trasaction 에서 read 를 한다면 buffer 의 size 를 조절할 수 있어서 파싱하기 수월하다.

<br>

#### 6-3-2. 역순으로 들어오는 chunk 로 들어오는 데이터
- chunked 는 데이터가 역순으로 들어오기 때문에, 읽으면서 바른 순서로 돌려야한다.
   - stack 이나 queue 자료구조에 대한 이야기도 했으나, 매번 chunk 값을 해석해서 그만큼 read 한다면 필요하지 않을 수도 있을 것 같다. 추후에 다시 논의가 필요하다.

### 6-4. 결론
논의가 더 필요하다.
#### < 다음주에 논의할 내용 >
- setRawMsg 를 전체적으로 개편?!

- Server 의 read/write도 개편<br>
   Server는 event만 감지하며, 그 외의 일은 모두 Transaction이 하자?!

- parsing (configuration, entity, header) 정리!

- Server, Transaction, Response, Request 객체간에 어떤 역할을 할지 명확하게 정리!

