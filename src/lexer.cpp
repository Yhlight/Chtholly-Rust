#include "lexer.h"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(const std::string& code) : code_(code), pos_(0) {}

Token Lexer::next_token() {
    skip_whitespace();

    if (pos_ >= code_.length()) {
        return make_token(TokenType::END_OF_FILE, "");
    }

    char current_char = advance();

    if (isalpha(current_char)) {
        std::string literal;
        literal += current_char;
        while (pos_ < code_.length() && isalnum(peek())) {
            literal += advance();
        }

        static const std::unordered_map<std::string, TokenType> keywords = {
            {"fn", TokenType::FN},
            {"let", TokenType::LET},
            {"mut", TokenType::MUT},
        };

        if (keywords.count(literal)) {
            return make_token(keywords.at(literal), literal);
        }
        return make_token(TokenType::IDENTIFIER, literal);
    }

    if (isdigit(current_char)) {
        std::string literal;
        literal += current_char;
        while (pos_ < code_.length() && isdigit(peek())) {
            literal += advance();
        }
        return make_token(TokenType::INTEGER, literal);
    }

    switch (current_char) {
        case '=': return make_token(TokenType::ASSIGN, "=");
        case '+': return make_token(TokenType::PLUS, "+");
        case '-': return make_token(TokenType::MINUS, "-");
        case '*': return make_token(TokenType::ASTERISK, "*");
        case '/': return make_token(TokenType::SLASH, "/");
        case '(': return make_token(TokenType::LPAREN, "(");
        case ')': return make_token(TokenType::RPAREN, ")");
        case '{': return make_token(TokenType::LBRACE, "{");
        case '}': return make_token(TokenType::RBRACE, "}");
        case '[': return make_token(TokenType::LBRACKET, "[");
        case ']': return make_token(TokenType::RBRACKET, "]");
        case ',': return make_token(TokenType::COMMA, ",");
        case ':': return make_token(TokenType::COLON, ":");
        case ';': return make_token(TokenType::SEMICOLON, ";");
        default:  return make_token(TokenType::ILLEGAL, std::string(1, current_char));
    }
}

void Lexer::skip_whitespace() {
    while (pos_ < code_.length() && isspace(code_[pos_])) {
        pos_++;
    }
}

char Lexer::peek() {
    if (pos_ >= code_.length()) {
        return '\0';
    }
    return code_[pos_];
}

char Lexer::advance() {
    return code_[pos_++];
}

Token Lexer::make_token(TokenType type, const std::string& literal) {
    return Token{type, literal};
}
