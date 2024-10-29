cmake_minimum_required(VERSION 3.27)
project(papa C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_OSX_ARCHITECTURES "arm64")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer")

# Set optimization flags
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native")

add_executable(papa main.c
        found_tree.h
        main.h
        found_tree.c
)
