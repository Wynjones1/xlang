#include "parser.h"

struct IgnoreTest {};
template<TokenType tok>
struct Ignore : public IgnoreTest {
    static constexpr TokenType value = tok;
};

struct AcceptTest {};
template<TokenType tok>
struct Accept : public AcceptTest{
    static constexpr TokenType value = tok;
};

template<typename T>
constexpr bool is_ignore() { return std::is_base_of<IgnoreTest, T>::value;}

template<typename T>
constexpr bool is_accept() { return std::is_base_of<AcceptTest, T>::value;}

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
    ParseState(TokenState tokens)
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
    TokenState  m_tokens;
};

template<>
auto ParseState::parse<FunctionArgs>() -> return_type<FunctionArgs>
{
    return std::make_unique<List>();
}

template<>
auto ParseState::parse<Expression>() -> return_type<Expression>
{
    parse<Accept<TokenType::Integer>,
      Accept<TokenType::Operator>,
      Accept<TokenType::Integer>>();
    return std::make_unique<Expression>();
}

template<>
auto ParseState::parse<ReturnStatement>() -> return_type<ReturnStatement>
{
    parse<Ignore<TokenType::KeywordReturn>, Expression>();
    return std::make_unique<ReturnStatement>();
}

template<>
auto ParseState::parse<StatementList>() -> return_type<StatementList>
{
    return any<ReturnStatement, Expression>();
}

template<>
auto ParseState::parse<Top>() -> return_type<Top>
{
    auto x = parse<Ignore<TokenType::KeywordFunction>,
          Accept<TokenType::Identifier>,
          Ignore<TokenType::LParen>,
                 FunctionArgs,
          Ignore<TokenType::RParen>,
          Ignore<TokenType::LBrace>,
                 StatementList,
          Ignore<TokenType::RBrace>>();
    return std::tuple<AstPtr>(nullptr);
}

ParseResult parse(const std::string &value)
{
    ParseResult out = { TokenState(value), nullptr };
    return out;
}
