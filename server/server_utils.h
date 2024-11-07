#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H
#include "../common/common_lib.h"
typedef struct user {	
	char userName[32];
	int fd;
} user; 

typedef struct userSet {
	user* users;
	int num_users;
	int max_users;
} userSet;

int InitializeServer();
void CreateMasterFdSet(fd_set* masterfd, int socket);
int AddUserByName(userSet* userSet, int fd, char* username);
void RemoveUserBySocket(int socket, userSet* userSet);
cJSON* GetMessageFromClient(int clientSocket);
int GetSocketByUserName(char* userName, userSet* userSet);
char* GetUsernameBySocket(int socket, userSet* userSet);
struct timeval CreateTimeOut(double seconds);
int AcceptNewConnection(struct sockaddr_storage* clientAddress, int serverSocket);

#endif
