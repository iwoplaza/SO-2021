cmake_minimum_required(VERSION 3.17)
project(zad2 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")

add_executable(zad2 main.c)

target_include_directories(zad2 PUBLIC ../mergeutils/src)
target_link_directories(zad2 PUBLIC ../mergeutils/out/static)

target_link_libraries(zad2 mergeutils)

