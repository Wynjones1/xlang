#include <boost/test/unit_test.hpp>
#include "lexer.h"

bool TEST_TOKENISE(const TokenState &toks, const std::vector<std::pair<TokenType, std::string>>  &expect)
{
    BOOST_TEST(toks.m_tokens.size() == expect.size());
    for(int i = 0; i < toks.m_tokens.size(); i++)
    {
        auto &[type, value] = expect.at(i);
        BOOST_TEST(to_string(toks.m_tokens.at(i).m_type) == to_string(type));
        BOOST_TEST(toks.m_tokens.at(i).m_value == value);
    }
    return false;
}

bool TEST_TOKENISE(const std::string &input, const std::vector<std::pair<TokenType, std::string>>  &expect)
{
    auto toks = tokenise(input);
    return TEST_TOKENISE(toks, expect);
}


BOOST_AUTO_TEST_CASE(lexer_test_simple_0)
{
    TEST_TOKENISE("a", {{TokenType::Identifier, "a"}});
}

BOOST_AUTO_TEST_CASE(lexer_test_simple_1)
{
    TEST_TOKENISE("1", {{TokenType::Integer, "1"}});
}

BOOST_AUTO_TEST_CASE(lexer_test_simple_2)
{
    TEST_TOKENISE("a b", {{TokenType::Identifier, "a"},
                          {TokenType::Identifier, "b"},
                         });
}

BOOST_AUTO_TEST_CASE(lexer_test_simple_3)
{
    TEST_TOKENISE("a\nb", {{TokenType::Identifier, "a"},
                           {TokenType::Newline,    "\n"},
                           {TokenType::Identifier, "b"},
                          });
}

BOOST_AUTO_TEST_CASE(lexer_test_every_keyword)
{
    auto toks = tokenise("function return if");
    TEST_TOKENISE(toks , {{TokenType::KeywordFunction, "function"},
                          {TokenType::KeywordReturn,   "return"},
                          {TokenType::KeywordIf,       "if"},
                          });
    BOOST_TEST(toks.m_tokens.size() == num_keywords());
}

BOOST_AUTO_TEST_CASE(lexer_test_every_operator)
{
    TEST_TOKENISE("+-/*%< <= > >= == !=", {{TokenType::Operator,"+"},
                          {TokenType::Operator,"-"},
                          {TokenType::Operator,"/"},
                          {TokenType::Operator,"*"},
                          {TokenType::Operator,"%"},
                          {TokenType::Operator,"<"},
                          {TokenType::Operator,"<="},
                          {TokenType::Operator,">"},
                          {TokenType::Operator,">="},
                          {TokenType::Operator,"=="},
                          {TokenType::Operator,"!="},
                          {TokenType::Operator,""},
                          });
}
