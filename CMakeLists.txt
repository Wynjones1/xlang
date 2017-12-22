cmake_minimum_required(VERSION 3.6)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

find_package(Deps REQUIRED)

add_subdirectory(dependencies)
include_directories(${DEPENDENCIES_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src)

add_definitions(-fno-exceptions --std=c++17 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
add_executable(out src/main.cpp)
target_link_libraries(out ${DEPENDENCIES_LIBRARIES})
