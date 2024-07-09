CC = gcc

CFLAGS = -Wall -Wextra -g -Werror

FILE_TXT_FUNCTION_DIR = File_txt_Function
FILE_TXT_FUNCTION_SRC = $(FILE_TXT_FUNCTION_DIR)/File_request_function.c

CLIENT_DIR = client
CLIENT_SRC = $(CLIENT_DIR)/client.c $(CLIENT_DIR)/client_main.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
CLIENT_EXEC = client_app

SERVER_DIR = server
SERVER_SRC = $(SERVER_DIR)/server.c $(SERVER_DIR)/server_main.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)
SERVER_EXEC = server_app

all: $(CLIENT_EXEC) $(SERVER_EXEC)

$(CLIENT_EXEC): $(CLIENT_OBJ) $(FILE_TXT_FUNCTION_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(SERVER_EXEC): $(SERVER_OBJ) $(FILE_TXT_FUNCTION_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_DIR)/%.o: $(CLIENT_DIR)/%.c $(CLIENT_DIR)/client.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(SERVER_DIR)/%.o: $(SERVER_DIR)/%.c $(SERVER_DIR)/server.h
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_EXEC) $(SERVER_EXEC)

distclean: clean
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC)

.PHONY: all clean distclean
