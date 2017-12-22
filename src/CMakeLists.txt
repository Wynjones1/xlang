cmake_minimum_required(VERSION 3.6)

include_directories(${DEPENDENCIES_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src)

add_definitions(-fno-exceptions --std=c++17 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)

add_library(compiler lexer.cpp parser.cpp)
add_executable(out main.cpp)
target_link_libraries(out compiler ${DEPENDENCIES_LIBRARIES})
