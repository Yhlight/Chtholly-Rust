#include <iostream>
#include <vector>
#include "Lexer.h"
#include "Token.h"

int main() {
    std::string_view source = "let mut x = 10; // This is a test\nfn main() {}";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::cout << "Token Type: " << Chtholly::to_string(token.type)
                  << ", Lexeme: '" << token.lexeme << "'"
                  << ", Line: " << token.line
                  << ", Column: " << token.column << std::endl;
    }

    return 0;
}
