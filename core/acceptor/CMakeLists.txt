cmake_minimum_required(VERSION 3.16)

project(acceptor)

add_library(${PROJECT_NAME} STATIC multiplex_acceptor.c)

target_link_libraries(${PROJECT_NAME} PUBLIC tcp_socket)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
