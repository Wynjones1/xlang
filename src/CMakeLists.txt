cmake_minimum_required(VERSION 3.6)

include_directories(${DEPENDENCIES_INCLUDE_DIRS})

add_definitions(-fno-exceptions --std=c++17 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)

add_executable(out main.cpp)
target_link_libraries(out ${DEPENDENCIES_LIBRARIES})
