#include "Lexer.h"
#include <unordered_map>
#include <cctype>

namespace Chtholly {

static std::unordered_map<std::string, TokenType> keywords = {
    {"fn", TokenType::Fn},
    {"let", TokenType::Let},
    {"mut", TokenType::Mut},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"while", TokenType::While},
    {"for", TokenType::For},
    {"do", TokenType::Do},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"class", TokenType::Class},
    {"struct", TokenType::Struct},
    {"enum", TokenType::Enum},
    {"public", TokenType::Public},
    {"private", TokenType::Private},
    {"return", TokenType::Return},
    {"import", TokenType::Import},
    {"use", TokenType::Use},
    {"as", TokenType::As},
    {"package", TokenType::Package},
    {"const", TokenType::Const}
};

Lexer::Lexer(const std::string& source)
    : source(source), current(0), line(1), column(1), start_line(1), start_column(1) {}

Token Lexer::nextToken() {
    skipWhitespace();

    start_line = line;
    start_column = column;

    if (current >= source.length()) {
        return makeToken(TokenType::EndOfFile, "");
    }

    char c = peek();

    if (isalpha(c) || c == '_') {
        return identifier();
    }

    if (isdigit(c)) {
        return number();
    }

    if (c == '"') {
        return stringLiteral();
    }

    if (c == '\'') {
        return charLiteral();
    }

    advance();

    switch (c) {
        case '+':
            if (peek() == '+') { advance(); return makeToken(TokenType::PlusPlus, "++"); }
            return makeToken(TokenType::Plus, "+");
        case '-':
            if (peek() == '-') { advance(); return makeToken(TokenType::MinusMinus, "--"); }
            return makeToken(TokenType::Minus, "-");
        case '*': return makeToken(TokenType::Star, "*");
        case '/': return makeToken(TokenType::Slash, "/");
        case '%': return makeToken(TokenType::Percent, "%");
        case '=':
            if (peek() == '=') { advance(); return makeToken(TokenType::EqualEqual, "=="); }
            return makeToken(TokenType::Equal, "=");
        case '!':
            if (peek() == '=') { advance(); return makeToken(TokenType::BangEqual, "!="); }
            return makeToken(TokenType::Bang, "!");
        case '<':
            if (peek() == '=') { advance(); return makeToken(TokenType::LessEqual, "<="); }
            if (peek() == '<') { advance(); return makeToken(TokenType::LeftShift, "<<"); }
            return makeToken(TokenType::Less, "<");
        case '>':
            if (peek() == '=') { advance(); return makeToken(TokenType::GreaterEqual, ">="); }
            if (peek() == '>') { advance(); return makeToken(TokenType::RightShift, ">>"); }
            return makeToken(TokenType::Greater, ">");
        case '&':
            if (peek() == '&') { advance(); return makeToken(TokenType::AmpersandAmpersand, "&&"); }
            return makeToken(TokenType::Ampersand, "&");
        case '|':
            if (peek() == '|') { advance(); return makeToken(TokenType::PipePipe, "||"); }
            return makeToken(TokenType::Pipe, "|");
        case '^': return makeToken(TokenType::Caret, "^");
        case '~': return makeToken(TokenType::Tilde, "~");
        case '(': return makeToken(TokenType::LeftParen, "(");
        case ')': return makeToken(TokenType::RightParen, ")");
        case '{': return makeToken(TokenType::LeftBrace, "{");
        case '}': return makeToken(TokenType::RightBrace, "}");
        case '[': return makeToken(TokenType::LeftBracket, "[");
        case ']': return makeToken(TokenType::RightBracket, "]");
        case ',': return makeToken(TokenType::Comma, ",");
        case '.': return makeToken(TokenType::Dot, ".");
        case ':': return makeToken(TokenType::Colon, ":");
        case ';': return makeToken(TokenType::Semicolon, ";");
        default:
            return makeToken(TokenType::Unknown, std::string(1, c));
    }
}

void Lexer::skipWhitespace() {
    while (current < source.length() && isspace(source[current])) {
        if (source[current] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        current++;
    }
}

Token Lexer::identifier() {
    int start = current;
    while (current < source.length() && (isalnum(peek()) || peek() == '_')) {
        advance();
    }
    std::string value = source.substr(start, current - start);
    if (keywords.count(value)) {
        return makeToken(keywords[value], value);
    }
    return makeToken(TokenType::Identifier, value);
}

Token Lexer::number() {
    int start = current;
    while (current < source.length() && isdigit(peek())) {
        advance();
    }
    if (peek() == '.') {
        advance();
        while (current < source.length() && isdigit(peek())) {
            advance();
        }
        return makeToken(TokenType::Float, source.substr(start, current - start));
    }
    return makeToken(TokenType::Integer, source.substr(start, current - start));
}

Token Lexer::stringLiteral() {
    advance(); // Consume the opening "
    int start = current;
    while (current < source.length() && peek() != '"') {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }
    std::string value = source.substr(start, current - start);
    advance(); // Consume the closing "
    return makeToken(TokenType::String, value);
}

Token Lexer::charLiteral() {
    advance(); // Consume the opening '
    char value = advance();
    advance(); // Consume the closing '
    return makeToken(TokenType::Char, std::string(1, value));
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return {type, value, start_line, start_column};
}

char Lexer::peek() {
    if (current >= source.length()) {
        return '\0';
    }
    return source[current];
}

char Lexer::advance() {
    if (current < source.length()) {
        if (source[current] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        current++;
    }
    return source[current - 1];
}

} // namespace Chtholly
