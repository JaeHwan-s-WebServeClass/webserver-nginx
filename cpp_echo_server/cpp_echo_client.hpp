#pragma once

/* c header */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* cpp header */
#include <iostream>

/* define */
#define BUF_SIZE 1024

/* functions */
void error_handling(char *message);