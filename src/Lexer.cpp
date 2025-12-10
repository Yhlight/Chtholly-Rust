#include "Lexer.h"
#include <cctype>
#include <unordered_map>

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"fn", TokenType::FN}, {"let", TokenType::LET}, {"mut", TokenType::MUT},
    {"if", TokenType::IF}, {"else", TokenType::ELSE}, {"while", TokenType::WHILE},
    {"for", TokenType::FOR}, {"return", TokenType::RETURN}, {"class", TokenType::CLASS},
    {"struct", TokenType::STRUCT}, {"enum", TokenType::ENUM}, {"public", TokenType::PUBLIC},
    {"private", TokenType::PRIVATE}, {"require", TokenType::REQUIRE}, {"import", TokenType::IMPORT},
    {"use", TokenType::USE}, {"as", TokenType::AS}, {"package", TokenType::PACKAGE},
    {"switch", TokenType::SWITCH}, {"case", TokenType::CASE}, {"default", TokenType::DEFAULT},
    {"break", TokenType::BREAK}, {"fallthrough", TokenType::FALLTHROUGH},
    {"do", TokenType::DO}, {"foreach", TokenType::FOREACH},
    {"true", TokenType::TRUE}, {"false", TokenType::FALSE}
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token = nextToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = nextToken();
    }
    tokens.push_back(token); // Add EOF token
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();

    if (current_pos >= source.length()) {
        return Token(TokenType::END_OF_FILE, "", line, column);
    }

    char current_char = peek();

    if (isalpha(current_char) || current_char == '_') {
        return identifier();
    }

    if (isdigit(current_char)) {
        return number();
    }

    if (current_char == '"') {
        return stringLiteral();
    }

    if (current_char == '\'') {
        return charLiteral();
    }

    switch (current_char) {
        case '+':
            advance();
            if (peek() == '+') { advance(); return Token(TokenType::PLUS_PLUS, "++", line, column - 2); }
            if (peek() == '=') { advance(); return Token(TokenType::PLUS_EQUAL, "+=", line, column - 2); }
            return Token(TokenType::PLUS, "+", line, column - 1);
        case '-':
            advance();
            if (peek() == '>') { advance(); return Token(TokenType::ARROW, "->", line, column - 2); }
            if (peek() == '-') { advance(); return Token(TokenType::MINUS_MINUS, "--", line, column - 2); }
            if (peek() == '=') { advance(); return Token(TokenType::MINUS_EQUAL, "-=", line, column - 2); }
            return Token(TokenType::MINUS, "-", line, column - 1);
        case '*':
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::STAR_EQUAL, "*=", line, column - 2); }
            return Token(TokenType::STAR, "*", line, column - 1);
        case '/':
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::SLASH_EQUAL, "/=", line, column - 2); }
            return Token(TokenType::SLASH, "/", line, column - 1);
        case '%':
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::PERCENT_EQUAL, "%=", line, column - 2); }
            return Token(TokenType::PERCENT, "%", line, column - 1);
        case '=':
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::DOUBLE_EQUAL, "==", line, column - 2); }
            return Token(TokenType::EQUAL, "=", line, column - 1);
        case '!':
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::NOT_EQUAL, "!=", line, column - 2); }
            return Token(TokenType::NOT, "!", line, column - 1);
        case '<':
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::LESS_EQUAL, "<=", line, column - 2); }
            if (peek() == '<') { advance(); return Token(TokenType::LEFT_SHIFT, "<<", line, column - 2); }
            return Token(TokenType::LESS, "<", line, column - 1);
        case '>':
            advance();
            if (peek() == '=') { advance(); return Token(TokenType::GREATER_EQUAL, ">=", line, column - 2); }
            if (peek() == '>') { advance(); return Token(TokenType::RIGHT_SHIFT, ">>", line, column - 2); }
            return Token(TokenType::GREATER, ">", line, column - 1);
        case '&':
            advance();
            if (peek() == '&') { advance(); return Token(TokenType::AND, "&&", line, column - 2); }
            return Token(TokenType::AMPERSAND, "&", line, column - 1);
        case '|':
            advance();
            if (peek() == '|') { advance(); return Token(TokenType::OR, "||", line, column - 2); }
            return Token(TokenType::PIPE, "|", line, column - 1);
        case '^': advance(); return Token(TokenType::CARET, "^", line, column - 1);
        case '~': advance(); return Token(TokenType::TILDE, "~", line, column - 1);
        case '(': advance(); return Token(TokenType::LEFT_PAREN, "(", line, column - 1);
        case ')': advance(); return Token(TokenType::RIGHT_PAREN, ")", line, column - 1);
        case '{': advance(); return Token(TokenType::LEFT_BRACE, "{", line, column - 1);
        case '}': advance(); return Token(TokenType::RIGHT_BRACE, "}", line, column - 1);
        case '[': advance(); return Token(TokenType::LEFT_BRACKET, "[", line, column - 1);
        case ']': advance(); return Token(TokenType::RIGHT_BRACKET, "]", line, column - 1);
        case ',': advance(); return Token(TokenType::COMMA, ",", line, column - 1);
        case '.': advance(); return Token(TokenType::DOT, ".", line, column - 1);
        case ':': advance(); return Token(TokenType::COLON, ":", line, column - 1);
        case ';': advance(); return Token(TokenType::SEMICOLON, ";", line, column - 1);
    }

    advance();
    return Token(TokenType::UNKNOWN, std::string(1, current_char), line, column - 1);
}

char Lexer::peek() {
    if (current_pos >= source.length()) {
        return '\0';
    }
    return source[current_pos];
}

char Lexer::advance() {
    char current_char = source[current_pos];
    current_pos++;
    if (current_char == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return current_char;
}

void Lexer::skipWhitespace() {
    while (current_pos < source.length()) {
        if (isspace(peek())) {
            advance();
        } else if (peek() == '/' && current_pos + 1 < source.length() && source[current_pos + 1] == '/') {
            // Skip single-line comments
            while (peek() != '\n' && current_pos < source.length()) {
                advance();
            }
        } else if (peek() == '/' && current_pos + 1 < source.length() && source[current_pos + 1] == '*') {
            // Skip multi-line comments
            advance(); // Consume '/'
            advance(); // Consume '*'
            while (current_pos < source.length()) {
                if (peek() == '*' && current_pos + 1 < source.length() && source[current_pos + 1] == '/') {
                    advance(); // Consume '*'
                    advance(); // Consume '/'
                    break;
                }
                advance();
            }
        }
        else {
            break;
        }
    }
}

Token Lexer::identifier() {
    int start_pos = current_pos;
    int start_col = column;
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }
    std::string value = source.substr(start_pos, current_pos - start_pos);

    auto it = KEYWORDS.find(value);
    if (it != KEYWORDS.end()) {
        return Token(it->second, value, line, start_col);
    }

    return Token(TokenType::IDENTIFIER, value, line, start_col);
}

Token Lexer::number() {
    int start_pos = current_pos;
    int start_col = column;
    bool is_float = false;
    while (isdigit(peek()) || (peek() == '.' && !is_float && (current_pos + 1 < source.length() && isdigit(source[current_pos + 1])))) {
        if (peek() == '.') {
            is_float = true;
        }
        advance();
    }
    std::string value = source.substr(start_pos, current_pos - start_pos);
    return Token(is_float ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL, value, line, start_col);
}

Token Lexer::stringLiteral() {
    int start_col = column;
    advance(); // Consume the opening "
    int start_pos = current_pos;
    while (peek() != '"' && current_pos < source.length()) {
        advance();
    }
    std::string value = source.substr(start_pos, current_pos - start_pos);
    advance(); // Consume the closing "
    return Token(TokenType::STRING_LITERAL, value, line, start_col);
}

Token Lexer::charLiteral() {
    int start_col = column;
    advance(); // Consume the opening '
    char value = advance();
    if (peek() == '\'') {
        advance(); // Consume the closing '
        return Token(TokenType::CHAR_LITERAL, std::string(1, value), line, start_col);
    }
    return Token(TokenType::UNKNOWN, std::string(1, value), line, start_col);
}
