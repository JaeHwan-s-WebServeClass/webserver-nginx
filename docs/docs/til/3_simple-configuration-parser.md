# nginx.conf 파일 파싱
## 음...
무작정 파싱을 하려다보니, 막막했다.
그래서 configuration 파일에 필요한 최소 옵션들을 먼저 파싱해보며 테스트를 진행해보기로 했다.
<br>

***

## 할 일
* 아래 파일을 파싱해보기 
```text
server {
    listen       80; // host:port 의 첫 번째 서버가 이 host:port 의 기본값이 된다.
    server_name  example.com www.example.com; // server_names 을 설정할지 여부를 결정한다.

    error_page  404              /404.html; // 기본 오류 페이지를 설정한다.
    client_max_body_size 10m; //클라이언트 본문 크기를 제한한다.

    location / {
        autoindex on;
    }
}
```

***

## 진행 과정
파일을 파싱하며 데이터들을 어떤 자료구조에 넣어야할지 많은 대화를 했다.

☝🏻 처음에 이야기했던 건 vector 였다.

예를 들면 이런 형태
```
server_name[0] = example.com;
server_name[1] = www.example.com;
```

❗️ 하지만, location 의 경우 여러 번 나올 수 있기 때문에 vector 보다, map 이 좋다는 의견이 나왔다.
```
// 자료구조 선언
std::map<std::string, map<std::string, std::string>> location;

// 예시
location[0] = <std::string(/), map<std::string(autoindex), std::string(on)>
location[1] = <std::string(/example), map<std::string(autoindex), std::string(on)>
```
***
### BUT... 자료구조 결정과 파서 구현을 잠시 미뤘다...
config 파일에 들어올 수 있는 여러가지 옵션들을 고려하다보니 파서 구조 결정하기가 어려웠다.
가장 어려웠던 건, 이중... 삼중... location 설정 시 어떻게 처리해야할지였다.

1. 블록('server{  }, 'location{  }') 과 디렉티브('CMD ; 로 끝나는 문장') 를 기준으로 나누기
2. {, } 를 스택에 쌓아가며 개수를 맞춰서 server 와 ,location 그리고 directive 로 나누기
3. server 와 location 을 딜리미터로 나누기
<br>
등등...

고민을 많이 하다가, silee 와 kkim 등에게 도움을 요청하고 조언을 받아본 결과
파서는 잠시 미루고 에코 서버 구현을 먼저 해보기로 했다.