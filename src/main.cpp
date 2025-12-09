#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "SemanticAnalyzer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Token.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::unique_ptr<BlockStmtAST> ast;
    try {
        ast = parser.parse();
    } catch (const std::exception& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return 1;
    }

    if (ast) {
        try {
            SemanticAnalyzer analyzer;
            analyzer.analyze(*ast);
            std::cout << "Semantic analysis passed." << std::endl;
            // ast->print(); // Optionally print AST on success
        } catch (const std::exception& e) {
            std::cerr << "Semantic error: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}
