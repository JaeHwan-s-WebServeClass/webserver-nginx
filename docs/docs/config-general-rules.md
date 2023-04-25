# configuration file rules

***

## general rules

1. semicolon(;) 포함하기 (없으면 error 처리)

2. value 값
    - error page는 한 줄에 연속해서

3. client size들은 byte 단위
    - client_max_body_size
    - client_max_head_size
    - default 값 설정해놓기

4. delimiter:
    - key랑 value는 tab으로
    - value끼리는 space로

5. directory: `/dir_name`의 형식으로

6. location:
    - 이중 location 없음
    - `location (tab) /example`의 형식으로?
    - location block을 server block의 가장 하단에 위치시켜야한다?

7. bracket {}:

***
<!-- 
## parsing

- getline으로 받아왔을 때 우선적으로 처리할 것:
    - 아예 비어있는 줄이 들어오게 되면 skip
    - 주석 문자 `#`로 시작하는 줄도 skip

- 한 줄을 받고 마지막이
    - `;`인 경우 -> key 값을 찾는다
    - `{}`인 경우 -> server 생성하거나 location 함수를 호출한다
    - 아무것도 없는 경우 -> throw error

***

1. server를 기준으로 split한다.
2. split된 부분에서 괄호를 확인한다.
    - server 옆에 '{' 확인한다.
    - split된 부분의 마지막 부분이 '}'로 끝나는지 확인한다.
3. server 기준으로 split된 덩어리를 다시 location 기준으로 split한다.
    - location도 server와 같은 방식으로 '{}' 확인한다.
    - location에 해당하지 않는 부분
    - location에 해당하는 부분
4. directive의 파싱을 진행한다.
    a. trim (앞부분 tab 제거)
    b. tab 기준 split
    c. space 기준 split

ㄴ 3번을 먼저할지 4번을 먼저할지 -->
