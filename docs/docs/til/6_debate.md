# 구현을 위한 논의

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

---

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

---

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

-   [ 참고 ] 오버헤드?

    오버헤드는 어떤 처리를 하기 위해 들어가는 간접적인 처리 시간 · 메모리 등을 말한다. 예를 들어 A라는 처리를 단순하게 실행한다면 10초 걸리는데, 안전성을 고려하고 부가적인 B라는 처리를 추가한 결과 처리시간이 15초 걸렸다면, 오버헤드는 5초가 된다

---

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

---

## 5. [2023.04.14(금)]

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

---

## 6. [2023.04.14(금)]

### 6-1. 상황

요청 메시지의 head (시작줄 & 헤더) 파싱을 마무리하고, entity 파싱을 해야할 차례다..! 쉬울 줄 알았던 entity 파싱... 생각치 못한 entity 의 특징이 있었다. <br>

#### 특징. entity 의 내용은 두 가지 종류로 나뉜다.

-   Chunked

    Chunked 로 들어올 경우, Content-Length 에 대한 정보가 없다. 그리고 chunk 의 값이 다르다.

     <img src="https://user-images.githubusercontent.com/85930183/232204903-3d1af4f5-b1ee-4f9e-860f-b24b356f9d94.png" width='80%'>

-   Content-Length

    length 로 들어올 경우, Chunked 가 없다.

     <img src="https://user-images.githubusercontent.com/85930183/232205330-c1b3fabc-7be5-4529-ad5f-fd965150c516.png" width='80%'>

<br>

 <!-- #### 특징 2. chunked 로 올 경우, 내용을 역순으로 받아온다. 
 
-   참고<br>
    <img src="https://user-images.githubusercontent.com/85930183/232204412-d04d520b-2184-44eb-bc38-18532a18deea.png" width='80%'> -->

<br>

### 6-2. 우리가 생각한 entity parsing

#### step 1. 분기

-   entity를 읽어올 때부터 chunked / content-length 두 종류로 분기해야 함

#### step 2. content_length

-   content-length 만큼 읽는다.
-   다 읽고, parsing 후 entity_done=true

#### step 3. chunked

-   chunked를 읽고, 그 사이즈만큼 read 해야함.
-   다 읽고, parsing 후 entity_done=true

<br>

### 6-3. 고민 사항

<br>

#### 6-3-1. entity 를 read 할 위치

-   우리가 생각한 entity parsing 을 한다면 server.cpp 의 109 line 분기문이 (read 하는 부분) Transaction 으로 넘겨져야할 것 같다.

    왜냐하면 Chunked 로 entity 가 들어온다면 Chunk size 에 따라 read 할 buffer size 를 조절해야 파싱하기 수월한데, 현 구조에서는 read 가 Transaction 밖에 있어서, chunk 에 따라 buffer 의 size 를 조절하지 못하기 때문이다.

    즉, Trasaction 에서 read 를 한다면 buffer 의 size 를 조절할 수 있어서 파싱하기 수월하다.

<br>

<!-- #### 6-3-2. 역순으로 들어오는 chunk 로 들어오는 데이터

-   chunked 는 데이터가 역순으로 들어오기 때문에, 읽으면서 바른 순서로 돌려야한다.
    -   stack 이나 queue 자료구조에 대한 이야기도 했으나, 매번 chunk 값을 해석해서 그만큼 read 한다면 필요하지 않을 수도 있을 것 같다. 추후에 다시 논의가 필요하다. -->

<br>

### 6-4. 결론

논의가 더 필요하다.

#### < 다음주에 논의할 내용 >

```
1.   Server 의 read/write도 개편<br>
     Server는 event만 감지하며, 그 외의 일은 모두 Transaction이 하자?!

2.   Server, Transaction, Response, Request 객체간에 어떤 역할을 할지 명확하게 정리!

3.   setRawMsg 를 전체적으로 개편?!

4.   parsing (configuration, entity, header) 정리!
```

<br>

### 6-5. 1, 2번 논의 후 결론 (2023.04.17)

오늘 논의할 내용의 1번과 2번을 정리했다. 전체적으로 두 번의 구조 변화가 있었다.

```
1.   Server 의 read/write도 개편<br>
     Server는 event만 감지하며, 그 외의 일은 모두 Transaction이 하자?!

2.   Server, Transaction, Response, Request 객체간에 어떤 역할을 할지 명확하게 정리!
```

<br>

#### 6-5-1. 첫 번째 구조...

서버 객체에서 read 하고 write 하던 코드를 transaction 객체의 executeTransaction 함수로 옮겼다.

-   `server 객체 -> transaction객체::executeTransaction` 함수 호출<br>
    -> executeTransaction 내부에서 나뉘는 분기
    1. executeRequest : read 하여 request 메시지 만듬
    2. executeMethod : 만들어진 request 메시지로 Method 실행
    3. executeResponse : write 하여 response 메시지 만듬

하나의 함수에서 같은 레벨의기능을 처리하는 의미가 있는 코드라고 생각되어 좋은 구조라고 생각했지만,
얼마 지나지 않아 생각이 바뀌었다.

하나의 함수에서 모든 기능을 처리하는 건 보기엔 좋았지만, 분기를 나눠 처리하는데 플래그 등이 추가로 필요했고, 그 과정에서 코드가 지저분해졌기 때문이다.
<br>
<br>

#### 6-5-2. 두 번째 구조...

코드가 지저분해지는 것뿐만 아니라, 이렇게 할 거라면 굳이 executeTransaction 안에 넣을 필요가 있을 필요가 있을까? 라는 의문이 들었다.

다 같이 논의한 결과 그럴 필요가 없다는 결론이 나왔다. 처음에 만들었던 구조가 아닌 executeTransaction 를 거치지 않고 read 와 write 를 따로 실행하는 구조로 수정했다.

-   `server 객체 -> 이벤트에 따라 transaction객체::executeRead 또는 executeMethod 또는 executeWrite` 각각 실행<br>
    -> server 에서 나뉘는 분기
    -   executeRead : read 하여 request 메시지 만듬
    -   executeMethod : 만들어진 request 메시지로 Method 실행
    -   executeWrite : write 하여 response 메시지 만듬

추가로 disconnectClient 에 대한 처리가 필요했는데, executeRead, executeWrite 반환값으로 처리했다.

<br>

### 6-6. 3, 4번 논의 후 결론 (2023.04.17)

1, 2 번 논의를 바탕으로 코드를 구현한 후, 3, 4번 논의를 진행했다.

```
3.   setRawMsg 를 전체적으로 개편?!

4.   parsing (configuration, entity, header) 정리!
```
버퍼에 쌓이는 데이터를 getline 으로 한 문장씩 읽어와 파싱하는 부분을 request 객체의 setRawMsg에서 하고 있었다. 의미상 요청 받는 객체가 파싱하는게 어색했는데, 그걸 새로 만든 executeRead 로 옮겼다.

<br>

---

## 7. [2023.04.18(화)]

6 번까지 논의 후 본격적으로 head 와 entity 파싱을 시작했고 여러가지 방법에 대해 논의했다.

여러가지 방법 중, config 파일에 `max_head_size` 와 `max_body_size` 옵션을 설정하는 방법을 선택했다. 이 방법의 특징은 head 나 body 의 크기가 max size 보다 클 경우 에러 상황으로 간주한다는 것이다. ( ❗️단, chunked 일 경우 max_body_size 의 제한을 무시하고 모두 받아온다. )

이제 `max_head_size` 와 `max_body_size` 를 어떻게 활용할지 논의했다.

<br>

### 7-1. 논의

논의의 큰 틀은 이렇다.

1. transaction 클래스에 max_head_size 와 max_body_size 로 멤버 변수 버퍼를 만들어 파싱.

    ```c++
    class transaction {
      private:
        ...
        head_buf[max_head_size];
        entity_buf[max_body_size];
        ...
    }
    ```

2. buffer_size 로 다 읽어온 후 max_head_size 와 max_body_size 크기로 예외처리하기

    ```c++
      buf[BUFFER_SIZE];
      ...
      전체 요청메시지 파싱
      ...
      if (head 크기 > max_head_size || entity size > max_body_size)
        throw error;
    ```

<br>

### 7-2. 각 방법의 특징

#### 7-2-1. 1번
1. 헤드와 바디가 max size 를 벋어나는 즉시 에러처리를 할 수 있다.
2. 한 번만 read 한다.
3. transactioin 객체를 생성하면, max 크기만큼의 head_buf 와 entity_buf 가 메모리를 차지한다.

#### 7-2-1. 2번
1. 1번에 비해 차지하는 메모리가 적다.
2. 여러번 read 하며 head 와 entity 파싱을 해야한다.
3. 모두 read 한 후에 max size 를 비교하여 에러처리한다.

<br>

### 7-3. 결론
우리는 1번에서 3번째 특징이 매우 큰 단점이라고 생각했다. 클라이언트의 생애주기동안 max 사이즈 크기의 버퍼를 가지고 있는게 불필요한 메모리 차지로 보였기 때문이다.

그래서 2번 방식으로 진행하기로 결정했다.

<br>


---

## 8. [2023.04.21(금)] multiple socket 처리!
### 8-1. 상황
요청 메시지 파싱을 마치고, config 파일 파싱을 진행했다. config 에는 server 블록이 여러개 존재할 수 있는데 어떤 의미인지에 대해 논의했다.

우리가 만든 server 에 여러 개의 포트가 연결될 수 있다는 의미로 의견을 합쳤다. 즉, 연결된 모든 포트로 클라이언트들과 트랜잭션(요청과 응답)을 처리할 수 있다.

코드 수정이 필요했다. 현재 코드는 server_socket 하나로 포트 하나만 할당해서 소켓을 server 에 연결한다. server_socket 을 여러 개로 생성해서 server 에 연결해야한다.

<br>

### 8-2. 결론
vector 를 사용해서 여러개의 server_socket 을 넣고, server 에 연결했다.
* 사용한 자료구조 <br>
    `std::vector<ServerSocket *> server_socket;`


<br>

---
## 9. [2023.04.21(금)] read & write error

### 9-1. 상황
multiple sockets 이 작동하도록 코드를 모두 구현한 후, 포스트맨으로 테스트를 진행하는 과정에서 read 와 write 에러가 발생하며 프로그램이 종료됐다.

### 9-2. 에러 픽스

#### 9-2-1. read 에러
**↘︎ how to fix**  
multiple server 를 만들면서 iterator 를 사용하는 for 문을 추가했었다. 그 과정에서 executeRead() 가 반복문 안에 잘못 들어가 있어서 error 가 발생했었다.
executeRead() 를 for 문 밖으로 옮겨서 고쳤다.

<br>

#### 9-2-2. write 에러
**↘︎ how to fix**
1.  `echo $?` 를 통해 exit status code 가 141 이라는 걸 확인했다. 141 상태 코드는 sigpipe 와 관련이 있었다.

    ```
    141의 경우, 이는 SIGPIPE 신호에 의해 프로세스가 종료되었음을 나타냅니다. SIGPIPE는 파이프로 연결된 프로세스 간에 데이터를 전송할 때 발생할 수 있는 오류를 나타내는 신호입니다.

    예를 들어, 프로세스 A가 파이프로 연결된 프로세스 B에게 데이터를 보내는데 B가 이를 처리하지 않아 파이프 버퍼가 가득 차면, A는 SIGPIPE 신호를 받아서 종료됩니다.

    따라서 141의 경우는 파이프 연결 중에 오류가 발생하여 종료된 상황을 나타내는 것입니다. 이 상황에서는 파이프 연결을 확인하고 오류를 수정해야 합니다.
    ```

2. 서버가 클라이언트의 요청에 대한 응답을 한 후, 클라이언트는 서버와 connect 를 종료하는데 서버는 소캣(파이프)를 통해 클라이언트에 write 하려고 계속해서 시도하고, 이 시도는 처리되지 않아 발생하는 에러였다.

3. recv, send 함수 앞/뒤로 SIGPIPE를 ignore 로 처리했다. <br> 하나의 트랜잭션  cycle이 끝나면 해당 server 에서 client disconnect 하며 해결했다.

<br>

---

## 10. [2023.04.24(월)] config 파싱

### 10-1. 상황
이전에 진행하다 이중 location 처리와 각종 에러 처리에 대한 고민으로 구현을 잠시 미웠던 config 파일 파싱을 재개했다.
파싱을 하기 위해 크게 두 가지 결정을 해야한다.

1. config 파일 규칙 정하기
2. 파싱 구조 정하기

자세한 내용은 config-general-rules 페이지에 정리가 되어있다.

### 10-1-1. config 파일 규칙
config 파일 규칙을 어떻게 해야할까? nginx 를 참고하며 웹서버 구현을 진행하긴 했지만, nginx config 까지 규칙을 따라할 필요는 없다고 의견이 모였다. 우리는 우리만의 nginx 규칙을 정하자!

다음 규칙들을 정했다. (자세한 내용은 config-general-rules 을 참고)

```
1. semicolon(;) 포함
2. bracket {} 적용
3. directive - key 와 value 는 '\t' 을, value 들은 ' ' 를 delimiter 로 나누기
4. location - 다중 location 처리 X
5. directory: `/dir_name`의 형식으로
...
```

❗️ 예외처리는 `{}` 유효성 검사만 진행하기로 했다.

<br>

### 10-1-2. 파싱 구조 정하기
파일을 어떤 구조로 파싱해야할까 논의했다. `{}` 유효성 검사는 진행하기로 했기 때문에 함께 고려해야했다.
location 도 `{}` 가 있기 때문에 server 의 `{}` 와 구분할 수 있어야한다.

1. `{` 과 `}` 를 delimiter 로 server 와 location 을 구분하고 split 하는 방식
2. `{` 과 `}` 의 짝을 맞춰가며 server 와 location 을 구분하고 내용을 한 줄씩 split 하는 방식
3. 서버와 location 의 `{` 과 `}` 를 flag 설정하여 분기를 나눈 뒤 한 줄씩 split 하는 방식

3 가지 방식을 논의했다. yichoi 가 3 번째 방식 수도코드를 구현했는데, 가독성도 좋고 불필요한 자료구조를 사용할 필요가 없어보여서 그대로 진행하기로 했다.


```c++
while (std::getline()) {
    if (line == "server {") {
        server_fl = true;
    } else if (isLocation()) {
        location_fl = true;
    } else if (server_fl == true && location_fl == true && line == "}") {
        location_fl = false;
    } else if (server_fl == true && location_fl == false && line == "}") {
        server_fl = false;
        config_data.push_back();
    } else {
        goParsing();
    }
}
```

<br>

### 10-2. 후기
파싱으로 몇 일동안 고생할 줄 알았는데, 하루만에 끝냈다니... 훌륭한 하루였다 👍

<br>

---

## 11. [2023.04.25(화)] config location 처리

### 11-1. location 처리 방법
config 파일의 location 을 처리해야한다. location 처리를 위한 location rule 을 정했다. 우리는 요청 메시지에 넘어오는 url 을 2 가지 종류로 나눴고, config 에 있는 location 관련 구성요소는 3 가지로 나눴다.

<img src="https://user-images.githubusercontent.com/85930183/234232858-8aa422a4-3b85-439e-893d-333a34971c1d.png" width="90%">

( ❗️ localhost:80 은 요청 메시지로 넘어온 데이터에 없기 때문에 나누지 않았다.)

#### 11-1-1. 요청 메시지
1. request_location <br>

    * `/example` : 찾을 파일의 위치를 의미

2. request_filename <br>

    * `/index.html` : 찾을 파일을 의미 


#### 11-1-2. config
3. docroot <br>

    * `root` : server 의 root 위치를 의미


4. config_location

    * `map<string, t_location> 의 string` : location 블록의 key 를 의미

5. loc_root

    * `t_location 내부의 root` : location 블록의 value 를 의미


### 11-2. config location 처리 방법
a. 1 과 4 를 비교해서 같은 경우, 4로 map 안에서 5 를 찾는다.

b. 3(docroot) + 5(locroot) + 2(reqest_filename) 으로 만든다.

c. access 를 이용해서 b 에서 만든 위치가 접근 가능한지 확인한다.

<br>

### 11-3. 논의 사진

<img src="https://user-images.githubusercontent.com/85930183/234231482-585545f6-1664-4b41-97a3-4a4af5dc6409.jpeg" width="70%">

<img src="https://user-images.githubusercontent.com/85930183/234231516-38e7df29-d5b6-47e7-807c-89145b5326c9.jpeg" width="70%">


---

## 12. [2023.05.02(화)] cgi???

### 12-1. 상황
구현해야될 기능은 post, delete 와 cgi 만 남았다. http 프로토콜은 정적인 처리밖에 못하기 때문에 동적인 처리를 할 수 있는 cgi 가 필요하다. cgi 는 common gateway inerface 의 줄임말로 서로 다른 프로토콜을 연결해주는 인터페이스라고 생각하면된다.

예를 들어 아래와 같은 코드가 있다고 하자.
```c++
<!DOCTYPE html>
<html>

<head>
    <title>Post</title>
</head>

<body>
    <form action="post.py" method="POST">
        <input type="text" name="name" />
        <input type="text" name="email" />
        <input type="submit" value="Submit" />
    </form>
</body>

</html>
```

`form` 태그 안에 `action="post.py"` 가 있다. `submit` 버튼을 누르면 `post.py` 파일이 실행되어야한다. 하지만 여기서 문제는 `http 프로토콜`로 처리할 수 없다. `py` 파일을 실행할 수 있게 하는 코드가 필요하다. 즉, 이 코드는 `http 프로토콜`로 html 코드를 처리하다가 `post.py` 를 만나면 이 파일을 실행할 수 있게 넘어가는 인터페이스가 된다. 이를 **`CGI`** 라고 한다.

<br>

### 12-2. 전체 로직

전체적인 `post.py` 처리 로직은 아래와 같다.

1. 요청 메시지를 파싱해서 `post.py` 의 존재를 알아야함.
2. `method` 가 `post` 인지 확인한다.
    * `post` 는 `CGI` 가 필요하다. (delete 도 생각해봐야할 듯)
3. `post.py` 이고 `post` 이면 `fork()` 와 `execve()` 로 `post.py` 를 실행한다.
4. 실행한 결과를 활용해서 응답 메시지를 만들어 클라이언트에게 전달한다.

3 번 _ `fork() -> execve( ~ )` _ 부분이 **`CGI`** 가 된다.

<img src="https://user-images.githubusercontent.com/85930183/235587194-b95b80c3-83ce-4785-bd3a-a913394ed8ae.jpeg" width="90%">

---

## 13. [2023.05.03(수)] post 재논의

 get cgi 구현 및 post 재논의 