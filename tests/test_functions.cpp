#include "semantic_analyzer.h"
#include "parser.h"
#include "lexer.h"
#include <iostream>
#include <vector>
#include <cassert>

void test_simple_function()
{
    std::string source = R"(
        fn add(a: i32, b: i32): i32 {
            return a + b;
        }

        let result = add(1, 2);
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
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    assert(!caught_exception);
    std::cout << "test_simple_function passed." << std::endl;
}

void test_argument_type_mismatch()
{
    std::string source = R"(
        fn add(a: i32, b: i32): i32 {
            return a + b;
        }

        let result = add(1, "2");
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
        assert(error_message.find("Argument type mismatch") != std::string::npos);
    }

    assert(caught_exception);
    std::cout << "test_argument_type_mismatch passed." << std::endl;
}

void test_return_type_mismatch()
{
    std::string source = R"(
        fn add(a: i32, b: i32): i32 {
            return "a + b";
        }
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
        assert(error_message.find("Return type mismatch") != std::string::npos);
    }

    assert(caught_exception);
    std::cout << "test_return_type_mismatch passed." << std::endl;
}

void test_return_local_reference()
{
    std::string source = R"(
        fn get_ref(): &i32 {
            let x = 10;
            return &x;
        }
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
        assert(error_message.find("Cannot return a reference to a local variable") != std::string::npos);
    }

    assert(caught_exception);
    std::cout << "test_return_local_reference passed." << std::endl;
}

void test_binary_expression_type()
{
    std::string source = R"(
        fn greater(a: i32, b: i32): bool {
            return a > b;
        }
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
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    assert(!caught_exception);
    std::cout << "test_binary_expression_type passed." << std::endl;
}

void test_function_call_type_inference()
{
    std::string source = R"(
        fn get_num(): i32 {
            return 42;
        }
        let x = get_num();
        let y: i32 = x;
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
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    assert(!caught_exception);
    std::cout << "test_function_call_type_inference passed." << std::endl;
}

int main()
{
    test_simple_function();
    test_argument_type_mismatch();
    test_return_type_mismatch();
    test_return_local_reference();
    test_binary_expression_type();
    test_function_call_type_inference();
    return 0;
}
