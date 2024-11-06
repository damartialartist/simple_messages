#include "../common/common_lib.h"
#include "client_utils.h"

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
	int bytes_sent = send(serverSocket, username, strlen(username),0);
	if (bytes_sent < 1) {
		PrintError("send",errno);
	}

	CreateMasterFdSet(&masterfd, serverSocket);

	bool doRun = true;
	while(doRun) {
		readfd = masterfd;
		timeout = CreateTimeOut(timeoutSeconds);

		if (select(serverSocket+1, &readfd, 0, 0, &timeout) < 0) {
			PrintError("select()", errno);
			return 1;
		}

		if (FD_ISSET(serverSocket, &readfd)) {
			char* msg = GetMessageFromServer(serverSocket);
			printf("Received msg from server: \"%s\"", msg);
			free(msg);
		}

		if(FD_ISSET(0, &readfd)) {
			char msg[4096];
			if (!fgets(msg, 4096, stdin)) break;
			printf("MSG = %s", msg);
			if (strcmp(msg, "q\n") == 0) {
				printf("Closing\n");
				close(serverSocket);
				doRun = false;
				break;
			}
			int bytes_sent = send(serverSocket, msg, strlen(msg), 0);

			if (bytes_sent <= 1) {
				PrintError("Send failed", errno);
			}
        }
    } //end while(1)
	return 0;

}

