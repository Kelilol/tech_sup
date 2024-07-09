cmake_minimum_required(VERSION 3.10)  


project(MyProject C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -Werror")

set(FILE_TXT_FUNCTION_DIR "File_txt_Function")
set(FILE_TXT_FUNCTION_SRC "${FILE_TXT_FUNCTION_DIR}/File_request_function.c")

set(CLIENT_DIR "client")
set(SERVER_DIR "server")

set(CLIENT_SRC "${CLIENT_DIR}/client.c" "${CLIENT_DIR}/client_main.c")
set(SERVER_SRC "${SERVER_DIR}/server.c" "${SERVER_DIR}/server_main.c")

add_executable(client_app ${CLIENT_SRC} ${FILE_TXT_FUNCTION_SRC})
add_executable(server_app ${SERVER_SRC} ${FILE_TXT_FUNCTION_SRC})

target_include_directories(client_app PRIVATE ${CLIENT_DIR})
target_include_directories(server_app PRIVATE ${SERVER_DIR})
