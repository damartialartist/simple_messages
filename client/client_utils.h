#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H
#include "../common/common_lib.h"
int ConnectToServer(char* SERVERIP, char* SERVERPORT);

void CreateMasterFdSet(fd_set* masterfd, int socket);
struct timeval CreateTimeOut(double seconds);
cJSON* GetMessageFromServer(int serverSocket);
int RegisterUser(int clientSocket, char* userName);
#endif
