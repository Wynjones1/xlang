#include "lexer.h"
#include <optional>

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
    static std::unordered_map<std::string_view, TokenType> kw_mapping =
    {
        {"function", TokenType::KeywordFunction},
        {"return",   TokenType::KeywordReturn},
        {"if",       TokenType::KeywordIf},
    };
    if(auto it = kw_mapping.find(view); it != std::end(kw_mapping))
    {
        return it->second;
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
    auto min_index = (t.m_line < context) ? 0 : (t.m_line - context);
    auto max_index = std::min(num_lines(), t.m_line + context + 1);

    for(auto i = min_index; i < max_index; i++)
    {
        fmt::print("{}", m_lines.at(i));
        if(i == m_tokens.at(index).m_line)
        {
            fmt::print("{}^{}\n", std::string(t.m_column, ' '), std::string(t.m_value.size() - 1, '~'));
        }
    }
}

TokenState tokenise(const std::string &value)
{
    std::vector<std::pair<TokenType, std::regex>> regexes = {
        {TokenType::Identifier, std::regex(R"(^[a-zA-Z][a-zA-Z0-9_]*)")},
        {TokenType::Integer,    std::regex(R"(^[0-9]+)")},
        {TokenType::Operator,   std::regex(R"(^(\+|\-|/|\*|%|<=?|>=?|==|!=))")},
        {TokenType::Newline,    std::regex(R"(^\n)")},
        {TokenType::Whitespace, std::regex(R"(^[ \t]+)")},
        {TokenType::LParen,     std::regex(R"(^\()")},
        {TokenType::RParen,     std::regex(R"(^\))")},
        {TokenType::LBrace,     std::regex(R"(^\{)")},
        {TokenType::RBrace,     std::regex(R"(^\})")},
    };

    auto out        = TokenState(value);
    auto b          = std::begin(value);
    auto e          = std::end(value);
    auto line_start = size_t(0);
    auto column     = size_t(0);

    while(b != e)
    {
        // Loop over regex and find the largest match at the current start position.
        auto match = std::optional<std::pair<TokenType,std::smatch>>();
        for(const auto &[type, regex] : regexes)
        {
            auto new_match = std::smatch();
            if(std::regex_search(b, e, new_match, regex))
            {
                if(!match || new_match[0].length() > match->second[0].length())
                {
                    match = {type, new_match};
                }
            }
        }

        // If we haven't found a match at this point we need to error out.
        if(!match)
        {
            char temp[1] = {'\n'};
            auto end_line = std::find_first_of(b, e, std::begin(temp), std::end(temp));
            error("Could not find match.\n{}\n", std::string(b, end_line));
        }

        // Add the token to the list and adjust line, column pointers etc.
        auto &[t, m] = *match;
        auto len = m[0].length();
        if(t != TokenType::Whitespace)
        {
            auto &tok = out.add_token(t, line_start, column, len);
        }
        column += len;
        b      += len;
        if(t == TokenType::Newline)
        {
            line_start += column;
            column      = 0;
        }
    }
    return out;
}
