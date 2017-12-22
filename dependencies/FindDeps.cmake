# Find all dependencies
# defines:
#   DEPENDENCIES_FOUND
#   DEPENDENCIES_INCLUDE_DIRS
#   DEPENDENCIES_LIBRARIES

set(DEPENDENCIES_FOUND True)

find_package(Boost 1.65.1 REQUIRED COMPONENTS filesystem unit_test_framework)

set(ASAN_LIBRARY /usr/local/Cellar/llvm/5.0.0/lib/clang/5.0.0/lib/darwin/libclang_rt.asan_osx_dynamic.dylib)

set(DEPENDENCIES_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/fmt-4.0.0
    ${Boost_INCLUDES}
)

set(DEPENDENCIES_LIBRARIES fmt ${Boost_LIBRARIES} ${ASAN_LIBRARY})
