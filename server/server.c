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
	int max_socket = serverSocket;

	printf("Waiting for connection...\n");

	userSet users;
	users.max_users = 100;
	users.users = (user*)(malloc(sizeof(users) * users.max_users));

	fd_set reads;
	int current_socket;
	while(1) {
		reads = master;
		if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
			fprintf(stderr, "select() failed. (%d)\n", errno);
			return 1;
		}

		for(current_socket = 1; current_socket <= max_socket; ++current_socket) {
			if (FD_ISSET(current_socket, &reads)) {
				if (current_socket == serverSocket) {
					struct sockaddr_storage client_address;
					socklen_t client_len = sizeof(client_address);
					int socket_client = accept(serverSocket, (struct sockaddr*) &client_address, &client_len);

					if (socket_client < 0) {
						PrintError("Adding socket_client",errno);
					}

					FD_SET(socket_client, &master);
					if (socket_client > max_socket) {
						max_socket = socket_client;
					}

					add_user(&users, current_socket, "test");
					char address_buffer[100];
					getnameinfo((struct sockaddr*)&client_address,
							client_len,
							address_buffer, sizeof(address_buffer), 0, 0,
							NI_NUMERICHOST);
					printf("New connection from %s\n", address_buffer);


				} else {
					char read[4028];
					int bytes_received = recv(current_socket, read, 4028, 0);

					if (bytes_received <= 0) {
						close(current_socket);
						FD_CLR(current_socket, &master);
					}

				}
			} //if FD_ISSET
		} //for i to max_socket
	} //while(1)
	free(users.users);
	return 0;
}

