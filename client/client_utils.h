#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H
#include "../common/common_lib.h"
int ConnectToServer();

void CreateMasterFdSet(fd_set* masterfd, int socket);
struct timeval CreateTimeOut(double seconds);
char* GetMessageFromServer(int serverSocket);
#endif
