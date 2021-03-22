cmake_minimum_required(VERSION 3.17)
project(filter C)

set(CMAKE_C_STANDARD 99)

add_executable(filter src/main.c)

target_include_directories(filter PUBLIC ../storeutils/src)
target_link_directories(filter PUBLIC ../storeutils/cmake-build-debug)

target_link_libraries(filter storeutils_sys)
#target_link_libraries(filter storeutils_lib)