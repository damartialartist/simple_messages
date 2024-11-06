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

#define DEBUG_MODE true
typedef enum {
	MESSAGE = 1,
	REGISTER = 2,
	LOGIN = 3,
	SEND_FRIEND_REQUEST = 4,
	REQUEST_RESPONSE = 5
} JSON_ACTIONS;
void PrintError(const char* function, int errorCode);
void PrintMessage(const char* msg);
cJSON* create_msg_packet(char* origin, char* recipient, JSON_ACTIONS action, char* msg);
void clearInputBuffer();
#endif
