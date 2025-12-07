#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Lexer.h"
#include "Token.h"

int main(int argc, char* argv[]) {
    std::string path = "examples/test.cns";
    if (argc > 1) {
        path = argv[1];
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::cout << "Type: " << Chtholly::tokenTypeToString(token.type)
                  << ", Lexeme: '" << token.lexeme << "'"
                  << ", Line: " << token.line << std::endl;
    }

    return 0;
}
