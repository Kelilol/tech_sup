// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>

// typedef struct Message {
//   int index;    // Номер сообщения
//   time_t time;  // Время сообщения
//   char *string; // Содержимое сообщения
// } Message;

// typedef struct {
//   int id;
//   char engeneer_name[50];
//   time_t time_create;
//   time_t time_end;
//   int active;
// } Request;

// int read_config() {
//   FILE *file = fopen("config.txt", "r");
//   if (!file) {
//     fprintf(stderr, "Не удалось открыть файл config.txt\n");
//     return -1;
//   }

//   int counter;
//   fscanf(file, "%d", &counter);
//   fclose(file);
//   return counter;
// }

// void update_config(int counter) {
//   FILE *file = fopen("config.txt", "w");
//   if (!file) {
//     fprintf(stderr, "Не удалось открыть файл config.txt\n");
//     return;
//   }

//   fprintf(file, "%d", counter);
//   fclose(file);
// }

// void create_request_file(Request *request, int counter) {
//   char filename[100];
//   snprintf(filename, sizeof(filename), "pending/%d.txt", counter);

//   FILE *file = fopen(filename, "w");
//   if (!file) {
//     fprintf(stderr, "Не удалось создать файл %s\n", filename);
//     return;
//   }

//   fprintf(file, "ID: %d\n", request->id);
//   fprintf(file, "Engineer name: %s\n", request->engeneer_name);
//   fprintf(file, "Time create: %ld\n", request->time_create);
//   fprintf(file, "Time end: %ld\n", request->time_end);
//   fprintf(file, "Active: %d\n", request->active);

//   fclose(file);
// }

// int get_last_message_index(const char *filename) {
//   FILE *file = fopen(filename, "r");
//   if (!file) {
//     fprintf(stderr, "Не удалось открыть файл %s\n", filename);
//     return -1;
//   }

//   char line[256];
//   int last_index = 0;

//   while (fgets(line, sizeof(line), file)) {
//     if (strncmp(line, "Message index:", 14) == 0) {
//       sscanf(line + 14, "%d", &last_index);
//     }
//   }

//   fclose(file);
//   return last_index;
// }

// void append_message_to_file(int request_id, Message *message) {
//   char filename[100];
//   snprintf(filename, sizeof(filename), "pending/%d.txt", request_id);

//   // Определение последнего индекса сообщения
//   int last_index = get_last_message_index(filename);
//   if (last_index == -1) {
//     fprintf(stderr, "Ошибка при чтении последнего индекса сообщения\n");
//     return;
//   }
//   message->index = last_index + 1;

//   FILE *file = fopen(filename, "a");
//   if (!file) {
//     fprintf(stderr, "Не удалось открыть файл %s\n", filename);
//     return;
//   }

//   fprintf(file, "Message index: %d\n", message->index);
//   fprintf(file, "Message time: %ld\n", message->time);

//   size_t len = strlen(message->string) + 1;
//   fprintf(file, "Message length: %zu\n", len);
//   fprintf(file, "Message content: %s\n", message->string);

//   fclose(file);
// }

// void move_request_file(int id, const char *from_dir, const char *to_dir) {
//   char old_path[100], new_path[100];
//   snprintf(old_path, sizeof(old_path), "%s/%d.txt", from_dir, id);
//   snprintf(new_path, sizeof(new_path), "%s/%d.txt", to_dir, id);

//   if (rename(old_path, new_path) != 0) {
//     return NULL;
//     // fprintf(stderr, "Не удалось переместить файл %s в %s\n", old_path,
//     //         new_path);
//   }
// }

// void print_request_file(int request_id) {
//   char filename[100];
//   snprintf(filename, sizeof(filename), "pending/%d.txt", request_id);

//   FILE *file = fopen(filename, "r");
//   if (!file) {
//     fprintf(stderr, "Не удалось открыть файл %s\n", filename);
//     return;
//   }

//   char line[256];
//   while (fgets(line, sizeof(line), file)) {
//     printf("%s", line);
//   }

//   fclose(file);
// }

// int main() {
//   int counter = read_config();
//   if (counter == -1)
//     return 1;
//   counter++;
//   update_config(counter);

//   // Создаем пример запроса
//   Request request = {counter, "Engineer1", time(NULL), 0, 1};

//   // Создаем файл для нового запроса
//   create_request_file(&request, counter);

//   // Создаем первое сообщение и добавляем его в файл
//   Message msg1 = {0, time(NULL),
//                   "first message"}; // Изначальный индекс не важен
//   append_message_to_file(request.id, &msg1);

//   // Создаем второе сообщение и добавляем его в файл
//   Message msg2 = {0, time(NULL),
//                   "second message"}; // Изначальный индекс не важен
//   append_message_to_file(request.id, &msg2);

//   // Печать содержимого файла запроса
//   print_request_file(request.id);

//   // Перемещение файла запроса в папку "open"
//   move_request_file(request.id, "pending", "open");

//   return 0;
// }
