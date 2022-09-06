cmake_minimum_required(VERSION 3.16)

project(tcp_socket)

add_library(${PROJECT_NAME} STATIC tcp_socket.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})