#include "File_request_function.h"

int read_config() {
  FILE *file = fopen("config.txt", "r");
  if (!file) {
    fprintf(stderr, "don`t opened file config.txt\n");
    return -1;
  }

  int counter;
  fscanf(file, "%d", &counter);
  fclose(file);
  return counter;
}

void update_config(int counter) {
  FILE *file = fopen("config.txt", "w");
  if (!file) {
    fprintf(stderr, "don`t opened file  config.txt\n");
    return;
  }

  fprintf(file, "%d", counter);
  fclose(file);
}

void create_request_file(Request *request, int counter) {
  char filename[SIZE_FILENAME];
  snprintf(filename, sizeof(filename), "pending/%d.txt", counter);
  printf("Check message: %s\n", filename);
  FILE *file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "don`t created file %s\n", filename);
    return;
  }

  fprintf(file, "ID: %d\n", request->id);
  fprintf(file,"Title %s\n",request->title);
  struct tm *now = localtime(&request->time_create);
  fprintf(file, "Time create:    %d.%d.%d %d:%d:%d\n", (now->tm_year + 1900),
          now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
  printf("Time create:    %d.%d.%d %d:%d:%d\n", (now->tm_year + 1900),
         now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
  fclose(file);
}

int get_last_message_index(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "don`t opened file %s\n", filename);
    return -1;
  }

  char line[SIZE_LINE];
  int last_index = 0;

  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "index:", 6) == 0) {
      sscanf(line + 6, "%d", &last_index);
    }
  }

  fclose(file);
  return last_index;
}

void append_message_to_file(int request_id, Message *message,
                            char *nameclient) {
  char filename[SIZE_FILENAME];
  snprintf(filename, sizeof(filename), "open/%d.txt", request_id);

  int last_index = get_last_message_index(filename);
  if (last_index == -1) {
    fprintf(stderr, "error read last Message`s index\n");
    return;
  }
  message->index = last_index + 1;

  FILE *file = fopen(filename, "a");
  if (!file) {
    fprintf(stderr, "don`t opened file %s\n", filename);
    return;
  }

  fprintf(file, "index: %d\t", message->index);

  struct tm *now = localtime(&message->time);
  fprintf(file, "time: %d.%d %d:%d:%d\t", now->tm_mon, now->tm_mday,
          now->tm_hour, now->tm_min, now->tm_sec);
  fprintf(file, "content %s: %s\n", nameclient, message->string);

  fclose(file);
}

void move_request_file(int id, const char *from_dir, const char *to_dir) {
  char old_path[SIZE_FILENAME], new_path[SIZE_FILENAME];
  snprintf(old_path, sizeof(old_path), "%s/%d.txt", from_dir, id);
  snprintf(new_path, sizeof(new_path), "%s/%d.txt", to_dir, id);

  if (rename(old_path, new_path) != 0) {
    return;
  }
}

void print_request_file(int request_id) {
  char filename[SIZE_FILENAME];
  snprintf(filename, sizeof(filename), "pending/%d.txt", request_id);

  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "don`t opened file %s\n", filename);
    return;
  }

  char line[SIZE_LINE];
  while (fgets(line, sizeof(line), file)) {
    printf("%s", line);
  }

  fclose(file);
}

void create_all_if_not_exists() {
  create_directory_if_not_exists("pending");
  create_directory_if_not_exists("open");
  create_directory_if_not_exists("closed");
  create_file_if_not_exists("config.txt");
}

void create_directory_if_not_exists(const char *dir_name) {
  struct stat st = {0};
  if (stat(dir_name, &st) == -1) {
    char command[SIZE_LINE];
    snprintf(command, sizeof(command), "mkdir -p %s", dir_name);
    system(command);
  }
}

void create_file_if_not_exists(const char *file_name) {
  if (access(file_name, F_OK) == -1) {
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
      perror("Error creating file");
      exit(EXIT_FAILURE);
    }
    fprintf(file, "0\n");
    fclose(file);
  }
}

int find_txt_file(const char *directory, const char *filename_without_extension) {
    char filepath[MAX_PATH];

    snprintf(filepath, sizeof(filepath), "%s/%s.txt", directory, filename_without_extension);
    if (access(filepath, F_OK) != -1) {
        return 1;
    } else {
        return 0;
    }
}

