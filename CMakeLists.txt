cmake_minimum_required(VERSION 2.8)

project(v8)

# set(CMAKE_C_FLAGS_DEBUG -DV8_DEBUG -ggdb)

add_definitions(-Wall -Werror -std=gnu99)

set(SRC_DIR src)
set(SRC ${SRC_DIR}/log.c
        ${SRC_DIR}/buffer.c
        ${SRC_DIR}/list.c
        ${SRC_DIR}/map.c
        ${SRC_DIR}/strmap.c
        ${SRC_DIR}/cookie.c
        ${SRC_DIR}/request.c
        ${SRC_DIR}/response.c
        ${SRC_DIR}/view.c
        ${SRC_DIR}/scgi.c
        ${SRC_DIR}/config.c
        ${SRC_DIR}/lua.c
        ${SRC_DIR}/v8.c)

set(HDR include/*.h)

include_directories(include)

add_library(v8 SHARED ${SRC})
target_link_libraries(v8 pthread lua)

install(TARGETS v8 DESTINATION lib)
install(FILES ${HDR} DESTINATION include/v8)

add_subdirectory(test)
add_subdirectory(example)
