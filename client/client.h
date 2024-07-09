#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER "127.0.0.1"
#define PORT 55555
#define BUFFER_SIZE 256
#define USER_TYPE_SIZE 10
#define TP_NAME_SIZE 50
#define SIZE_LOGIN 50
#define SIZE_PASSWORD 50

void engineer_mode(int sock);
void support_mode(int sock);
int connect_socket(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int create_socket(int domain, int type, int protocol);
int recv_msg(int sockfd, void *buf, size_t len, int flags);
int send_msg(int sockfd, const void *buf, size_t len, int flags);
void select_mode(int sock, char *user_type);
void *receive_messages(void *arg);
int sign_in(int socket, char *reg);
int sign_up(int socket, char *reg);

#endif
