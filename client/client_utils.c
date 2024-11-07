#include "../common/common_lib.h"
const char SERVERIP[128] = "127.0.0.1";
const char SERVERPORT[8] = "8085";

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
	freeaddrinfo(serverAddress);

	return serverSocket;
}

void CreateMasterFdSet(fd_set* masterfd, int socket) {
	FD_ZERO(masterfd);
	FD_SET(socket, masterfd);
	FD_SET(0, masterfd);
	return;
}

struct timeval CreateTimeOut(double seconds) {
		struct timeval timeout;
		timeout.tv_sec = (int) seconds;
        timeout.tv_usec = (int) ((int)seconds - seconds) * 1000000;
		return timeout;
}

cJSON* GetMessageFromServer(int serverSocket) {
	int msgBufferSize = 8192;
	char msg[msgBufferSize];
	int bytes_received = recv(serverSocket, msg, msgBufferSize, 0);
	msg[bytes_received] = '\0';
	if (bytes_received < 1) {
		printf("Connection closed by peer.\n");
		return NULL;	
	} 
	cJSON *jsonMessage = cJSON_Parse(msg);
	return jsonMessage;	
}

int RegisterUser(int serverSocket, char* userName) {
	cJSON* jsonMessage = CreateMsgPacket(userName, "server", REGISTER, userName);
	char* msg = cJSON_PrintUnformatted(jsonMessage);
	int bytes_sent = send(serverSocket, msg, strlen(msg),0);
	cJSON_Delete(jsonMessage);
	
	return bytes_sent;
}