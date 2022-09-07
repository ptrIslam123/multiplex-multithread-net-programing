cmake_minimum_required(VERSION 3.16)

project(thread_pool)

add_library(${PROJECT_NAME} STATIC
    static_thread_pool_api.c
    queue_api.c
)

target_link_libraries(${PROJECT_NAME} PUBLIC pthread)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})