#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common_lib/cJSON.h"
#define DEBUG_MODE false
void PrintError(const char* function, int errorCode);
void PrintMessage(const char* msg);

#endif
