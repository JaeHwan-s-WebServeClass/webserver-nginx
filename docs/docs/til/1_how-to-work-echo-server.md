# 에코 서버 구현하기

에코 서버 레퍼런스를 분석하고 구현해보기로 했다.

서버에는 클라이언트의 요청들을 하나씩 처리할 수 있는 동기 방식과 동시에 처리할 수 있는 비동기 방식이 있다.(100% 맞는 말인지는 잘 모르겠다.)
(⭐️ 동시에 처리하는 것은 멀티플렉싱이라고 할 수 있다.)

***

## 1. 에코 서버란?

에코 서버는 클라이언트가 보낸 메시지를 그대로 되돌려주는 서버이다. 클라이언트는 에코 서버에게 메시지를 보내면, 에코 서버는 그 메시지를 받아서 다시 클라이언트에게 보내준다. 이를 통해 클라이언트와 서버 간의 통신이 잘 이루어지는지를 확인할 수 있다. 에코 서버는 디버깅이나 테스트 목적으로 자주 사용된다.

***

## 2. 에코 서버 레퍼런스 분석

코드에 주석으로 각 코드가 어떤 역할을 하는지 분석했다.

### 2-1. 클라이언트의 요청을 하나씩 처리하는 서버

#### ☝🏻 서버 코드
[server](https://github.com/JaeHwan-s-WebServeClass/webserver-nginx/blob/1-%EC%97%90%EC%BD%94-%EC%84%9C%EB%B2%84-%EB%A7%8C%EB%93%A4%EA%B8%B0/echo_server/Chapter4%20echo/echo_server.c)

#### ☝🏻클라이언트 코드
[client](https://github.com/JaeHwan-s-WebServeClass/webserver-nginx/blob/1-%EC%97%90%EC%BD%94-%EC%84%9C%EB%B2%84-%EB%A7%8C%EB%93%A4%EA%B8%B0/echo_server/Chapter4%20echo/echo_client.c)
<br><br>
### 2-2. 멀티플렉싱 (클라이언트의 요청을 동시에 처리하는 서버)
멀티플렉싱을 할 수 있는 방식에는 여러가지가 있다. select, epoll, kqueue 등이 있다.
[select, epoll, kqueue 의 차이점](https://github.com/JaeHwan-s-WebServeClass/webserver-nginx/wiki/5.-Allowed-Functions#%F0%9F%85%80-select-poll-epoll-kqueue-%EC%9D%98-%ED%8A%B9%EC%A7%95%EA%B3%BC-%EC%B0%A8%EC%9D%B4%EC%A0%90%EC%9D%B4-%EB%AD%90%EC%97%AC)

#### 2-2-1. select
##### ☝🏻 서버 코드
[server](https://github.com/JaeHwan-s-WebServeClass/webserver-nginx/blob/1-%EC%97%90%EC%BD%94-%EC%84%9C%EB%B2%84-%EB%A7%8C%EB%93%A4%EA%B8%B0/echo_server/Chapter12%20multiplexing_select/echo_selectserv.c)

##### ☝🏻클라이언트 코드
[client](https://github.com/JaeHwan-s-WebServeClass/webserver-nginx/blob/1-%EC%97%90%EC%BD%94-%EC%84%9C%EB%B2%84-%EB%A7%8C%EB%93%A4%EA%B8%B0/echo_server/Chapter12%20multiplexing_select/echo_client.c)

#### 2-2-1. epoll
