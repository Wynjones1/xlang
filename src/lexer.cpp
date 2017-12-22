#include "lexer.h"

std::string to_string(TokenType type)
{
    #define X(name) if(type == TokenType::name) return #name;
    X_TOKEN_LIST
    #undef X
    return "??";
}

Token &TokenState::back()
{
    return m_tokens.back();
}

static auto try_convert_to_keyword(std::string_view &view)
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

Token &TokenState::add_token(TokenType type, size_t line_offset, size_t column, size_t len)
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

size_t TokenState::size()
{
    return m_tokens.size();
}

Token &TokenState::at(size_t index)
{
    return m_tokens.at(index);
}

size_t TokenState::num_lines()
{
    return back().m_line + 1;
}

void TokenState::print_token(size_t index, int context)
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

TokenState tokenise(const std::string &value)
{
    std::map<TokenType, std::regex> regexes = {
        {TokenType::Identifier, std::regex(R"(^[a-zA-Z][a-zA-Z0-9_]*)")},
        {TokenType::Integer,    std::regex(R"(^[0-9]+)")},
        {TokenType::Operator,   std::regex(R"(^(\+|\-|/|\*|%|<=?|>=?|==|!=))")},
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
