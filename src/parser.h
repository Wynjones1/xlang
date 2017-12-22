#pragma once
#include <memory>
#include <type_traits>
#include <optional>
#include "lexer.h"

class Ast {};
using AstPtr = std::unique_ptr<Ast>;

class List : public Ast
{
public:
    void push_back(AstPtr &ptr)
    {
        m_data.push_back(std::move(ptr));
    }

    std::vector<AstPtr> m_data;
};

class Top             : public Ast  {};
class Function        : public Top  {};
class FunctionArgs    : public Top  {};
class StatementList   : public List {};
class Statement       : public Ast  {};
class ReturnStatement : public Ast {};
class Expression      : public Ast {};

struct ParseResult
{
    TokenState m_tokens;
    AstPtr     m_ast;
};

ParseResult parse(const std::string &value);
