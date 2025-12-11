#include "../src/Lexer.h"
#include <cassert>
#include <iostream>
#include <vector>

void test_lexer() {
    std::string source = "let x = 10; // this is a comment\nfn main() { return x; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    assert(tokens.size() == 15);
    assert(tokens[0].type == TokenType::LET);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[1].value == "x");
    assert(tokens[2].type == TokenType::EQUAL);
    assert(tokens[3].type == TokenType::INTEGER);
    assert(tokens[3].value == "10");
    assert(tokens[4].type == TokenType::SEMICOLON);
    assert(tokens[5].type == TokenType::FN);
    assert(tokens[6].type == TokenType::IDENTIFIER);
    assert(tokens[6].value == "main");
    assert(tokens[7].type == TokenType::LPAREN);
    assert(tokens[8].type == TokenType::RPAREN);
    assert(tokens[9].type == TokenType::LBRACE);
    assert(tokens[10].type == TokenType::RETURN);
    assert(tokens[11].type == TokenType::IDENTIFIER);
    assert(tokens[11].value == "x");
    assert(tokens[12].type == TokenType::SEMICOLON);
    assert(tokens[13].type == TokenType::RBRACE);
    assert(tokens[14].type == TokenType::END_OF_FILE);

    std::cout << "Lexer test passed!" << std::endl;
}

int main() {
    test_lexer();
    return 0;
}
