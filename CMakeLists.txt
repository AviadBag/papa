cmake_minimum_required(VERSION 3.27)
project(papa C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_OSX_ARCHITECTURES "arm64")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer")

add_executable(papa main.c)
