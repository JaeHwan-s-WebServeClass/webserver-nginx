#pragma once

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW "\033[0;33m"
#define BLU "\033[0;36m"
#define GRY "\033[90m"
#define DFT "\033[0;37m"

#define MAX_EVENT_SIZE 8

#define MAX_HEAD_SIZE 9999
#define MAX_BODY_SIZE 9999
#define BUFFER_SIZE 9999

#define F_STREAM_SIZE 294

#define CHUNK_SIZE 0
#define CHUNK_ENTITY 1

#define BACK_LOG 5

typedef enum step {
  START,           // Transaction 생성자
  REQUEST_HEAD,    // T.cpp executeReadHead line 155
  REQUEST_ENTITY,  // Request.cpp line 75(length), 96(chunked)
  REQUEST_DONE,    // T.cpp executeRead line 131
  FILE_READ,       // T.cpp checkResource line 76
  FILE_CGI,
  FILE_WRITE,
  FILE_DONE,      // T.cpp httpGet line 290
  RESPONSE_DONE,  // Response.cpp setResponseMsg line 82
  END
} t_step;

typedef enum method {
  GET = 1,
  POST = 1 << 1,
  DELETE = 1 << 2,
} t_method;