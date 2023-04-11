# 컨벤션

## 깃 컨벤션

-   깃 허브 이슈 기능으로 프로젝트 진행 및 관리
-   pull request (approve 필요) 사용
-   깃 컨벤션은 vscode 의 lint 기능을 사용해서 Google 또는 MS 적용 예정

<br>

---

## TDD
할지 말지 추후에 논의하기

<br>

---

## 코드 컨벤션

### ↘︎ 네이밍 규칙

-   변수의 이름은 모두 소문자와 밑줄을 사용합니다.
    `예: my_variable`
-   함수의 이름은 camelCase를 사용하며, 첫 글자는 소문자로 시작합니다.
    `예: myFunction`
-   클래스와 구조체 명은 UpperCamelCase를 사용하며, 첫 글자는 대문자로 시작합니다.
    `예: MyClass`
-   상수, 매크로 등의 이름은 모두 대문자와 밑줄을 사용합니다.
    `예: #define MY_MACRO`

<br>

### ↘︎ 함수 규칙

-   함수의 인자는 가능한 한 const로 선언합니다.
-   함수에서 반환되는 값은 가능한 한 const로 선언합니다.

<br>

### ↘︎ 클래스 규칙 (추후 바뀔 수 있음)

-   private 멤버에는 \_ 를 넣어준다.
-   protected 멤버에는 \_\_ 를 넣어준다.

```cpp
class A {
private:
    int _a;
    int _b;
    int _func(int c);

protected:
    int __d;
    int __e;
    int __func(int f);

public:
    int g;
    int h;
    int func(int i);

};
```

<br>

### ↘︎ 중괄호 규칙

-   중괄호는 줄바꿈 없이 한다.
-   한 줄 일때도 { } 를 넣는다.

> O

```cpp
ex 1.
int func(void) {
	...
}

ex 2.
if (...) {
	...
} else if {
	...
} else {
	...
}
```

> …

```cpp
ex 1.
int func(void)
{
	...
}

ex 2.
if (...)
{
	...
}
else if
{
	...
}
else
{
	...
}
```

<br>

### ↘︎ 변수 선언 & 헤더 파일 함수 선언 규칙

-   라인 맞추기

```cpp
int       a;
double    b;
long long c;
```

<br>

### ↘︎ 들여쓰기 규칙

-   들여쓰기는 탭(4칸)을 사용합니다.

<br>

### ↘︎ 주석 규칙

-   클래스와 함수에 대한 주석은 Doxygen 스타일을 사용합니다.
-   주석은 한글로 작성한다.
-   코드 라인에 주석을 달 때는 "//"를 사용하며, 블록 주석은 "/\* \*/"를 사용합니다.
