#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"
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

    Chtholly::Parser parser(tokens);
    auto stmts = parser.parse();

    Chtholly::ASTPrinter printer;
    for (const auto& stmt : stmts) {
        if (stmt) {
            std::cout << printer.print(*stmt) << std::endl;
        }
    }

    return 0;
}
