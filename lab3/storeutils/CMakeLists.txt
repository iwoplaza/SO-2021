cmake_minimum_required(VERSION 3.17)
project(storeutils C)

set(CMAKE_C_STANDARD 99)
set(LIB_SOURCES src/storeutils.c src/storeutils.h)

add_library(storeutils_lib ${LIB_SOURCES})
add_library(storeutils_sys ${LIB_SOURCES})

target_compile_definitions(storeutils_sys PUBLIC -DSU_SYS_SOLUTION)