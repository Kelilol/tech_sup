#include "server.h"

int create_socket(int domain, int type, int protocol) {
  int result = socket(domain, type, protocol);
  if (result < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  int opt = 1;
  if (setsockopt(result, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    perror("setsockopt");
    close(result);
    exit(EXIT_FAILURE);
  }
  printf("server socket created\n");
  return result;
}

int bind_socket(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  int result = bind(sockfd, addr, addrlen);
  if (result < 0) {
    perror("bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("bind successful\n");
  return result;
}

int listen_connection(int sockfd, int backlog) {
  int result = listen(sockfd, backlog);
  if (result < 0) {
    perror("listen failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("listening...\n");
  return result;
}

int accept_socket(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  int result = accept(sockfd, addr, addrlen);
  if (result < 0) {
    perror("accept failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  return result;
}

int recv_msg(int sockfd, void *buf, size_t len, int flags) {
  int result = recv(sockfd, buf, len, flags);
  if (result < 0) {
    perror("recieve failed");
  }
  return result;
}

int send_msg(int sockfd, const void *buf, size_t len, int flags) {
  int result = send(sockfd, buf, len, flags);
  if (result < 0) {
    perror("send failed");
  }
  return result;
}

void save_data_tp(SupportAgent *tp) {
  FILE *file = fopen(CREDENTIALS_FILE, "a+");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  fprintf(file, "%s %s\n", tp->login, tp->password);
  fclose(file);
}

int reg_func(int socketfd, SupportAgent *tp) {
  char buffer[SIZE_BUF] = {0};
  recv_msg(socketfd, buffer, sizeof(buffer), 0);

  sscanf(buffer, "%s %s", tp->login, tp->password);

  FILE *file = fopen(CREDENTIALS_FILE, "r");
  if (file == NULL) {
    save_data_tp(tp);
    return 0;
  }

  char line[SIZE_BUF];
  while (fgets(line, sizeof(line), file)) {
    char file_login[50], file_password[50];
    sscanf(line, "%s %s", file_login, file_password);

    if (strcmp(tp->login, file_login) == 0) {
      fclose(file);
      printf("a user with this username already exists\n");
      return 1;
    }
  }

  fclose(file);
  save_data_tp(tp);
  return 0;
}

int log_func(int socketfd, SupportAgent *tp) {
  char buf[100] = {0};
  recv_msg(socketfd, buf, sizeof(buf), 0);
  buf[strcspn(buf, "\n")] = 0;
  if (sscanf(buf, "%s %s", tp->login, tp->password) != 2) {
    perror("sscanf failed");
    return 0;
  }

  FILE *file = fopen(CREDENTIALS_FILE, "r");
  if (file == NULL) {
    perror("Error opening file");
    return 0;
  }

  char line[SIZE_BUF];
  while (fgets(line, sizeof(line), file)) {
    char file_login[SIZE_LOGIN];
    char file_password[SIZE_PASSWORD];
    sscanf(line, "%s %s", file_login, file_password);
    if (strcmp(tp->login, file_login) == 0 &&
        strcmp(tp->password, file_password) == 0) {
      fclose(file);
      return 1;
    }
  }
  fclose(file);
  return 0;
}

void process_result_sign_up(int socketfd, int result) {
  if (result) {
    send_msg(socketfd, "FAIL\n", sizeof("FAIL"), 0);
  } else {
    send_msg(socketfd, "OK\n", sizeof("OK"), 0);
  }
}

void process_result_sign_in(int socketfd, int result) {
  if (result) {
    send_msg(socketfd, "OK\n", 2, 0);
  } else {
    send_msg(socketfd, "FAIL\n", 4, 0);
  }
}

void initialize_chat_mutex_session(ChatSession *session) {
  pthread_mutex_init(&session->file_mutex, NULL);
}

void destroy_chat_session(ChatSession *session) {
  pthread_mutex_destroy(&session->file_mutex);
}