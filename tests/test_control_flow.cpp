#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include <iostream>
#include <cassert>

void test_if_statement() {
    std::string source = "fn main(): void { if (true) { let a = 1; } }";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    assert(stmts.size() == 1);
    auto funcStmt = std::dynamic_pointer_cast<Chtholly::FunctionStmt>(stmts[0]);
    assert(funcStmt != nullptr);
    assert(funcStmt->body->statements.size() == 1);
    auto ifStmt = std::dynamic_pointer_cast<Chtholly::IfStmt>(funcStmt->body->statements[0]);
    assert(ifStmt != nullptr);
    assert(ifStmt->elseBranch == nullptr);
    std::cout << "test_if_statement passed." << std::endl;
}

void test_if_else_statement() {
    std::string source = "fn main(): void { if (true) { let a = 1; } else { let b = 2; } }";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    assert(stmts.size() == 1);
    auto funcStmt = std::dynamic_pointer_cast<Chtholly::FunctionStmt>(stmts[0]);
    assert(funcStmt != nullptr);
    assert(funcStmt->body->statements.size() == 1);
    auto ifStmt = std::dynamic_pointer_cast<Chtholly::IfStmt>(funcStmt->body->statements[0]);
    assert(ifStmt != nullptr);
    assert(ifStmt->elseBranch != nullptr);
    std::cout << "test_if_else_statement passed." << std::endl;
}

void test_semantic_analysis_of_if_statement() {
    std::string source = "fn main(): void { let x = 10; if (x > 5) { let a = 1; } }";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    Chtholly::SemanticAnalyzer semanticAnalyzer;
    try {
        semanticAnalyzer.analyze(stmts);
        std::cout << "test_semantic_analysis_of_if_statement passed." << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "test_semantic_analysis_of_if_statement failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_semantic_analysis_of_if_else_statement() {
    std::string source = "fn main(): void { let x = 10; if (x > 5) { let a = 1; } else { let b = 2; } }";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();
    Chtholly::SemanticAnalyzer semanticAnalyzer;
    try {
        semanticAnalyzer.analyze(stmts);
        std::cout << "test_semantic_analysis_of_if_else_statement passed." << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "test_semantic_analysis_of_if_else_statement failed: " << e.what() << std::endl;
        assert(false);
    }
}

int main() {
    test_if_statement();
    test_if_else_statement();
    test_semantic_analysis_of_if_statement();
    test_semantic_analysis_of_if_else_statement();
    return 0;
}
