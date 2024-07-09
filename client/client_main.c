#include "client.h"

int main() {
  int sock;
  struct sockaddr_in server;
  char user_type[USER_TYPE_SIZE];

  sock = create_socket(AF_INET, SOCK_STREAM, 0);

  server.sin_addr.s_addr = inet_addr(SERVER);
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);

  connect_socket(sock, (struct sockaddr *)&server, sizeof(server));

  printf("Enter your type \n(1 - support)\n(2 - engineer)\n");
  fgets(user_type, USER_TYPE_SIZE, stdin);
  user_type[strcspn(user_type, "\n")] = 0;

  select_mode(sock, user_type);

  close(sock);
  return 0;
}
