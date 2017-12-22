#pragma once
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace debug
{
    template<typename... Args>
    void print(Args&&... args)
    {
        if(getenv("DEBUG"))
        {
            fmt::print(std::forward<Args>(args)...);
        }
    }
};
