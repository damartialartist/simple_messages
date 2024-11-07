#include "server_requests.h"
#include "../common/common_lib.h"

void ActionExit(int clientSocket, userSet *users, fd_set *master) {
  RemoveUserBySocket(clientSocket, users);
  close(clientSocket);
  FD_CLR(clientSocket, master);
  return;
}

int VerifyActionMessage(cJSON *recipient, userSet* users) {
  char* cRecipient = cJSON_GetStringValue(recipient);
  if (cRecipient == NULL || cRecipient[0]=='\0' ) {
    return -1;
  }
  int sock = GetSocketByUserName(cRecipient, users);
  return sock;
}

int ActionMessage(cJSON *origin, cJSON *recipient, cJSON *action, cJSON *data, userSet* users) {
  int recipientSocket = VerifyActionMessage(recipient,users);
  if (recipientSocket == -1) {
    return -1;
  }

  char* cUsername = cJSON_GetStringValue(origin);
	char* cRecipient = cJSON_GetStringValue(recipient);

  cJSON *content = cJSON_GetObjectItem(data, "content");
  char *cMsg = cJSON_GetStringValue(content);

  int bytesSent = -1;

  cJSON *newMsg = CreateMsgPacket(cUsername, cRecipient, MESSAGE, cMsg);
  char *cnewMsg = cJSON_PrintUnformatted(newMsg);
  bytesSent = send(recipientSocket, cnewMsg, strlen(cnewMsg), 0);
  free(cnewMsg);
  cJSON_Delete(newMsg);

  return bytesSent;
}
