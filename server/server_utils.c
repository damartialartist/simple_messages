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
	if (getaddrinfo(0, "8765", &hints, &bindAddress) != 0) {
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

void AddUserByName(userSet* userSet, int fd, char* username) {
	if (userSet->num_users >= userSet->max_users) {
		user* newPtr = realloc(userSet->users, sizeof(user)* (userSet->max_users + 100));
		userSet->max_users = userSet->max_users + 100;
		if (newPtr != NULL) {
			userSet->users = newPtr;
		} else {
			PrintError("add_user",errno);
		}
	}
	++userSet->num_users;
	user* currentUser = &(userSet->users[userSet->num_users - 1]);
	strcpy(currentUser->userName, username);
	currentUser->fd = fd;
	return;
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
	
}

char* GetMessageFromClient(int clientSocket) {
	int msgBufferSize = 4096;
	char* msg = (char*) malloc(sizeof(char) * msgBufferSize);
	int bytes_received = recv(clientSocket, msg, 4096, 0);
	if (bytes_received < 1) {
		printf("Connection closed by peer.\n");
		free(msg);
		msg = NULL;
		return msg;	
	} 
	msg[bytes_received] = '\0';
	return msg;	
}

struct timeval CreateTimeOut(double seconds) {
		struct timeval timeout;
		timeout.tv_sec = (int) seconds;
        timeout.tv_usec = (int) ((int)seconds - seconds) * 1000000;
		return timeout;
}
