#ifndef FILE_REQUEST_FUNCTION_H
#define FILE_REQUEST_FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define SIZE_FILENAME 100
#define SIZE_LINE 256
#define MAX_PATH 1024
#define SIZE_DIR 50
#define SIZE_FILE_NAME 50
#define SIZE_NAME_CHAT 256
#define OPEN "open"
#define PENDING "pending"

typedef struct Message {
  int index;   
  time_t time;  
  char *string; 
  int flagname;

} Message;

typedef struct {
  int id;
  time_t time_create;
  time_t time_end;
  int active;
  char *title;
} Request;

int read_config();

void update_config(int counter);

void create_request_file(Request *request, int counter);

int get_last_message_index(const char *filename);

void append_message_to_file(int request_id, Message *message, char *nameclient);

void move_request_file(int id, const char *from_dir, const char *to_dir);
void print_request_file(int request_id);
void create_all_if_not_exists();
void create_directory_if_not_exists(const char *dir_name);
void create_file_if_not_exists(const char *file_name);
int find_txt_file(const char *directory, const char *filename_without_extension);
#endif 