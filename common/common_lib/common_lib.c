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
			case ERROR:
			case LIST:
				cJSON_AddStringToObject(data,"content", msg);
				break;
			case REGISTER:
				cJSON_AddStringToObject(data,"password", msg);
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

void UnpackJSON(cJSON* msg, cJSON** origin,cJSON** recipient, cJSON** action, cJSON** data, cJSON** len) {
	*origin = cJSON_GetObjectItem(msg, "origin");
	*recipient = cJSON_GetObjectItem(msg, "recipient");
	*action = cJSON_GetObjectItem(msg, "action");
	*data = cJSON_GetObjectItem(msg, "data");
	*len = cJSON_GetObjectItem(msg, "msg_len");
	return;
}
