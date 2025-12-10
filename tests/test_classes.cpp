#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include <iostream>
#include <cassert>

void test_class_declaration()
{
    std::string source = R"(
        class Point {
            let x: i32;
            let y: i32;
            fn new(x: i32, y: i32): Point {
                self.x = x;
                self.y = y;
                return self;
            }
        }
    )";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    assert(stmts.size() == 1);
    auto classStmt = std::dynamic_pointer_cast<Chtholly::ClassStmt>(stmts[0]);
    assert(classStmt != nullptr);
    assert(classStmt->name.lexeme == "Point");
    assert(classStmt->fields.size() == 2);
    assert(classStmt->methods.size() == 1);
    std::cout << "test_class_declaration passed." << std::endl;
}

void test_class_instantiation()
{
    std::string source = R"(
        class Point {
            let x: i32;
            let y: i32;
            fn new(x: i32, y: i32): Point {
                self.x = x;
                self.y = y;
                return self;
            }
        }
        fn main(): void {
            let p = Point.new(1, 2);
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
        std::cout << "test_class_instantiation passed." << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "test_class_instantiation failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_member_access()
{
    std::string source = R"(
        class Point {
            let x: i32;
            let y: i32;
            fn new(x: i32, y: i32): Point {
                self.x = x;
                self.y = y;
                return self;
            }
        }
        fn main(): void {
            let p = Point.new(1, 2);
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
        std::cout << "test_member_access passed." << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "test_member_access failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_invalid_self_use()
{
    std::string source = "fn main(): void { self; }";
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
        assert(error_message.find("Cannot use 'self' outside of a class.") != std::string::npos);
        std::cout << "test_invalid_self_use passed." << std::endl;
    }
}

int main()
{
    std::cout << "--- Running test_class_declaration ---" << std::endl;
    test_class_declaration();
    std::cout << "--- Running test_class_instantiation ---" << std::endl;
    test_class_instantiation();
    std::cout << "--- Running test_member_access ---" << std::endl;
    test_member_access();
    std::cout << "--- Running test_invalid_self_use ---" << std::endl;
    test_invalid_self_use();
    return 0;
}
