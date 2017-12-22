cmake_minimum_required(VERSION 3.6)

include_directories(${DEPENDENCIES_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src)
add_definitions(-fno-exceptions --std=c++17 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
add_executable(unittests test_main.cpp lexer_tests.cpp)
target_link_libraries(unittests compiler ${DEPENDENCIES_LIBRARIES})
