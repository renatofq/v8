cmake_minimum_required(VERSION 2.8)

project(v8)

set(SRC_DIR src)
set(SRC ${SRC_DIR}/map.c
        ${SRC_DIR}/scgi.c)
set(HDR include/*.h)

include_directories(include)

add_library(v8 ${SRC})

install(TARGETS v8 DESTINATION lib)
install(FILES ${HDR} DESTINATION include/v8)

add_subdirectory(test)