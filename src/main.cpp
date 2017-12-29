#include "common.h"
#include "io.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char **argv)
{
    ASSERT_ERROR(argc > 1, "Must supply more than one argument.");
    auto data = read_file(argv[1]);
    auto tokens = TokenState(data);

    auto parse_result = parse(data);
    fmt::print("{}\n", parse_result.m_ast->print());
    return 0;
}
