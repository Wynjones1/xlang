#pragma once
#include "common.h"

std::string read_file(const boost::filesystem::path &path)
{
    std::string out;
    if(!boost::filesystem::exists(path))
    {
        error("Filename '{0}' does not exist.\n", path.string());
    }
    if(!boost::filesystem::is_regular_file(path))
    {
        error("Filename '{0}' must be a file.\n", path.string());
    }
    std::FILE *fp = fopen(path.c_str(), "r");
    auto pos = std::fseek(fp, 0, SEEK_END);
    auto size = std::ftell(fp);
    std::fseek(fp, 0, SEEK_SET);
    out.resize(size);
    std::fread(out.data(), 1, size, fp);
    return out;
}
