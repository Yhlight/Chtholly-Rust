#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include <iostream>
#include <cassert>

void test_valid_switch_statement()
{
    std::string source = R"(
        fn main(): void {
            let x = 1;
            switch (x) {
                case 1: {
                    let a = 1;
                    break;
                }
                case 2: {
                    let b = 2;
                    fallthrough;
                }
                default: {
                    let c = 3;
                    break;
                }
            }
        }
    )";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    Chtholly::SemanticAnalyzer semanticAnalyzer;
    try
    {
        semanticAnalyzer.analyze(stmts);
        std::cout << "test_valid_switch_statement passed." << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "test_valid_switch_statement failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_multiple_defaults()
{
    std::string source = R"(
        fn main(): void {
            let x = 1;
            switch (x) {
                default: {
                    break;
                }
                default: {
                    break;
                }
            }
        }
    )";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    Chtholly::SemanticAnalyzer semanticAnalyzer;
    try
    {
        semanticAnalyzer.analyze(stmts);
        assert(false);
    }
    catch (const std::runtime_error& e)
    {
        std::string error_message = e.what();
        assert(error_message.find("Multiple default cases in switch statement.") != std::string::npos);
        std::cout << "test_multiple_defaults passed." << std::endl;
    }
}

void test_duplicate_cases()
{
    std::string source = R"(
        fn main(): void {
            let x = 1;
            switch (x) {
                case 1: {
                    break;
                }
                case 1: {
                    break;
                }
            }
        }
    )";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    Chtholly::SemanticAnalyzer semanticAnalyzer;
    try
    {
        semanticAnalyzer.analyze(stmts);
        assert(false);
    }
    catch (const std::runtime_error& e)
    {
        std::string error_message = e.what();
        assert(error_message.find("Duplicate case value.") != std::string::npos);
        std::cout << "test_duplicate_cases passed." << std::endl;
    }
}

void test_type_mismatch()
{
    std::string source = R"(
        fn main(): void {
            let x = 1;
            switch (x) {
                case "hello": {
                    break;
                }
            }
        }
    )";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    Chtholly::SemanticAnalyzer semanticAnalyzer;
    try
    {
        semanticAnalyzer.analyze(stmts);
        assert(false);
    }
    catch (const std::runtime_error& e)
    {
        std::string error_message = e.what();
        assert(error_message.find("Case type does not match switch type.") != std::string::npos);
        std::cout << "test_type_mismatch passed." << std::endl;
    }
}

void test_invalid_fallthrough_position()
{
    std::string source = R"(
        fn main(): void {
            let x = 1;
            switch (x) {
                case 1: {
                    fallthrough;
                    let a = 1;
                    break;
                }
            }
        }
    )";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    Chtholly::SemanticAnalyzer semanticAnalyzer;
    try
    {
        semanticAnalyzer.analyze(stmts);
        assert(false);
    }
    catch (const std::runtime_error& e)
    {
        std::string error_message = e.what();
        assert(error_message.find("Fallthrough must be the last statement in a case block.") != std::string::npos);
        std::cout << "test_invalid_fallthrough_position passed." << std::endl;
    }
}

void test_duplicate_string_cases()
{
    std::string source = R"(
        fn main(): void {
            let x = "hello";
            switch (x) {
                case "hello": {
                    break;
                }
                case "hello": {
                    break;
                }
            }
        }
    )";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    Chtholly::SemanticAnalyzer semanticAnalyzer;
    try
    {
        semanticAnalyzer.analyze(stmts);
        assert(false);
    }
    catch (const std::runtime_error& e)
    {
        std::string error_message = e.what();
        assert(error_message.find("Duplicate case value.") != std::string::npos);
        std::cout << "test_duplicate_string_cases passed." << std::endl;
    }
}

int main()
{
    test_valid_switch_statement();
    test_multiple_defaults();
    test_duplicate_cases();
    test_type_mismatch();
    test_invalid_fallthrough_position();
    test_duplicate_string_cases();
    return 0;
}
