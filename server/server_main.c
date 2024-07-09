#include "server.h"

SupportAgent support_agents[MAX_CLIENTS];
Engineer engineers[MAX_CLIENTS];
int num_support_agents = 0;
int num_engineers = 0;

pthread_mutex_t support_agents_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_support(void *arg) {
  ChatSession *session = (ChatSession *)arg;
  char buffer[SIZE_BUF];
  ssize_t bytes_received;
  Message msg1;
  msg1.index = 0;

  while (1) {
    bytes_received =
        recv_msg(session->support.socket_fd, buffer, sizeof(buffer), 0);
      if (bytes_received == 0) {
        printf("Support disconnected\n");
        break;
      }
      if(strncmp(buffer, "exit", sizeof("exit")) == 0){
      pthread_mutex_lock(&support_agents_mutex);
      if (num_support_agents > 0 && session->engineer.socket_fd > 0) {
        session->support = support_agents[--num_support_agents];
        engineers[num_engineers++] = session->engineer;
        pthread_mutex_unlock(&support_agents_mutex);
        printf("Reassigning new support agent\n");
        continue;
      } else if (num_support_agents == 0 && session->engineer.socket_fd > 0){
          engineers[num_engineers++] = session->engineer;
          pthread_mutex_unlock(&support_agents_mutex);
          continue;
      } else {
        session->support = support_agents[--num_support_agents];
        pthread_mutex_unlock(&support_agents_mutex);
        printf("No available support agents, engineer will be disconnected\n");
        break;
      }
    } else if (bytes_received > 0) {
      msg1.time = time(NULL);
      msg1.string = buffer;
      pthread_mutex_lock(&session->file_mutex);
      move_request_file(session->engineer.id_request, "pending", "open");
      append_message_to_file(session->engineer.id_request, &msg1, "support");
      pthread_mutex_unlock(&session->file_mutex);

      char send_buffer[SIZE_BUF + 9]; // 9 is the length of "Support:"
      snprintf(send_buffer, sizeof(send_buffer), "Support:%s", buffer);
      send_msg(session->engineer.socket_fd, send_buffer, strlen(send_buffer),
               0);

      printf("Support sent: %s\n", buffer);
      memset(buffer, 0, sizeof(buffer));
    } else if (bytes_received == -1) {
      printf("Error receiving from support\n");
      break;
    }
  }

  close(session->support.socket_fd);
  session->support.socket_fd = 0;
  return NULL;
}

void *handle_engineer(void *arg) {
  ChatSession *session = (ChatSession *)arg;
  char buffer[SIZE_BUF];
  ssize_t bytes_received;
  Message msg1;
  msg1.index = 0;
  

  while (1) {
    bytes_received =
        recv_msg(session->engineer.socket_fd, buffer, sizeof(buffer), 0);
    if (bytes_received == 0) {
      printf("Engineer disconnected\n");
      break;

    } else if (bytes_received > 0) {
      if (strncmp(buffer, "exit", sizeof("exit")) == 0) {
        move_request_file(session->engineer.id_request, "open", "closed");
        pthread_mutex_lock(&support_agents_mutex);
        if(session->support.socket_fd > 0 && num_engineers > 0){
          session->engineer = engineers[--num_engineers];
          support_agents[num_support_agents++] = session->support;
          pthread_mutex_unlock(&support_agents_mutex);
          memset(buffer, 0, sizeof(buffer));
          continue;          
        }else if (session->support.socket_fd > 0 && num_engineers == 0){
          support_agents[num_support_agents++] = session->support;
          pthread_mutex_unlock(&support_agents_mutex);
          memset(buffer, 0, sizeof(buffer));
          continue;
        }else if (session->support.socket_fd == 0 && num_engineers > 0){
        pthread_mutex_unlock(&support_agents_mutex);
        session->engineer = engineers[--num_engineers];
        printf("Engineer disconnected\n");
        break;
        }
      } else {
        msg1.time = time(NULL);
        msg1.string = buffer;
        pthread_mutex_lock(&session->file_mutex);
        move_request_file(session->engineer.id_request, "pending", "open");
        append_message_to_file(session->engineer.id_request, &msg1, "engineer");
        pthread_mutex_unlock(&session->file_mutex);
        char send_buffer[SIZE_BUF + 10] = {0}; // 10 is the length of "Engineer:"
        snprintf(send_buffer, sizeof(send_buffer), "Engineer:%s", buffer);
        send_msg(session->support.socket_fd, send_buffer, strlen(send_buffer),
                 0);

        printf("Engineer sent: %s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
      }
    } else if (bytes_received == -1) {
      printf("Error receiving from engineer\n");
      break;
    }
  }

  close(session->engineer.socket_fd);
  session->engineer.socket_fd = 0;
  return NULL;
}

void handle_buf(int new_socketfd) {
repeat:
  char buffer[SIZE_BUF] = {0};
  ssize_t bytes_received = recv_msg(new_socketfd, buffer, sizeof(buffer), 0);
  printf("Check message: %s\n", buffer);
  buffer[strcspn(buffer, "\n")] = 0;
  if (strcmp(buffer, "sign in") == 0) {
    SupportAgent tp;
    tp.socket_fd = new_socketfd;
    support_agents[num_support_agents++] = tp;
    int result_log = log_func(new_socketfd, &tp);
    process_result_sign_in(new_socketfd, result_log);
  } else if (strcmp(buffer, "sign up") == 0) {
    SupportAgent tp;
    tp.socket_fd = new_socketfd;
    int result_reg = reg_func(new_socketfd, &tp);
    process_result_sign_up(new_socketfd, result_reg);
    if (result_reg) {
      goto repeat;
    }
    support_agents[num_support_agents++] = tp;
  } else if (strcmp(buffer, "engineer") == 0) {
Engineer engineer;
    engineer.socket_fd = new_socketfd;
    send_msg(engineer.socket_fd, "If you want open chat   ENTER: 1 \nIf you want create chat ENTER: 2", 
    sizeof("If you want open chat   ENTER: 1 \n If you want create chat ENTER: 2"),0);
    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv_msg(engineer.socket_fd, buffer, sizeof(buffer), 0);
    
     if (strcmp(buffer, "1") == 0) {
        const char directory[SIZE_DIR];
        const char filename_without_extension[SIZE_FILE_NAME];
        printf("check Buffer  '1'\n");
        memset(buffer, 0, sizeof(buffer));

        send_msg(engineer.socket_fd, "Dir ENTER:",sizeof("Dir ENTER:"), 0);
        bytes_received = recv_msg(engineer.socket_fd, buffer, sizeof(buffer), 0);
        strncpy((char *)directory, buffer, SIZE_DIR - 1);
        ((char *)directory)[SIZE_DIR - 1] = '\0'; 

        memset(buffer, 0, sizeof(buffer));

        send_msg(engineer.socket_fd, "Id chat ENTER:",sizeof("id chat ENTER:"), 0);
        bytes_received = recv_msg(engineer.socket_fd, buffer, sizeof(buffer), 0);
        strncpy((char *)filename_without_extension, buffer, SIZE_FILE_NAME - 1);
        ((char *)filename_without_extension)[SIZE_FILE_NAME - 1] = '\0'; 
        printf("Directory: %s\n", directory);
        printf("file id: %s\n", filename_without_extension);
        memset(buffer, 0, sizeof(buffer));

        if (find_txt_file(directory, filename_without_extension)!=0) {
          printf("found file: %s/%s.txt\n", directory, filename_without_extension);
          snprintf(engineer.namechat, sizeof(engineer.namechat), "%s/%s.txt",directory,filename_without_extension);
          engineer.id_request = atoi(filename_without_extension);
          engineers[num_engineers++] = engineer;
          send_msg(engineer.socket_fd, engineer.namechat, sizeof(SIZE_NAME_CHAT),0);
        } else {
          printf("file not found\n");
          goto repeat;
        }

  } else if (strcmp(buffer, "2") == 0) {
        int counter = read_config();
        if (counter == -1)
          return;
        counter++;
        update_config(counter);


        memset(buffer, 0, sizeof(buffer));
        send_msg(engineer.socket_fd, "enter title", sizeof("enter title"),0);
        bytes_received = recv_msg(engineer.socket_fd, buffer, sizeof(buffer), 0);
        Request request = {counter, time(NULL), 0, 1,buffer};
        create_request_file(&request, counter);
        snprintf(engineer.namechat, sizeof(engineer.namechat), "pending/%d.txt",counter);
        engineer.id_request = counter;
        printf("Check message id_request: %d\n", engineer.id_request);
        engineers[num_engineers++] = engineer;
        printf("Check message: %s\n", engineer.namechat);
        send_msg(engineer.socket_fd, engineer.namechat, sizeof(engineer.namechat),0);
    } else {
        send_msg(engineer.socket_fd,"Buffer содержит что-то другое ", sizeof("Buffer содержит что-то другое "),0);
        goto repeat;
    }
    
  }

  memset(buffer, 0, sizeof(buffer));

  if (num_support_agents > 0 && num_engineers > 0) {

    ChatSession *session = malloc(sizeof(ChatSession));
    initialize_chat_mutex_session(session);
    session->support = support_agents[--num_support_agents];
    session->engineer = engineers[--num_engineers];
    printf("Check message id_request: %d\n", session->engineer.id_request);
    printf("Check NAME: %s\n", session->engineer.namechat);

    pthread_t thread_id_support, thread_id_engineer;
    int result;

    result = pthread_create(&thread_id_support, NULL, handle_support, session);
    if (result != 0) {
      perror("pthread_create for support");
    }
    pthread_detach(thread_id_support);

    result =
        pthread_create(&thread_id_engineer, NULL, handle_engineer, session);

    if (result != 0) {
      perror("pthread_create for engineer");
    }
    pthread_detach(thread_id_engineer);
    free(session);
  }

  if (bytes_received < 0) {
    printf("Client disconnected\n");
  }
}

int main() {
  create_all_if_not_exists();
  int socketfd;
  struct sockaddr_in my_addr;

  memset(&my_addr, 0, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(PORT);
  my_addr.sin_addr.s_addr = INADDR_ANY;

  socketfd = create_socket(AF_INET, SOCK_STREAM, 0);

  bind_socket(socketfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
  listen_connection(socketfd, 5);

  printf("Server is listening on port %d\n", PORT);

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int new_socketfd =
        accept_socket(socketfd, (struct sockaddr *)&client_addr, &addrlen);
    if (new_socketfd == -1) {
      fprintf(stderr, "Accept failed\n");
      continue;
    }

    handle_buf(new_socketfd);
  }

  close(socketfd);
  return 0;
}
