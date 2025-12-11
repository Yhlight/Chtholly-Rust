#include "../src/SemanticAnalyzer.h"
#include "../src/Parser.h"
#include "../src/Lexer.h"
#include <cassert>
#include <iostream>
#include <vector>

void test_semantic_analyzer_success() {
    std::string source = "fn main() { let x = 10; return x; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> ast = parser.parse();
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast);
    std::cout << "Semantic analyzer success test passed!" << std::endl;
}

void test_semantic_analyzer_fail() {
    std::string source = "fn main() { return y; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> ast = parser.parse();
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(ast);
        assert(false); // Should have thrown an exception
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "Undefined variable 'y'.");
        std::cout << "Semantic analyzer fail test passed!" << std::endl;
    }
}


int main() {
    test_semantic_analyzer_success();
    test_semantic_analyzer_fail();
    return 0;
}
