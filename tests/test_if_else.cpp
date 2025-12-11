#include "../src/Parser.h"
#include "../src/Lexer.h"
#include "../src/SemanticAnalyzer.h"
#include "../src/CodeGenerator.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

void test_parser_if_else() {
    std::string source = "if (x > 5) { return 1; } else { return 0; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> ast = parser.parse();

    assert(ast.size() == 1);
    auto* if_stmt = dynamic_cast<IfStmt*>(ast[0].get());
    assert(if_stmt != nullptr);
    assert(if_stmt->condition != nullptr);
    assert(if_stmt->thenBranch != nullptr);
    assert(if_stmt->elseBranch != nullptr);

    std::cout << "Parser if-else test passed!" << std::endl;
}

void test_codegen_if_else() {
    std::string source = "fn main() { let x = 10; if (x > 5) { return 1; } else { return 0; } }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> ast = parser.parse();
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast);
    CodeGenerator generator;
    std::string generated_ir = generator.generate(ast);

    std::ifstream golden_file("../tests/golden_if_else.ll");
    std::stringstream buffer;
    buffer << golden_file.rdbuf();
    std::string golden_ir = buffer.str();

    // Trim trailing whitespace from both strings
    generated_ir.erase(generated_ir.find_last_not_of(" \n\r\t")+1);
    golden_ir.erase(golden_ir.find_last_not_of(" \n\r\t")+1);

    assert(generated_ir == golden_ir);

    std::cout << "Codegen if-else test passed!" << std::endl;
}

int main() {
    test_parser_if_else();
    test_codegen_if_else();
    return 0;
}
