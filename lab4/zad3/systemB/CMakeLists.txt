cmake_minimum_required(VERSION 3.17)
project(sender C)
project(catcher C)

set(CMAKE_C_STANDARD 99)

add_executable(sender sender.c mode.c msg.c)
add_executable(catcher catcher.c mode.c msg.c)