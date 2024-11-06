#include "../common/common_lib.h"
const char SERVERIP[16] = "127.0.0.1";
const char SERVERPORT[8] = "8765";

int ConnectToServer() {
	PrintMessage("In ConnectToServer");

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *serverAddress;

	if (getaddrinfo(SERVERIP, SERVERPORT, &hints, &serverAddress)) {
		PrintError("getadrrinfo",errno);	
	}

	if (DEBUG_MODE) {
		char addressBuf[1024];
		char serviceBuf[1024];
		
		getnameinfo(serverAddress->ai_addr, serverAddress->ai_addrlen, addressBuf, sizeof(addressBuf),
					serviceBuf, sizeof(serviceBuf), NI_NUMERICHOST);
		printf("Server address: %s ; Service: %s\n", addressBuf, serviceBuf);
	}

	int serverSocket = socket(serverAddress->ai_family, serverAddress->ai_socktype, serverAddress->ai_protocol);

	if (serverSocket == -1) {
		PrintError("socket", errno);
	}

	printf("Connecting . . . \n");

	if(connect(serverSocket, serverAddress->ai_addr, serverAddress->ai_addrlen)) {
		PrintError("connect", errno);
	}

	printf("Connection successful.\n");
	free(serverAddress);

	return serverSocket;
}
