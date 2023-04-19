#include <iostream>
#include <string>
#include <vector>
#include <iterator>

// using namespace std;
// #define CHUNK_SIZE false
// #define CHUNK_ENTITY true

int chunk_size = 0;
std::string hex_str = "";
std::vector<char> tmp_entity;
bool CR = false;
bool LF = false;
bool CRLF = false;

int hexToInt(const std::string& hex_str) {
  unsigned int dec = 0;

  for (std::string::const_iterator it = hex_str.begin(); it != hex_str.end();
       ++it) {
    if ((dec & 0xF0000000) != 0) throw std::string("overflow");

    char c = *it;

    if (c >= '0' && c <= '9') {
      dec = (dec << 4) + (c - '0');
    } else if (c >= 'a' && c <= 'f') {
      dec = (dec << 4) + (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
      dec = (dec << 4) + (c - 'A' + 10);
    } else {
      throw std::string("Invalid character in hex string");
    }
  }
  return dec;
}

/*
	1. 정상적으로 다 받고 chunk-content에서 buf가 끝난 경우 (일단 문제 없는걸로)
	2. chunk-size를 받는 도중에 buf가 끝난 경우
		 ex) 3A\r\n
		 2-1. 3까지 읽었는데 buf가 끝난 경우
		 2-2. 3A까지 읽었는데 buf가 끝난 경우
		 2-3. \r까지 읽었는데 buf가 끝난 경우
	3. chunk를 받는 중 끝난 경우 (chunk_size 로 체크)
*/

void addChunkedEntity(char * buf, int read_len) {
// read buf ->(parsing).\r\n -> request.entity(vec)
  int i = 0;

	while (i < read_len) {
		if (chunk_size == 0) {
			// size 읽어야 됨
			while (buf[i]) {
				// if (buf[i] != '\n')
					// throw std::string("error");
				//if (buf[i] == '\r') {
			    //    CR = true;
				//}

				// case 1. CR 이 false인 경우
				//	case 1-1 buf[i] == CR 인가? => CR = true; i++ continue;
				//	case 1-2 buf[i] != CR => error
				// case 2. CR 이 true 인데 LF 가 false인 경우
				//	case 2-1. buf[i] == LF => LF = true; i++ chunk size 할당 continue;
				//	case 2-2 buf[i] != LF => error
				// 나머지 경우 buf[i] 가 end 인지 검사 

			while (buf[i] ) {
				if (CR && i == 0)
				{
					if (buf[0] == '\n') 
					{
						LF == true;
						break;
					}
					else
					{
						throw error;
					}
				}
				if (!CR)
				{
					if (buf[i] == '\r')
					{
						CR = true;
						if (i + 1 < read_len)
						{
							if (buf[i+1] == '\n')
							{
								LF = true;
								CR = false;
								i++;
								break;
							}
							else 
							{
								throw error;
							}
						} 
						else
						{
							break;
						}
					}
					// r도 아니고 숫자도 아닐때는 처리해야되긴함
					else
					{
						hex_str += buf[i];
					}
				}
				if (LF == true)
				{
					chunk_size = hexToInt(hex_str);
					LF = false;
				}
				i++;
			}


				if (buf[i] == '\r') {
					CR = true;
				} 
				//case 1. crlf가 안끊긴 경우
				if (CR && i+1 < read_len) {
					if (buf[i+1]== '\n') {
						LF = true;
						CR = false;
						i++;
						break;
					} else  {
						throw std::string("error LF");
					}
				} else if (CR && i == 0) {
					
				}

				// case 1. 4/r/n이 들어온 경우
                // buf[i] 가 \r, buf[i] 다음이 \n 이면
				if (buf[i] == '\r' && (CR = true) && (++i < read_len) && buf[i] == '\n') {
					LF = true;
					CR = false;
					break;
				} 
				//case 2. 앞전에 /r로 끝났고, 이제 무조건 /n이 나와야 하는 차례
				if (CR && !LF && buf[i] == '\n') {
					LF = true;
					break;
				} else {
					throw std::string("error LF");
				}
				// chunk_size == 0이고 /r or /n 도 아닌 상태
				hex_str += buf[i];
				i++;
			}
			if (LF == true){
				chunk_size = hexToInt(hex_str);
				LF = false;
			}
			// /r/n 면 그만 읽고 hexToInt를 호출
			// 0이면 끝
			i++;
		} else {
			// entity 읽음
			// 읽으면서 --chunk_size
			// chunk_size == 0 일때 뒤에 /r/n 나오는지 확인
		}
	}




  while(i < read_len) {
    if (mode == CHUNK_SIZE) {
      // buf를 /r/n 을 딜리미터로 파싱해서, hex_str 을 구해야함   
      if (buf[i] != '\r' && buf[i + 1] && buf[i + 1] != '\n') {
        hex_str += buf[i];
      } else if (buf[i] == '\r' && buf[i + 1] && buf[i + 1] == '\n') {
        chunk_size = hexToInt(hex_str); // FIXME overflow 문제 처리
        mode = CHUNK_ENTITY;
      }
    } else {
			while (buf[i] && chunk_size--) {
				tmp_entity.push_back(buf[i]);
				i++;
			}
			if (chunk_size == 0) {
				mode = CHUNK_SIZE;
			}
      // if (buf[i] != '\r' && buf[i + 1] && buf[i + 1] != '\n') {
				// tmp_entity.push_back(buf[i]);
			// } else if (buf[i] == '\r' && buf[i + 1] && buf[i + 1] == '\n') {
				// mode = CHUNK_SIZE; 
				// chunk_size = 0;
			// }
    }
    ++i;
  }
  std::cout << "--------------------------------------------" << '\n' << '\n';
  for (std::vector<char>::iterator it = tmp_entity.begin(); it != tmp_entity.end(); it++)
  {
    std::cout << "chunk_size: " << chunk_size << std::endl;
    std::cout << "hex_str: " << hex_str << std::endl;
    std::cout << "*it : " << *it << std::endl;
  }
  // 청크사이즈만큼 다 읽은 후 바로 다음이 0\r\n 일 때 끝
}
/*
Encoded data :
4\r\n        (bytes to send)
Wiki\r\n     (data)
6\r\n        (bytes to send)
pedia \r\n   (data)
E\r\n        (bytes to send)
in \r\n
\r\n
chunks.\r\n  (data)
0\r\n        (final byte - 0)
\r\n         (end message)

Decoded data :
Wikipedia in 

chunks.
*/

int main() 
{
  char buf[1024] =
      "4\r\nWiki\r\n6\r\npedia \r\nE\r\nin \r\n\r\nchunks.\r\n 0\r\n\r\n";
	int i = 0;
	while (i < 5)
	{
		addChunkedEntity(buf + (7*i), 7);
		i++;
	}
}
