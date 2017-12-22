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

std::string to_string(TokenType type)
{
    #define X(name) if(type == TokenType::name) return #name;
    X_TOKEN_LIST
    #undef X
    return "??";
}

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
    std::string                   m_data;
    std::vector<Token>            m_tokens;
    std::vector<std::string_view> m_lines;

    TokenState(const std::string &data)
    : m_data(data)
    {}

    auto &back()
    {
        return m_tokens.back();
    }

    TokenType try_convert_to_keyword(std::string_view &view)
    {
        static std::unordered_map<std::string, TokenType> kw_mapping =
        {
            {"function", TokenType::KeywordFunction},
            {"return",   TokenType::KeywordReturn},
            {"if",       TokenType::KeywordIf},
        };
        for(auto &[k, v] : kw_mapping)
        {
            if(k == view) return v;
        }
        return TokenType::Identifier;
    }

    auto &add_token(TokenType type, size_t line_offset, size_t column, size_t len)
    {
        auto view = std::string_view(m_data.data() + line_offset + column, len);
        if(type == TokenType::Identifier)
        {
            type = try_convert_to_keyword(view);
        }
        m_tokens.push_back(Token{type, view, m_lines.size(), column});
        if(type == TokenType::Newline)
        {
            m_lines.emplace_back(m_data.data() + line_offset, column + len);
        }
        debug::print("Added token: line offset {:04}, column {:03}, length {:02}, TokenType::{:<20}, value '{}'\n",
                                                               line_offset, column, len, to_string(type),
                                                               (type == TokenType::Newline) ? "<newline>"sv : view);
        return back();
    }

    auto size()
    {
        return m_tokens.size();
    }

    auto &at(size_t index)
    {
        return m_tokens.at(index);
    }

    auto num_lines()
    {
        return back().m_line + 1;
    }

    void print_token(size_t index, int context = 0)
    {
        auto &t        = at(index);
        auto lines     = num_lines();
        auto min_index = (context            > t.m_line) ? 0     : t.m_line - context;
        auto max_index = (context + t.m_line > lines   ) ? lines : t.m_line + context + 1;

        for(auto i = min_index; i < max_index; i++)
        {
            fmt::print("{}", m_lines[i]);
            if(i == m_tokens[index].m_line)
            {
                fmt::print("{}^{}\n", std::string(t.m_column, ' '), std::string(t.m_value.size() - 1, '~'));
            }
        }
    }
};

TokenState tokenise(const std::string &value)
{
    std::map<TokenType, std::regex> regexes = {
        {TokenType::Identifier, std::regex(R"(^[a-zA-Z][a-zA-Z0-9_]*)")},
        {TokenType::Integer,    std::regex(R"(^[0-9]+)")},
        {TokenType::Operator,   std::regex(R"(^(\+|\-|\*|%|<|<=|>|>=|==|!=))")},
        {TokenType::Newline,    std::regex(R"(^\n)")},
        {TokenType::Whitespace, std::regex(R"(^\s+)")},
        {TokenType::LParen,     std::regex(R"(^\()")},
        {TokenType::RParen,     std::regex(R"(^\))")},
        {TokenType::LBrace,     std::regex(R"(^\{)")},
        {TokenType::RBrace,     std::regex(R"(^\})")},
    };
    TokenState out(value);

    auto match      = std::smatch();
    auto b           = std::begin(value);
    auto e           = std::end(value);
    auto line_start  = size_t(0);
    auto column      = size_t(0);

again:
    while(b != e)
    {
        for(const auto &[type, regex] : regexes)
        {
            if(std::regex_search(b, e, match, regex))
            {
                auto len = match[0].length();
                if(type != TokenType::Whitespace)
                {
                    auto &tok = out.add_token(type, line_start, column, len);
                }
                column += len;
                b      += len;
                if(type == TokenType::Newline)
                {
                    line_start += column;
                    column      = 0;
                }
                goto again;
            }
        }
        char temp[1] = {'\n'};
        auto end_line = std::find_first_of(b, e, std::begin(temp), std::end(temp));
        error("Could not find match.\n{}\n", std::string(b, end_line));
    }
    return out;
}
