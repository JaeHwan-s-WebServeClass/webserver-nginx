# request 메시지 파싱

## 1.
파싱을 하기 위한 몇 가지 순서를 정했다.

1. 클라이언트에서 요청에 대해 어떤 형태로 메시지가 오는지 확인
2. 메시지를 헤드(시작줄 + 헤더) 와 바디로 구분하여 파싱
* 시작줄부터 헤더는 개행 하나로 구분하지만, 바디와는 개행 두개로 구분하기 때문에 헤드와 바디로 나눔
3. 시작줄 : method, url, version 로 파싱 <br>
헤더 : map 자료구조에 ':' 를 구분자로 두 개의 데이터를 넣음 <br>
바디 : vector 에 string 으로 넣음

<br>


## 2. 클라이언트 요청 메시지 확인
아래 요청을 보냈다.<br>
`http://localhost:8080/?message=hi`

다음 응답이 왔다.

```http
GET /?message=hi HTTP/1.1
Host: localhost:8080
Connection: keep-alive
Cache-Control: max-age=0
sec-ch-ua: "Chromium";v="112", "Google Chrome";v="112", "Not:A-Brand";v="99"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "macOS"
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br
Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7
```

<br>

## 3. 헤드와 바디 파싱
메시지를 헤드(시작줄 + 헤더) 와 바디로 구분하여 파싱했다.

* 포스트맨으로 테스트 했다. POST 로 http://localhost:8080 을 보냈다.
<p align='center'> <img src="https://user-images.githubusercontent.com/85930183/231100591-dc6a8343-8038-4f68-941b-1cefc6f3cd49.png" width="95%">
</p>

<br>

## 4. 시작줄 헤더 바디 파싱
이제 헤드를 시작줄과 헤더로 나누고, 바디는 vector 에 string 으로 push_back 한다.

시작줄과 헤더는 다음과 같이 한 번 더 파싱한다.

<br>

### 4-1. 시작줄
method, url, version 으로 파싱

<br>


### 4-2. 헤더
옵션과 값들을 map 안에 key & value 로 넣음

* 포스트맨으로 테스트 했다. POST 로 http://localhost:8080 을 보냈다.
<p align='center'><img src="https://user-images.githubusercontent.com/85930183/231134351-935d929e-6cd2-4764-99a1-cf972213c287.png" width="90%"></p>
