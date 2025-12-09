#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include <iostream>
#include <cassert>

void test_struct_declaration()
{
    std::string source = R"(
        struct Point {
            let x: i32;
            let y: i32;
        }
    )";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    assert(stmts.size() == 1);
    auto structStmt = std::dynamic_pointer_cast<Chtholly::StructStmt>(stmts[0]);
    assert(structStmt != nullptr);
    assert(structStmt->name.lexeme == "Point");
    assert(structStmt->fields.size() == 2);
    std::cout << "test_struct_declaration passed." << std::endl;
}

void test_struct_instantiation()
{
    std::string source = R"(
        struct Point {
            let x: i32;
            let y: i32;
        }
        fn main(): void {
            let p = Point{x: 1, y: 2};
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
        std::cout << "test_struct_instantiation passed." << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "test_struct_instantiation failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_field_access()
{
    std::string source = R"(
        struct Point {
            let x: i32;
            let y: i32;
        }
        fn main(): void {
            let p = Point{x: 1, y: 2};
            let x = p.x;
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
        std::cout << "test_field_access passed." << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "test_field_access failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_field_assignment()
{
    std::string source = R"(
        struct Point {
            let mut x: i32;
            let y: i32;
        }
        fn main(): void {
            let mut p = Point{x: 1, y: 2};
            p.x = 3;
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
        std::cout << "test_field_assignment passed." << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "test_field_assignment failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_invalid_field_assignment()
{
    std::string source = R"(
        struct Point {
            let x: i32;
            let y: i32;
        }
        fn main(): void {
            let p = Point{x: 1, y: 2};
            p.x = 3;
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
        assert(error_message.find("Cannot assign to immutable field.") != std::string::npos);
        std::cout << "test_invalid_field_assignment passed." << std::endl;
    }
}

int main()
{
    test_struct_declaration();
    test_struct_instantiation();
    test_field_access();
    test_field_assignment();
    test_invalid_field_assignment();
    return 0;
}
