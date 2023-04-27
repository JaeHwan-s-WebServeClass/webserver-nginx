# Mockup response 객체 구현

reponse 객체를 구현하기 앞서 간단한 request - reponse 코드를 구현하기로 했다.

1. response 객체 및 메시지 생성
2. 클라이언트에서 localhost:8080 으로 request
3. 서버에서 클라이언트에 response

## 1. response 메시지 생성
response 객체를 생성하고, set 으로 생성자에서 직접 메시지를 넣었다.
생성자에서 null 로 되어있지만, setter 에서 직접 넣어주고 있다.
```c++
void setHttpVersion(std::string);
void setStatus(std::string);
void setStatusMsg(std::string);
void setHeader(std::string, std::string);
void setEntity(std::string);

...

Response::Response() : response_msg("") {
  setHttpVersion("null");
  setStatus("null");
  setStatusMsg("null");
  setHeader("null", "null");
  setEntity("null");
  setResponseMsg();
}

```

## 2. 요청 및 응답
[ 이미지 참고 ]
<p align='center'>
    <img width="1319" alt="image" src="https://user-images.githubusercontent.com/85930183/231362689-1fe96932-d668-43d1-8766-8c5ccd88a710.png" width='95%'>
</p>

1. 클라이언트(브라우저)에서 서버(localhost:8080)로 request 를 보냄.

2. response 를 터미널에 출력 및 클라이언트에게 보냄.

3. 개발자도구 네트워크에서 요청 응답이 잘 된걸 확인할 수 있음.
