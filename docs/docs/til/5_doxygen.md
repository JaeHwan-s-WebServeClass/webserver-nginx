# Doxygen?

## Doxygen
Doxygen 은 도큐멘테이션 생성기이자 소프트웨어 참조 설명문을 작성하기 위한 도구이다. 다음과 같은 구조로 작성한다.

```
  /// @brief
  /// @param
  /// @param
  /// @param
  /// @return
```

예를 들면 이렇다.

```
public:
  /// @brief ServerSocket constructor
  /// @param protocol AF_INET
  /// @param port 8080
  ServerSocket(sa_family_t address_family, int port);

 private:
  /// @brief 소켓 생성하는 함수
  /// @param  PF_INET
  /// @param  socket_stream
  /// @param  0
  /// @return socket fd
  int safeSocket(int, int, int);
```
safeSocket 함수 위에 마우스를 올리면, 다음처럼 함수에 대한 설명이 생긴다.
<p align="center"><img src="https://user-images.githubusercontent.com/85930183/230557259-4d7ddee8-e97f-4115-a2f4-6dd51e746510.png" width="90%"></p>

## 좋은 점은??
헤더에 함수를 선언할 때, 매개변수의 이름을 굳이 안 적어도 된다.

```
// 원문은 다음과 같은데,
// 헤더에서는 domain, type, protocol 는 doxigen이 있기 때문에 뺄 수 있다.

int ServerSocket::safeSocket(int domain, int type, int protocol)
```
