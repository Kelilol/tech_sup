#include "client.h"
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void select_mode(int sock, char *user_type) {
  if (strcmp(user_type, "engineer") == 0 || strcmp(user_type, "2") == 0) {
    engineer_mode(sock);
  } else if (strcmp(user_type, "support") == 0 || strcmp(user_type, "1") == 0) {
    support_mode(sock);
  } else {
    printf("Invalid user type\n");
    close(sock);
    exit(EXIT_FAILURE);
  }
}

void engineer_mode(int sock) {
  char message[BUFFER_SIZE];
  pthread_t recv_thread;

  send_msg(sock, "engineer", sizeof("engineer"), 0);

  pthread_create(&recv_thread, NULL, receive_messages, &sock);

  while (1) {
    printf("Enter your message: ");
    fflush(stdout);
    if (fgets(message, BUFFER_SIZE, stdin) != NULL) {
      message[strcspn(message, "\n")] = 0;
      if (strcmp(message, "exit") == 0) {
        send_msg(sock, "exit", sizeof("exit"), 0);
        break;
      }
      send_msg(sock, message, strlen(message), 0);
    }
  }

  pthread_cancel(recv_thread);
  pthread_join(recv_thread, NULL);
  close(sock);
  exit(EXIT_SUCCESS);
}

void support_mode(int sock) {
  char tp_type[TP_NAME_SIZE];
  int authorized = 0;
  while (!authorized) {
    printf("Enter your (sign up/sign in)\n");
    fflush(stdout);
    if (fgets(tp_type, TP_NAME_SIZE, stdin) != NULL) {
      tp_type[strcspn(tp_type, "\n")] = 0;
      if (strcmp(tp_type, "sign up") == 0) {
        int result_sign_up = sign_up(sock, tp_type);
        if (result_sign_up) {
          printf("A TP with this username has already been registered\n");
          continue;
        } else {
          printf("TP is registered\n");
          authorized = 1;
        }
      } else if (strcmp(tp_type, "sign in") == 0) {
        int result = sign_in(sock, tp_type);
        if (result) {
          printf("Authorization is successful\n");
          authorized = 1;
        } else {
          printf("TP not found, authorization failed\n");
        }
      } else {
        printf("Invalid input\n");
      }
    }
  }

  char message[BUFFER_SIZE];
  pthread_t recv_thread;

  pthread_create(&recv_thread, NULL, receive_messages, &sock);

  while (1) {
    printf("Enter your message: ");
    fflush(stdout);
    if (fgets(message, BUFFER_SIZE, stdin) != NULL) {
      message[strcspn(message, "\n")] = 0;
      if (strcmp(message, "exit") == 0) {
        send_msg(sock, "exit", sizeof("exit"), 0);
        break;
      }
      send_msg(sock, message, strlen(message), 0);
    }
  }

  pthread_cancel(recv_thread);
  pthread_join(recv_thread, NULL);
  close(sock);
  exit(EXIT_SUCCESS);
}

void *receive_messages(void *arg) {
  int sock = *(int *)arg;
  char buffer[BUFFER_SIZE];
  ssize_t bytes_received;

  while ((bytes_received = recv_msg(sock, buffer, sizeof(buffer), 0)) > 0) {
    buffer[bytes_received] = '\0';

    if (strncmp(buffer, "Support:", 8) == 0) {
      printf("\nSupport answer: %s\n", buffer + 8);
    } else if (strncmp(buffer, "Engineer:", 9) == 0) {
      printf("\nEngineer answer: %s\n", buffer + 9);
    } else {
      printf("\n%s\n", buffer);
    }
    printf("Enter your message: ");
    fflush(stdout);
  }

  if (bytes_received < 0) {
    printf("Server connection lost\n");
  }

  return NULL;
}

int sign_in(int socket, char *reg) {
  char buf[SIZE_LOGIN + SIZE_PASSWORD];
  char login[SIZE_LOGIN], password[SIZE_PASSWORD];
  int result_log = 0;

  send_msg(socket, reg, strlen(reg), 0);

  printf("Enter login: ");
  fflush(stdout);
  if (fgets(login, sizeof(login), stdin) != NULL) {
    login[strcspn(login, "\n")] = 0;

    printf("Enter password: ");
    fflush(stdout);
    if (fgets(password, sizeof(password), stdin) != NULL) {
      password[strcspn(password, "\n")] = 0;

      snprintf(buf, sizeof(buf), "%s %s", login, password);
      send_msg(socket, buf, strlen(buf), 0);

      memset(buf, 0, sizeof(buf));
      recv_msg(socket, buf, sizeof(buf), 0);

      buf[strcspn(buf, "\n")] = 0;
      if (strcmp(buf, "OK") == 0) {
        result_log = 1;
      } else if (strcmp(buf, "FAIL") == 0) {
        result_log = 0;
      }
    }
  }
  return result_log;
}

int sign_up(int socket, char *reg) {
  char buf[SIZE_LOGIN + SIZE_PASSWORD] = {0};
  char login[SIZE_LOGIN], password[SIZE_PASSWORD];
  int result_reg = 0;
  send_msg(socket, reg, strlen(reg), 0);

  printf("Enter login: ");
  fflush(stdout);
  if (fgets(login, sizeof(login), stdin) != NULL) {
    login[strcspn(login, "\n")] = 0;

    printf("Enter password: ");
    fflush(stdout);
    if (fgets(password, sizeof(password), stdin) != NULL) {
      password[strcspn(password, "\n")] = 0;

      snprintf(buf, sizeof(buf), "%s %s", login, password);
      printf("buffer sign up: %s\n", buf);
      send_msg(socket, buf, strlen(buf), 0);

      memset(buf, 0, sizeof(buf));
      recv_msg(socket, buf, sizeof(buf), 0);

      buf[strcspn(buf, "\n")] = 0;
      if (strcmp(buf, "OK") == 0) {
        result_reg = 0;
      } else if (strcmp(buf, "FAIL") == 0) {
        result_reg = 1;
      }
    }
  }
  return result_reg;
}

int create_socket(int domain, int type, int protocol) {
  int sockfd = socket(domain, type, protocol);
  if (sockfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

int connect_socket(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  if (connect(sockfd, addr, addrlen) == -1) {
    perror("connect");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  return 0;
}

int send_msg(int sockfd, const void *buf, size_t len, int flags) {
  ssize_t sent_bytes = send(sockfd, buf, len, flags);
  if (sent_bytes == -1) {
    perror("send");
  }
  return sent_bytes;
}

int recv_msg(int sockfd, void *buf, size_t len, int flags) {
  ssize_t recv_bytes = recv(sockfd, buf, len, flags);
  if (recv_bytes == -1) {
    perror("recv");
  }
  return recv_bytes;
}
