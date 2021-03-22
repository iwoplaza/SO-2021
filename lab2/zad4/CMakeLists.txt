cmake_minimum_required(VERSION 3.17)
project(zad4 C)

set(CMAKE_C_STANDARD 99)

add_executable(zad4 src/main.c)

target_include_directories(zad4 PUBLIC ../storeutils/src)
target_link_directories(zad4 PUBLIC ../storeutils/cmake-build-debug)

target_link_libraries(zad4 storeutils_sys)
#target_link_libraries(zad4 storeutils_lib)