#ifndef SERVER_REQUESTS_H
#define SERVER_REQUESTS_H
#include "../common/common_lib.h"
#include "server_utils.h"
void ActionExit(int clientSocket, userSet* users, fd_set* master);
int ActionMessage(cJSON *origin, cJSON *recipient, cJSON *action, cJSON *data, userSet* user);
int VerifyActionMessage(cJSON *recipient, userSet* users);
#endif
