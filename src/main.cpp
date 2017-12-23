#include "common.h"
#include "io.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char **argv)
{
    ASSERT_ERROR(argc > 1, "Must supply more than one argument.");
    auto data = read_file(argv[1]);
    auto tokens = tokenise(data);
    for(int i = 0; i < tokens.size(); i++)
    {
        fmt::print("{}\n", std::string(80, '*'));
        tokens.print_token(i, 2);
        fflush(stdout);
    }

    //auto ast = parse(data);
    return 0;
}
