#include "../src/CodeGenerator.h"
#include "../src/Parser.h"
#include "../src/Lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

void test_code_generator() {
    std::string source = "fn main() { let x = 10; return x + 5; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> ast = parser.parse();
    CodeGenerator generator;
    std::string generated_ir = generator.generate(ast);

    std::ifstream golden_file("../tests/golden.ll");
    std::stringstream buffer;
    buffer << golden_file.rdbuf();
    std::string golden_ir = buffer.str();

    // Trim trailing whitespace from both strings
    generated_ir.erase(generated_ir.find_last_not_of(" \n\r\t")+1);
    golden_ir.erase(golden_ir.find_last_not_of(" \n\r\t")+1);

    assert(generated_ir == golden_ir);

    std::cout << "Code generator test passed!" << std::endl;
}

int main() {
    test_code_generator();
    return 0;
}
