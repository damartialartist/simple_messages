#include "../common/common_lib.h"
#include "server_utils.h"

int main() {
	int currentSocket;
	int serverSocket;
	printf("Starting up Server\n");
	serverSocket = InitializeServer();
	
	if (listen(serverSocket, 10) < 0) {
		fprintf(stderr, "listen() failed. (%d)\n", errno);
		return 1;
	}
	
	fd_set master;
	CreateMasterFdSet(&master, serverSocket);

	int max_socket = serverSocket;

	printf("Waiting for connection...\n");

	userSet users;
	memset(&users,0, sizeof(userSet));

	fd_set reads;	
	bool doRun = true;

	while(doRun) {
		reads = master;
        struct timeval timeout = CreateTimeOut(0.01);

		if (select(max_socket+1, &reads, 0, 0, &timeout) < 0) {
			fprintf(stderr, "select() failed. (%d)\n", errno);
			return 1;
		}

		for(currentSocket = 0; currentSocket < max_socket + 1; ++currentSocket) {
			if (FD_ISSET(currentSocket, &reads)) { 
				if (currentSocket == 0) { // SYSTEM INPUT DETECTED
					PrintMessage("Received message from console\n");
					char msg[4096];
					if (!fgets(msg, 4096, stdin)) {
						PrintMessage("Console command failed or is too long\n");
						break;
					};

					if(strcmp(msg,"q")) {
						doRun = false;
						break;
					}

				} else if (currentSocket == serverSocket) { // NEW USER CONNECTED 
					struct sockaddr_storage client_address;
					int socket_client = AcceptNewConnection(&client_address, currentSocket);

					if (currentSocket < 0) {
						PrintError("Adding socket_client",errno);
					}
					FD_SET(socket_client, &master);
					if (socket_client> max_socket) {
						max_socket = socket_client;
					}

					char* msg = GetMessageFromClient(socket_client);
					
					if( msg != NULL) {
						AddUserByName(&users, socket_client, msg);
					} else {
						AddUserByName(&users, socket_client, "Unknown");
					}

					char address_buffer[100];
					socklen_t client_len = sizeof(struct sockaddr_storage);

					getnameinfo((struct sockaddr*)&client_address, client_len,
							address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
					if (DEBUG_MODE) {
						printf("New connection from: %s\n", address_buffer);
						printf("Total number of users: %d\n", users.num_users);
						for (int i = 0; i < users.num_users; ++i) {
							printf("%s : ", (users.users[i]).userName);
							printf("%d\n", users.users[i].fd);
							printf("-------------------\n");
						}
					}

					free(msg);

				} else { // CONNECTED USER SENDS MESSAGE
					PrintMessage("Message from existing user\n");
					char* msg = GetMessageFromClient(currentSocket);

					if (msg == NULL) {
						PrintMessage("Someone has left\n");
						RemoveUserBySocket(currentSocket, &users);
						close(currentSocket);
						FD_CLR(currentSocket, &master);
					} else {
						printf("%s",msg);
					}

					free(msg);

				}
			} //if FD_ISSET
		} //for i to max_socket
	} //while(1)
	free(users.users);
	return 0;
}

