cmake_minimum_required(VERSION 3.6)

include_directories(${DEPENDENCIES_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src)
add_executable(unittests test_main.cpp)
