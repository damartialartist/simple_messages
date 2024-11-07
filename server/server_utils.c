#include "../common/common_lib.h"
#include "server_utils.h"

int InitializeServer() {
	PrintMessage("Initializing Server");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *bindAddress;
	if (getaddrinfo(0, "8085", &hints, &bindAddress) != 0) {
		PrintError("getaddrinfo", errno);
	}

	PrintMessage("Creating Socket");
	int serverSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype,
								bindAddress->ai_protocol);

	if (serverSocket == -1) {
		PrintError("socket", errno);
	}

	PrintMessage("Binding socket");
	if (bind(serverSocket, bindAddress->ai_addr, bindAddress->ai_addrlen)) {
		PrintError("bind", errno);
	}

	freeaddrinfo(bindAddress);
	return serverSocket;
}

int AcceptNewConnection(struct sockaddr_storage* clientAddress, int serverSocket) {
	socklen_t client_len = sizeof(struct sockaddr_storage);
	int socket_client = accept(serverSocket, (struct sockaddr*) clientAddress, &client_len);
	return socket_client;
}

void CreateMasterFdSet(fd_set* masterfd, int socket) {
	FD_ZERO(masterfd);
	FD_SET(socket, masterfd);
	FD_SET(0, masterfd);
	return;
}

int AddUserByName(userSet* userSet, int fd, char* username) {
	user* userIterator;
	
	if (userSet->num_users >= userSet->max_users) {
		user* newPtr = realloc(userSet->users, sizeof(user) * (userSet->max_users + 100));
		if (newPtr != NULL) {
			userSet->users = newPtr;
			userSet->max_users = userSet->max_users + 100;
		} else {
			PrintError("add_user",errno);
		}
	}
	
	for (int i = 0; i < userSet->num_users; ++i) {
		userIterator = &(userSet->users[i]);
		if (strcmp(userIterator->userName, username) == 0) {
			return -1;
		}
	}

	user* currentUser = &(userSet->users[userSet->num_users]);
	strncpy(currentUser->userName, username,32);
	currentUser->fd = fd;
	userSet->num_users = userSet->num_users + 1;
	return 0;
}

void RemoveUserBySocket(int socket, userSet* userSet) {
	user* currentUser;
	for (int i = 0; i < userSet->num_users; ++i) {
		currentUser = &(userSet->users[i]);
		if (currentUser->fd == socket) {
			for(int j = i; j < (userSet->num_users - 1); ++j) {
				strcpy(userSet->users[j].userName, userSet->users[j+1].userName);
				userSet->users[j].fd = userSet->users[j+1].fd;
			}
		}
	}
	--userSet->num_users;
	
}

cJSON* GetMessageFromClient(int clientSocket) {
	int msgBufferSize = 8192;
	char msg[msgBufferSize];
	int bytes_received = recv(clientSocket, msg, msgBufferSize, 0);
	msg[bytes_received] = '\0';
	if (bytes_received < 1) {
		printf("Connection closed by peer.\n");
		return NULL;	
	} 
	cJSON *jsonMessage = cJSON_Parse(msg);
	return jsonMessage;	
}

struct timeval CreateTimeOut(double seconds) {
		struct timeval timeout;
		timeout.tv_sec = (int) seconds;
        timeout.tv_usec = (int) ((int)seconds - seconds) * 1000000;
		return timeout;
}

int GetSocketByUserName(char* userName, userSet* userSet) {
	user* currentUser = NULL;
	for (int i = 0; i < userSet->num_users; ++i) {
		currentUser = &(userSet->users[i]);
		if (strcmp(userName,currentUser->userName) == 0) {
			return currentUser->fd;
		}
	}
	return -1;	
}

char* GetUsernameBySocket(int socket, userSet* userSet){
	user* currentUser = NULL;
	for (int i = 0; i < userSet->num_users; ++i) {
		currentUser = &(userSet->users[i]);
		if (currentUser->fd == socket) {
			return currentUser->userName;
		}
	}
	return NULL;	
}


