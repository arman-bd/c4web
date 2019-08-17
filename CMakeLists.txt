cmake_minimum_required(VERSION 2.8.9)
set(PROJECT_NAME "C4Web")
project(${PROJECT_NAME})

find_package (Threads REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(c4web main.c)
target_link_libraries(c4web ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARIES})