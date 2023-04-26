#pragma once

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW "\033[0;33m"
#define BLU "\033[0;36m"
#define GRY "\033[90m"
#define DFT "\033[0;37m"

#define MAX_HEAD_SIZE 1024
#define MAX_BODY_SIZE 1024
#define BUFFER_SIZE 1024

#define CHUNK_SIZE 0
#define CHUNK_ENTITY 1

typedef enum step {
  START,
  REQUEST_HEAD,
  REQUEST_ENTITY,
  REQUEST_DONE,
  FILE_OPEN,
  FILE_DONE,
  RESPONSE_DONE
} t_step;

typedef enum method {
  GET = 1,
  POST = 1 << 1,
  DELETE = 1 << 2,
} t_method;