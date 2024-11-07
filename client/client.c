#include "../common/common_lib.h"
#include "client_utils.h"

int RegisterUser(int serverSocket, char* userName) {
	cJSON* jsonMessage = CreateMsgPacket(userName, "server", REGISTER, userName);
	char* msg = cJSON_PrintUnformatted(jsonMessage);
	int bytes_sent = send(serverSocket, msg, strlen(msg),0);
	cJSON_Delete(jsonMessage);
	
	return bytes_sent;
}

int main() {
	fd_set readfd;
	char username[32];
	fd_set masterfd;
	int serverSocket;
	struct timeval timeout;
	double timeoutSeconds = 0.01;
	system("clear");

	printf("Enter desired username\n");
	scanf("%s", username);
	clearInputBuffer();


	serverSocket = ConnectToServer();

	if (RegisterUser(serverSocket, username) < 0) {
		PrintError("Register",errno);
	}
	CreateMasterFdSet(&masterfd, serverSocket);

	bool doRun = true;

	printf("Type /s <recipient> <message> to send message to someone (case sensitive)\n");

	while(doRun) {
		readfd = masterfd;
		timeout = CreateTimeOut(timeoutSeconds);

		if (select(serverSocket+1, &readfd, 0, 0, &timeout) < 0) {
			PrintError("select()", errno);
			return 1;
		}

		if (FD_ISSET(serverSocket, &readfd)) {
			char* msg = GetMessageFromServer(serverSocket);
			printf("Received msg from server: \"%s\"\n", msg);
			free(msg);
			continue;
		}

		if(FD_ISSET(0, &readfd)) {
			char msg[4096];
			if (!fgets(msg, 4096, stdin)) break;
			char cmd[10];
			char recipient[64];
			char message[4022];

			sscanf(msg, "%2s %31s %4020[^\n]", cmd, recipient, message);

			if (strcmp(cmd, "/q") == 0) {
				printf("Closing\n");
				cJSON* jsonMessage = CreateMsgPacket(username, "server", EXIT, "");
				char* messageToSend = cJSON_PrintUnformatted(jsonMessage);
				send(serverSocket, messageToSend, strlen(messageToSend),0);
				close(serverSocket);
				doRun = false;
				cJSON_Delete(jsonMessage);
				free(messageToSend);
			} else if (strcmp(cmd, "/s") == 0) {
				cJSON* jsonMessage = CreateMsgPacket(username, recipient, MESSAGE, message);
				char* messageToSend = cJSON_PrintUnformatted(jsonMessage);
				if (send(serverSocket,messageToSend,strlen(messageToSend),0) < -1) {
					PrintError("Send", errno);
				}
				free(messageToSend);
				cJSON_Delete(jsonMessage);
			} else if (strcmp(cmd, "/r") ==0) {
				cJSON* jsonMessage = CreateMsgPacket(username, "server", REGISTER, "");
				char* messageToSend = cJSON_PrintUnformatted(jsonMessage);
				if(send(serverSocket, messageToSend,strlen(messageToSend),0) < -1) {
					PrintError("Register",errno);
				}
				free(messageToSend);
				cJSON_Delete(jsonMessage);
			}
        }
    } //end while(1)
	return 0;

}

