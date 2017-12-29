cmake_minimum_required(VERSION 3.6)

set(GEN_DIR  ${CMAKE_BINARY_DIR}/include)
set(GEN_FILE ${GEN_DIR}/ast_types.h)
set(GEN_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/generate_ast_types.py)

include_directories(
    ${DEPENDENCIES_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${GEN_DIR}
)

add_custom_command(OUTPUT ${GEN_FILE}
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/include
    COMMAND ${GEN_SCRIPT}  ${GEN_FILE}
    COMMAND clang-format -i ${GEN_FILE}
    DEPENDS ${GEN_SCRIPT}
)
add_definitions(-fno-exceptions --std=c++17 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)

add_library(compiler lexer.cpp parser.cpp ${GEN_FILE})
add_executable(out main.cpp)
target_link_libraries(out compiler ${DEPENDENCIES_LIBRARIES})
