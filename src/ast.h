#pragma once
#include <memory>
#include <vector>
#include "lexer.h"

using AstPtr = std::unique_ptr<class Ast>;

class Ast
{
public:
    virtual std::string print(int indent = 0) = 0;
    virtual ~Ast();
};

class List : public Ast
{
public:
    void push_back(AstPtr &ptr)
    {
        m_data.push_back(std::move(ptr));
    }

    std::vector<AstPtr> m_data;
    virtual std::string print(int indent = 0)
    {
        auto out = std::string();
        for(auto &x : m_data)
        {
            out += x->print(indent + 1);
        }
        return out;
    }
};
