cmake_minimum_required(VERSION 3.17)
project(zad3 C)

set(CMAKE_C_STANDARD 99)

add_executable(zad3 src/main.c)

target_include_directories(zad3 PUBLIC ../storeutils/src)
target_link_directories(zad3 PUBLIC ../storeutils/cmake-build-debug)

target_link_libraries(zad3 storeutils_sys)
#target_link_libraries(zad3 storeutils_lib)