cmake_minimum_required(VERSION 2.6)

project(v8)

set(SRC_DIR src)
set(SRC ${SRC_DIR}/map.c
        ${SRC_DIR}/scgi.c)

include_directories(include)

add_library(v8 ${SRC})
