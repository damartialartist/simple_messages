#include "../common/common_lib.h"
#include "client_utils.h"


int main() {
	char SERVERIP[128] = "127.0.0.1";
	char SERVERPORT[8] = "8085";

	fd_set readfd;
	char username[32];
	fd_set masterfd;
	int serverSocket;
	struct timeval timeout;
	double timeoutSeconds = 0.01;

	system("clear");
	printf("Enter provided ip-addr, this will change every time\n");
	scanf("%s", SERVERIP);
	printf("Enter provided port addr\n");
	scanf("%s", SERVERPORT);

	printf("Enter desired username\n");
	scanf("%s", username);
	clearInputBuffer();

	serverSocket = ConnectToServer(SERVERIP,SERVERPORT);

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
			cJSON* jsonMessage = GetMessageFromServer(serverSocket);
			if (jsonMessage == NULL) {
				cJSON_Delete(jsonMessage);
				doRun = false;
				continue;
			}
			cJSON* origin = NULL, *recipient=NULL, *action=NULL, *data=NULL, *msg_len = NULL;
			UnpackJSON(jsonMessage, &origin, &recipient, &action, &data, &msg_len);
			JSON_ACTIONS cAction = action->valueint;

			if (cAction == MESSAGE) {
				char* from = cJSON_GetStringValue(origin);
				cJSON* content = cJSON_GetObjectItem(data, "content");
				if (content != NULL) {
					char* cMsg = cJSON_Print(content);
					printf("%s: %s\n",from,cMsg);
					free(cMsg);
				}
			} else if (cAction == ERROR) {
				char* from = cJSON_GetStringValue(origin);
				cJSON* content = cJSON_GetObjectItem(data, "content");
				if (content != NULL) {
					char* cMsg = cJSON_Print(content);
					printf("ERROR: From: %s: %s\n",from,cMsg);
					free(cMsg);
				}
			} else {
				printf("Unknown action or is not implemented yet\n");
			}
			cJSON_Delete(jsonMessage);
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
			} else {
				printf("Unknown Command / wrong formatting\n");
			}
        }
    } //end while(1)
	return 0;

}

