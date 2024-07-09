#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../File_txt_Function/File_request_function.h"

#define CREDENTIALS_FILE "support_credentials.txt"
#define PORT 55555
#define SIZE_BUF 1024
#define MAX_CLIENTS 10
#define SIZE_LOGIN 50
#define SIZE_PASSWORD 50

typedef struct {
  int id;
  char login[SIZE_LOGIN];
  char password[SIZE_PASSWORD];
  int socket_fd;
  int available;
  time_t last_activity;
} SupportAgent;

typedef struct {
  int socket_fd;
  int id;
  char namechat[SIZE_NAME_CHAT];
  int id_request;
} Engineer;

typedef struct {
  SupportAgent support;
  Engineer engineer;
  pthread_mutex_t file_mutex;
} ChatSession;

int reg_func(int socketfd, SupportAgent *tp);
int log_func(int socketfd, SupportAgent *tp);
void save_data_tp(SupportAgent *tp);
void process_result_sign_in(int socketfd, int result_log);
void process_result_sign_up(int socketfd, int result_reg);
int create_socket(int domain, int type, int protocol);
int bind_socket(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen_connection(int sockfd, int backlog);
int accept_socket(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int recv_msg(int sockfd, void *buf, size_t len, int flags);
int send_msg(int sockfd, const void *buf, size_t len, int flags);
void initialize_chat_mutex_session(ChatSession *session);
void destroy_chat_session(ChatSession *session);

#endif
