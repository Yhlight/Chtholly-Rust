#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer.h"
#include "parser.h"
#include "sema.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();

    Chtholly::Sema sema;
    sema.analyze(statements);

    return 0;
}
