#include "../common/common_lib.h"
#include "server_utils.h"


int main() {
	printf("Starting up Server\n");
	int serverSocket = InitializeServer();
	
	if (listen(serverSocket, 10) < 0) {
		fprintf(stderr, "listen() failed. (%d)\n", errno);
		return 1;
	}
	
	fd_set master;
	FD_ZERO(&master);
	FD_SET(serverSocket, &master);
	FD_SET(0, &master);
	int max_socket = serverSocket;

	printf("Waiting for connection...\n");

	userSet users;
	users.max_users = 100;
	users.users = (user*)(malloc(sizeof(users) * users.max_users));

	fd_set reads;
	int currentSocket;
	
	bool doRun = true;
	while(doRun) {
		reads = master;
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
		if (select(max_socket+1, &reads, 0, 0, &timeout) < 0) {
			fprintf(stderr, "select() failed. (%d)\n", errno);
			return 1;
		}

		for(currentSocket = 0; currentSocket <= max_socket + 1; ++currentSocket) {
			if (FD_ISSET(currentSocket, &reads)) {
				if (currentSocket == 0) {
					printf("detected\n");
					char msg[4096];
					if (!fgets(msg, 4096, stdin)) break;

					if(strcmp(msg,"q")) {
						doRun = false;
						break;
					}

				} else if (currentSocket == serverSocket) {
					struct sockaddr_storage client_address;
					socklen_t client_len = sizeof(client_address);
					int socket_client = accept(currentSocket, (struct sockaddr*) &client_address, &client_len);

					if (currentSocket < 0) {
						PrintError("Adding socket_client",errno);
					}

					FD_SET(socket_client, &master);

					if (socket_client> max_socket) {
						max_socket = socket_client;
					}

					AddUserByName(&users, socket_client, "test");

					char address_buffer[100];
					getnameinfo((struct sockaddr*)&client_address, client_len,
							address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);

					printf("New connection from %s\n", address_buffer);

				} else {
					printf("Message received\n");
					char read[4028];
					int bytes_received = recv(currentSocket, read, 4028, 0);

					if (bytes_received == 0) {
						printf("Someone has left\n");
						RemoveUserBySocket(currentSocket, &users);
						close(currentSocket);
						FD_CLR(currentSocket, &master);
					} else {
						printf("%.*s\n", bytes_received, read);
					}

				}
			} //if FD_ISSET
		} //for i to max_socket
	} //while(1)
	free(users.users);
	return 0;
}

