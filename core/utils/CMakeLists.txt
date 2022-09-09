cmake_minimum_required(VERSION 3.16)

project(utils)

add_library(${PROJECT_NAME} STATIC reverse_buffer/reverse_buffer.c)

target_include_directories(${PROJECT_NAME} PUBLIC reverse_buffer)