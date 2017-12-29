#pragma once
#include <algorithm>
#include <regex>
#include <map>
#include <unordered_map>
#include "common.h"
#include "debug.h"

#define X_TOKEN_LIST \
    X(Identifier) \
    X(Integer) \
    X(LBrace) \
    X(LParen) \
    X(Newline) \
    X(Operator) \
    X(RBrace) \
    X(RParen) \
    X(Whitespace) \
    X(KeywordFunction) \
    X(KeywordReturn) \
    X(KeywordIf) \

enum class TokenType
{
#define X(name) name,
    X_TOKEN_LIST
#undef X
};

std::string to_string(TokenType type);

constexpr std::string_view to_string_view(TokenType type)
{
    #define X(name) if(type == TokenType::name) return #name ## sv;
    X_TOKEN_LIST
    #undef X
    return "??";
}

constexpr auto num_keywords()
{
    constexpr auto prefix = "Keyword"sv;
    uint32_t out = 0;
#define X(name) \
    if((to_string_view(TokenType:: name).size() > prefix.size()) && \
        (to_string_view(TokenType:: name).substr(0, prefix.size()) == prefix)) \
    { \
        out += 1; \
    }
    X_TOKEN_LIST
#undef X
    return out;
}

struct Token
{
    TokenType        m_type;
    std::string_view m_value;
    size_t           m_line;
    size_t           m_column;
};

struct TokenState
{
    const std::string m_data;
    std::vector<Token>            m_tokens;
    std::vector<std::string_view> m_lines;

    TokenState(const std::string &data);

    Token  &back();
    Token  &at(size_t index);
    Token  &add_token(TokenType type, size_t line_offset, size_t column, size_t len);
    size_t  size();
    size_t  num_lines();
    void    print_token(size_t index, int context = 0);
};
