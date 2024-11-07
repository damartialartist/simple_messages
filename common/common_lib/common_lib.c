#include "../common_lib.h"
#include "cJSON.h"

void PrintError(const char* function, int errorCode) {
	if (DEBUG_MODE) {
		fprintf(stderr, "%s() failed (%d)\n", function, errorCode);
	}
	exit(errorCode);
}

void PrintMessage(const char* msg) {
	if (DEBUG_MODE) {
		printf("%s\n", msg);
	}
	return;	
}

void clearInputBuffer() {
	while ((getchar()) != '\n');
}

cJSON* CreateMsgPacket(char* origin, char* recipient, JSON_ACTIONS action, char* msg) {
		cJSON* msg_packet = cJSON_CreateObject();
		cJSON_AddStringToObject(msg_packet, "origin", origin);
		cJSON_AddStringToObject(msg_packet, "recipient", recipient);
		cJSON_AddNumberToObject(msg_packet, "action", action);

		cJSON* data = cJSON_CreateObject();
		switch (action) {
			case EXIT:
				break;
			case MESSAGE:
				cJSON_AddStringToObject(data,"content", msg);
				break;
			case REGISTER:
			case LOGIN:
				cJSON_AddStringToObject(data,"password", msg);
				break;
			case SEND_FRIEND_REQUEST:
				cJSON_AddStringToObject(data,"user", msg);
				break;
			case REQUEST_RESPONSE:
				cJSON_AddStringToObject(data,"response", msg);
				break;
			default:
				printf("Unknown action\n");
				free(msg_packet);
				free(data);
				return NULL;
		}
		cJSON_AddItemToObject(msg_packet,"data",data);
		cJSON_AddNumberToObject(msg_packet, "msg_len", strlen(msg));
		return msg_packet;
}
