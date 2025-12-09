#include "lexer.h"
#include <iostream>
#include <vector>
#include <cassert>

void test_simple_assignment() {
    std::string source = "let x = 10;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    assert(tokens.size() == 6);
    assert(tokens[0].type == Chtholly::TokenType::LET);
    assert(tokens[1].type == Chtholly::TokenType::IDENTIFIER);
    assert(tokens[1].lexeme == "x");
    assert(tokens[2].type == Chtholly::TokenType::EQUAL);
    assert(tokens[3].type == Chtholly::TokenType::INTEGER);
    assert(std::get<int>(tokens[3].literal) == 10);
    assert(tokens[4].type == Chtholly::TokenType::SEMICOLON);
    assert(tokens[5].type == Chtholly::TokenType::END_OF_FILE);

    std::cout << "test_simple_assignment passed." << std::endl;
}

void test_function_declaration() {
    std::string source = "fn main() { return 0; }";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    assert(tokens.size() == 10);
    assert(tokens[0].type == Chtholly::TokenType::FN);
    assert(tokens[1].type == Chtholly::TokenType::IDENTIFIER);
    assert(tokens[1].lexeme == "main");
    assert(tokens[2].type == Chtholly::TokenType::LEFT_PAREN);
    assert(tokens[3].type == Chtholly::TokenType::RIGHT_PAREN);
    assert(tokens[4].type == Chtholly::TokenType::LEFT_BRACE);
    assert(tokens[5].type == Chtholly::TokenType::RETURN);
    assert(tokens[6].type == Chtholly::TokenType::INTEGER);
    assert(std::get<int>(tokens[6].literal) == 0);
    assert(tokens[7].type == Chtholly::TokenType::SEMICOLON);
    assert(tokens[8].type == Chtholly::TokenType::RIGHT_BRACE);
    assert(tokens[9].type == Chtholly::TokenType::END_OF_FILE);


    std::cout << "test_function_declaration passed." << std::endl;
}

void test_multiline_comment() {
    std::string source = "/* this is a* test */let x = 10;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    assert(tokens.size() == 6);
    assert(tokens[0].type == Chtholly::TokenType::LET);
    assert(tokens[1].type == Chtholly::TokenType::IDENTIFIER);
    assert(tokens[1].lexeme == "x");
    assert(tokens[2].type == Chtholly::TokenType::EQUAL);
    assert(tokens[3].type == Chtholly::TokenType::INTEGER);
    assert(std::get<int>(tokens[3].literal) == 10);
    assert(tokens[4].type == Chtholly::TokenType::SEMICOLON);
    assert(tokens[5].type == Chtholly::TokenType::END_OF_FILE);

    std::cout << "test_multiline_comment passed." << std::endl;
}

void test_out_of_range_integer() {
    std::string source = "let x = 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    assert(tokens.size() == 6);
    assert(tokens[0].type == Chtholly::TokenType::LET);
    assert(tokens[1].type == Chtholly::TokenType::IDENTIFIER);
    assert(tokens[2].type == Chtholly::TokenType::EQUAL);
    assert(tokens[3].type == Chtholly::TokenType::ERROR);
    assert(tokens[4].type == Chtholly::TokenType::SEMICOLON);
    assert(tokens[5].type == Chtholly::TokenType::END_OF_FILE);

    std::cout << "test_out_of_range_integer passed." << std::endl;
}

void test_out_of_range_float() {
    std::string source = "let x = 1.0e1000;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    assert(tokens.size() == 6);
    assert(tokens[0].type == Chtholly::TokenType::LET);
    assert(tokens[1].type == Chtholly::TokenType::IDENTIFIER);
    assert(tokens[2].type == Chtholly::TokenType::EQUAL);
    assert(tokens[3].type == Chtholly::TokenType::ERROR);
    assert(tokens[4].type == Chtholly::TokenType::SEMICOLON);
    assert(tokens[5].type == Chtholly::TokenType::END_OF_FILE);

    std::cout << "test_out_of_range_float passed." << std::endl;
}

void test_unexpected_character() {
    std::string source = "let x = @;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    assert(tokens.size() == 6);
    assert(tokens[0].type == Chtholly::TokenType::LET);
    assert(tokens[1].type == Chtholly::TokenType::IDENTIFIER);
    assert(tokens[2].type == Chtholly::TokenType::EQUAL);
    assert(tokens[3].type == Chtholly::TokenType::ERROR);
    assert(tokens[4].type == Chtholly::TokenType::SEMICOLON);
    assert(tokens[5].type == Chtholly::TokenType::END_OF_FILE);

    std::cout << "test_unexpected_character passed." << std::endl;
}

void test_unterminated_string() {
    std::string source = "\"hello";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    assert(tokens.size() == 2);
    assert(tokens[0].type == Chtholly::TokenType::ERROR);
    assert(tokens[1].type == Chtholly::TokenType::END_OF_FILE);

    std::cout << "test_unterminated_string passed." << std::endl;
}

void test_unterminated_comment() {
    std::string source = "/* hello";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();

    assert(tokens.size() == 2);
    assert(tokens[0].type == Chtholly::TokenType::ERROR);
    assert(tokens[1].type == Chtholly::TokenType::END_OF_FILE);

    std::cout << "test_unterminated_comment passed." << std::endl;
}

int main() {
    test_simple_assignment();
    test_function_declaration();
    test_multiline_comment();
    test_out_of_range_integer();
    test_out_of_range_float();
    test_unexpected_character();
    test_unterminated_string();
    test_unterminated_comment();
    return 0;
}
