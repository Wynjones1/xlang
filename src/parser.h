#pragma once
#include "ast_types.h"
#include "lexer.h"

struct ParseResult
{
    TokenState m_tokens;
    AstPtr     m_ast = nullptr;
};

ParseResult parse(const std::string &value);
