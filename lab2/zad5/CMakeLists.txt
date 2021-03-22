cmake_minimum_required(VERSION 3.17)
project(zad5 C)

set(CMAKE_C_STANDARD 99)

add_executable(zad5 src/main.c)

target_include_directories(zad5 PUBLIC ../storeutils/src)
target_link_directories(zad5 PUBLIC ../storeutils/cmake-build-debug)

target_link_libraries(zad5 storeutils_sys)
#target_link_libraries(zad5 storeutils_lib)