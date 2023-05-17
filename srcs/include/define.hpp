#pragma once

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW "\033[0;33m"
#define BLU "\033[0;36m"
#define GRY "\033[90m"
#define DFT "\033[0;37m"

#define MAX_EVENT_SIZE 1024

#define BUFFER_SIZE 9999

#define F_STREAM_SIZE 294

#define CHUNK_SIZE 0
#define CHUNK_ENTITY 1

#define BACK_LOG 100

#define EMPTY_FILE -1
#define DIRECTORY -1
#define NONE_FD -2

#define DEFALUT_CGIPID -2

#define HEALTHY 0
#define UNHEALTHY 1

#define TIMEOUT 5

typedef enum step {
  START,
  REQUEST_HEAD,
  REQUEST_ENTITY,
  REQUEST_DONE,
  FILE_READ,
  FILE_CGI,
  FILE_WRITE,
  FILE_DONE,
  RESPONSE_DONE,
  END
} t_step;

typedef enum method {
  GET = 1,
  POST = 1 << 1,
  DELETE = 1 << 2,
  PUT = 1 << 3
} t_method;