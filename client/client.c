#include "../common/common_lib.h"
#include "client_utils.h"

int main() {
	int serverSocket = ConnectToServer();
	
	fd_set masterfd;
	FD_ZERO(&masterfd);
	FD_SET(serverSocket, &masterfd);
	FD_SET(0, &masterfd);
	fd_set readfd;
	bool doRun = true;

	 while(doRun) {
        readfd = masterfd;

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        if (select(serverSocket+1, &readfd, 0, 0, &timeout) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", errno);
            return 1;
        }

        if (FD_ISSET(serverSocket, &readfd)) {
			char read[4096];
            int bytes_received = recv(serverSocket, read, 4096, 0);
            if (bytes_received < 1) {
                printf("Connection closed by peer.\n");
                break;
            }
            printf("Received (%d bytes): %.*s\n",
                    bytes_received, bytes_received, read);
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

