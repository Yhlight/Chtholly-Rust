#include "semantic_analyzer.h"
#include "parser.h"
#include "lexer.h"
#include <iostream>
#include <vector>
#include <cassert>

void test_dangling_reference()
{
    std::string source = R"(
        let mut r: &i32;
        {
            let x = 10;
            r = &x;
        }
        // x is out of scope here, so r should be a dangling reference
    )";

    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();
    Chtholly::SemanticAnalyzer analyzer;

    bool caught_exception = false;
    try
    {
        analyzer.analyze(statements);
    }
    catch (const std::runtime_error& e)
    {
        caught_exception = true;
        std::string error_message = e.what();
        assert(error_message.find("Dangling reference") != std::string::npos);
    }

    assert(caught_exception);
    std::cout << "test_dangling_reference passed." << std::endl;
}

int main()
{
    test_dangling_reference();
    return 0;
}
