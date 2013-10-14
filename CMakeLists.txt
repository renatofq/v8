cmake_minimum_required(VERSION 2.8)

project(v8)

add_definitions(-Wall -Werror -std=gnu99)

set(SRC_DIR src)
set(SRC ${SRC_DIR}/buffer.c
        ${SRC_DIR}/list.c
        ${SRC_DIR}/map.c
        ${SRC_DIR}/strmap.c
        ${SRC_DIR}/cookie.c
        ${SRC_DIR}/request.c
        ${SRC_DIR}/response.c
        ${SRC_DIR}/scgi.c
        ${SRC_DIR}/v8.c)

set(HDR include/*.h)

include_directories(include)

add_library(v8 SHARED ${SRC})
target_link_libraries(v8 pthread)

install(TARGETS v8 DESTINATION lib)
install(FILES ${HDR} DESTINATION include/v8)

add_subdirectory(test)
add_subdirectory(example)
