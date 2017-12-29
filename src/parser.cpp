#include "parser.h"
#include <memory>
#include <type_traits>
#include <optional>
#include <map>

#define PARSE_FUNC(TYPE) template<> auto ParseState::parse<TYPE>() -> return_type<TYPE>

struct IgnoreTag {};
template<TokenType tok>
struct Ignore : public IgnoreTag {
    static constexpr TokenType value = tok;
};

struct AcceptTag {};
template<TokenType tok>
struct Accept : public AcceptTag{
    static constexpr TokenType value = tok;
};

template<typename T>
constexpr bool is_ignore() { return std::is_base_of<IgnoreTag, T>::value;}

template<typename T>
constexpr bool is_accept() { return std::is_base_of<AcceptTag, T>::value;}

template<typename T, typename... Rest>
auto get_return_type()
{
    if constexpr(is_accept<T>())
    {
        if constexpr(sizeof...(Rest) == 0)
        {
            return std::tuple<Token>();
        }
        else
        {
            return std::tuple_cat(std::tuple<Token>(), get_return_type<Rest...>());
        }
    }
    else if constexpr(is_ignore<T>())
    {
        if constexpr(sizeof...(Rest) == 0)
        {
            return std::tuple<>();
        }
        else
        {
            return get_return_type<Rest...>();
        }
    }
    else
    {
        if constexpr(sizeof...(Rest) == 0)
        {
            return std::tuple<AstPtr>();
        }
        else
        {
            return std::tuple_cat(std::tuple<AstPtr>(), get_return_type<Rest...>());
        }
    }
}

template<typename T, typename... Rest>
using return_type = std::optional<decltype(get_return_type<T, Rest...>())>;

class ParseState
{
public:
    ParseState(TokenState &tokens)
    : m_pos(0)
    , m_tokens(tokens)
    {}

    template<typename T, typename ListType=List>
    auto list(const T &callable)
    {
        auto out = std::make_unique<ListType>();
        while(auto x = callable())
        {
            out->push_back(x);
        }
        return out;
    }

    template<typename First, typename... Rest>
    auto any() -> return_type<AstPtr>
    {
        if constexpr(sizeof...(Rest) == 0)
        {
            return parse<First>();
        }
        else if(auto p = parse<First>())
        {
            return p;
        }
        else
        {
            return any<Rest...>();
        }
    }

    std::optional<Token> accept(TokenType type)
    {
        if(type != TokenType::Newline)
        {
            while(m_tokens.at(m_pos).m_type == TokenType::Newline)
            {
                m_pos++;
            }
        }
        if(m_tokens.at(m_pos).m_type == type)
        {
            m_pos += 1;
            debug::print("Accepted token at position {:03}: {}\n", m_pos, m_tokens.at(m_pos-1).m_value);
            return m_tokens.at(m_pos-1);
        }
        return std::nullopt;
    }

    template<typename T, typename... Rest>
    auto parse() -> return_type<T, Rest...>
    {
        using ret_t = return_type<T, Rest...>;
        if constexpr(is_ignore<T>())
        {
            if(auto tok = accept(T::value))
            {
                if constexpr(sizeof...(Rest) == 0)
                {
                    return std::tuple<>();
                }
                else
                {
                    return parse<Rest...>();
                }
            }
        }
        else if constexpr(is_accept<T>())
        {
            if(auto tok = accept(T::value))
            {
                if constexpr(sizeof...(Rest) == 0)
                {
                    return std::tuple<Token>(*tok);
                }
                else if(auto rest = parse<Rest...>())
                {
                    return std::tuple_cat(std::tuple(*tok), std::move(*rest));
                }
            }
        }
        else
        {
            if(auto t = parse<T>())
            {
                if constexpr(sizeof...(Rest) == 0)
                {
                    return std::move(*t);
                }
                else if(auto rest = parse<Rest...>())
                {
                    return std::tuple_cat(std::move(*t), std::move(*rest));
                }
            }
        }
        return std::nullopt;
    }

    size_t      m_pos;
    TokenState  &m_tokens;
};

PARSE_FUNC(FunctionArgs)
{
    return std::make_unique<List>();
}

PARSE_FUNC(Expression)
{
    parse<Accept<TokenType::Integer>,
      Accept<TokenType::Operator>,
      Accept<TokenType::Integer>>();
    return std::make_unique<Expression>();
}

PARSE_FUNC(ReturnStatement)
{
    if(auto temp = parse<Ignore<TokenType::KeywordReturn>, Expression>())
    {
        auto &[expr] = *temp;
        return std::make_unique<ReturnStatement>(expr);
    }
    return std::nullopt;
}

PARSE_FUNC(StatementList)
{
    return any<ReturnStatement, Expression>();
}

PARSE_FUNC(Top)
{
    auto temp = parse<Ignore<TokenType::KeywordFunction>,
          Accept<TokenType::Identifier>,
          Ignore<TokenType::LParen>,
                 FunctionArgs,
          Ignore<TokenType::RParen>,
          Ignore<TokenType::LBrace>,
                 StatementList,
          Ignore<TokenType::RBrace>>();
    if(temp)
    {
        auto &[id, args, stmts] = *temp;
        auto out = std::make_unique<Function>(id, args, stmts);
        return out;
    }
    return std::nullopt;
}

ParseResult parse(const std::string &value)
{
    ParseResult out = { TokenState(value)};
    fmt::print("{}\n", out.m_tokens.m_tokens.size());
    auto res = ParseState(out.m_tokens).template parse<Top>();
    if(res)
    {
        out.m_ast = std::move(std::get<0>(*res));
    }
    return out;
}
