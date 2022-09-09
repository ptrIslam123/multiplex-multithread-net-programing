cmake_minimum_required(VERSION 3.16)

project(raw_socket)

add_executable(${PROJECT_NAME}
    main.c
    extract_headers.c
    net_interface_info.c
    maker_net_headers.c
)

target_link_libraries(${PROJECT_NAME} PUBLIC utils)