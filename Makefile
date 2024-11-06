CC = gcc
CFLAGS = -Wall -Werror -Iinclude
CLIENT_TARGET = client_app
SERVER_TARGET = server_app

CLIENTD = client
SERVERD = server
COMMOND = common/common_lib

CLIENT_SRC = $(CLIENTD)/client.c $(CLIENTD)/client_utils.c $(COMMOND)/common_lib.c $(COMMOND)/cJSON.c
SERVER_SRC = $(SERVERD)/server.c $(SERVERD)/server_utils.c $(SERVERD)/server_requests.c $(COMMOND)/common_lib.c $(COMMOND)/cJSON.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJ)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJ)

# Rule to compile .o files from .c files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(CLIENT_OBJ) $(CLIENT_TARGET) $(SERVER_OBJ) $(SERVER_TARGET)

