#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include <iostream>

void test_enum_declaration()
{
    std::string source = R"(
        enum Color {
            Red,
            Green,
            Blue(i32, i32, i32)
        }
    )";

    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();

    Chtholly::Parser parser(tokens);
    auto statements = parser.parse();

    if (parser.hadError())
    {
        std::cerr << "Test failed: Parser error." << std::endl;
        return;
    }

    Chtholly::SemanticAnalyzer analyzer;
    analyzer.analyze(statements);

    std::cout << "Test passed: Enum declaration" << std::endl;
}

int main()
{
    test_enum_declaration();
    return 0;
}
