cmake_minimum_required(VERSION 3.17)
project(zad1 C)
project(other C)

set(CMAKE_C_STANDARD 99)

add_executable(zad1 main.c mode.h mode.c)
add_executable(other other.c)