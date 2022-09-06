cmake_minimum_required(VERSION 3.16)

project(multiplex-tcp-server)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PUBLIC tcp_socket)
target_include_directories(${PROJECT_NAME} PUBLIC ../)
