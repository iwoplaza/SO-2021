cmake_minimum_required(VERSION 3.17)
project(merger C)

set(CMAKE_C_STANDARD 99)

add_executable(merger src/main.c src/mergeutils.c src/mergeutils.h)

target_include_directories(merger PUBLIC ../storeutils/src)
target_link_directories(merger PUBLIC ../storeutils/cmake-build-debug)

target_link_libraries(merger storeutils_sys)
#target_link_libraries(merger storeutils_lib)