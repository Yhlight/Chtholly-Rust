#include "lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "token.h"

// Function to convert TokenType to string for readability
std::string tokenTypeToString(Chtholly::TokenType type) {
    switch (type) {
        case Chtholly::TokenType::LET: return "LET";
        case Chtholly::TokenType::MUT: return "MUT";
        case Chtholly::TokenType::IDENTIFIER: return "IDENTIFIER";
        case Chtholly::TokenType::COLON: return "COLON";
        case Chtholly::TokenType::EQUAL: return "EQUAL";
        case Chtholly::TokenType::INTEGER: return "INTEGER";
        case Chtholly::TokenType::SEMICOLON: return "SEMICOLON";
        case Chtholly::TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
    }
}

int main() {
    std::ifstream file("tests/ownership.cns");
    if (!file) {
        std::cerr << "Could not open file: tests/ownership.cns" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::cout << "Type: " << tokenTypeToString(token.type) << " (" << token.lexeme << ")" << std::endl;
    }

    return 0;
}
