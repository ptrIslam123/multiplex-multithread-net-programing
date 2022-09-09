cmake_minimum_required(VERSION 3.16)

project(project)

add_subdirectory(core)
add_subdirectory(multiplex-tcp-server)
add_subdirectory(multithread-multiplex-tcp-server)
add_subdirectory(raw_socket)