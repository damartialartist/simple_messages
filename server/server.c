#include "../common/common_lib.h"
#include "server_utils.h"
#include "server_requests.h"

int main() {
	system("clear");
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
	users.users = (user*) malloc(sizeof(user) * 100);
	users.num_users = 0;
	users.max_users = 100;

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
				} else { // CONNECTED USER SENDS MESSAGE 
					PrintMessage("Message from existing user\n");
					cJSON* msg = GetMessageFromClient(currentSocket);
					if (msg == NULL) {
						RemoveUserBySocket(currentSocket,&users);
						close(currentSocket);
						FD_CLR(currentSocket, &master);	
						continue;
					}

					char* cMessage = cJSON_Print(msg);
					printf("%s", cMessage);
					free(cMessage);
			
					cJSON* origin = NULL, *recipient=NULL, *action=NULL, *data=NULL, *msg_len = NULL;
					UnpackJSON(msg, &origin, &recipient, &action, &data, &msg_len);
					JSON_ACTIONS cAction = action->valueint; 
					char* cUsername = cJSON_GetStringValue(origin);
					
					if (cAction == EXIT){
						ActionExit(currentSocket,&users,&master);
						printf("%s: has left the server\n",cUsername);
					} else if (cAction == REGISTER) {
						AddUserByName(&users, currentSocket, cUsername);
						printf("%s: has joined the server\n", cUsername);
					} else if (cAction == MESSAGE) {
						int code = ActionMessage(origin,recipient,action,data, &users);
						if (code < 1) {
							PrintMessage("Message send Error");
							cJSON* clientErr = CreateMsgPacket("server",cUsername, ERROR, "Invalid Message sent, check recipient name. \n");
							char* cClientErr = cJSON_PrintUnformatted(clientErr);
							if (send(currentSocket,cClientErr,strlen(cClientErr),0) < 0) {
								PrintMessage("ERROR: Sending error to client with invalid responses\n");
							}
							free(cClientErr);
							cJSON_Delete(clientErr);
						}
					}

					if (DEBUG_MODE) {
						printf("Total number of users: %d\n", users.num_users);
						for (int i = 0; i < users.num_users; ++i) {
							printf("%s : ", (users.users[i]).userName);
							printf("%d\n", users.users[i].fd);
							printf("-------------------\n");
						}
					}
					cJSON_Delete(msg);
				}
			} //if FD_ISSET
		} //for i to max_socket
	} //while(1)
	close(serverSocket);
	free(users.users);
	return 0;
}

