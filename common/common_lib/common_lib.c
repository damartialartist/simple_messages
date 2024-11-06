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



cJSON* create_msg_packet(char* origin, char* recipient, JSON_ACTIONS action, char* msg) {
		cJSON* msg_packet = cJSON_CreateObject();
		cJSON_AddStringToObject(msg_packet, "origin", origin);
		cJSON_AddNumberToObject(msg_packet, "recipient", action);
		cJSON_AddStringToObject(msg_packet, "action", recipient);

		cJSON* data = cJSON_CreateObject();
		switch (action) {
			case MESSAGE:
				cJSON_AddNumberToObject(data,"type", action);
				cJSON_AddStringToObject(data,"content", msg);
				break;
			case REGISTER:
				break;
			case LOGIN:
				break;
			case SEND_FRIEND_REQUEST:
				break;
			case REQUEST_RESPONSE:
				break;
			default:
				printf("Unknown action\n");
				free(msg_packet);
				free(data);
				return NULL;
		}
		cJSON_AddStringToObject(msg_packet, "data", msg);
		cJSON_AddNumberToObject(msg_packet, "msg_length", strlen(msg)); 
		
		return msg_packet;
}
