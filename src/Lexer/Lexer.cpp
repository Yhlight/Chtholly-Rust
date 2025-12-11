#include "Lexer.h"
#include <cctype>
#include <map>

Lexer::Lexer(const std::string& source) : source(source), position(0) {}

char Lexer::currentChar() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

void Lexer::advance() {
    position++;
}

void Lexer::skipWhitespace() {
    while (isspace(currentChar())) {
        advance();
    }
}

Token Lexer::integer() {
    std::string value;
    while (isdigit(currentChar())) {
        value += currentChar();
        advance();
    }
    return {TokenType::Integer, value};
}

Token Lexer::identifier() {
    std::string value;
    while (isalnum(currentChar()) || currentChar() == '_') {
        value += currentChar();
        advance();
    }

    static const std::map<std::string, TokenType> keywords = {
        {"fn", TokenType::Fn},
        {"let", TokenType::Let},
        {"mut", TokenType::Mut},
        {"i32", TokenType::Identifier} // Treat i32 as a type identifier
    };

    if (keywords.count(value)) {
        return {keywords.at(value), value};
    }

    return {TokenType::Identifier, value};
}

Token Lexer::nextToken() {
    skipWhitespace();

    char ch = currentChar();

    if (isdigit(ch)) {
        return integer();
    }
    if (isalpha(ch) || ch == '_') {
        return identifier();
    }

    advance();

    switch (ch) {
        case '+': return {TokenType::Plus, "+"};
        case '=': return {TokenType::Assign, "="};
        case ';': return {TokenType::Semicolon, ";"};
        case '(': return {TokenType::LParen, "("};
        case ')': return {TokenType::RParen, ")"};
        case '{': return {TokenType::LBrace, "{"};
        case '}': return {TokenType::RBrace, "}"};
        case ':': return {TokenType::Colon, ":"};
        case '\0': return {TokenType::Eof, ""};
        default: return {TokenType::Unknown, std::string(1, ch)};
    }
}
