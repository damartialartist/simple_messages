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
