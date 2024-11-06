#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H
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
void add_user(userSet* userSet, int fd, char* username);
void removeUserBySocket(int socket, userSet* userSet);
#endif
