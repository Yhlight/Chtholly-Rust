#include <iostream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"
#include "Token.h"

int main() {
    std::string_view source = "(-1 + 2) * 3 / 4";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::unique_ptr<Chtholly::Expr> expr = parser.parse();

    if (expr)
    {
        Chtholly::ASTPrinter printer;
        std::cout << printer.print(*expr) << std::endl;
    }
    else
    {
        std::cout << "Failed to parse expression." << std::endl;
    }


    return 0;
}
