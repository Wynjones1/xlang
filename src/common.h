#pragma once
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <string_view>
#include <boost/filesystem.hpp>
#include <cstdio>

using namespace std::literals;

template<typename... Args>
[[noreturn]] void error(const std::string &format, Args&&... args)
{
    fmt::print(format, std::forward<Args>(args)...);
    exit(-1);
}

#define ASSERT_ERROR(test, msg, ...) \
do \
{ \
    if(!(test)) \
    { \
        error("Test '" #test "' failed. " msg "\n", #__VA_ARGS__); \
    } \
} \
while(0);
