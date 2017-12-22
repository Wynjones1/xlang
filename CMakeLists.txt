cmake_minimum_required(VERSION 3.6)

# Setup CMake variables.
set(CMAKE_EXPORT_COMPILE_COMMANDS  True)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

find_package(Deps REQUIRED)

add_subdirectory(dependencies)
add_subdirectory(src)
add_subdirectory(tests)
