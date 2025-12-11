#include "Lexer.h"
#include <unordered_map>

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;

        char c = advance();

        if (isalpha(c)) {
            tokens.push_back(identifier());
            continue;
        }

        if (isdigit(c)) {
            tokens.push_back(number());
            continue;
        }

        switch (c) {
            case '(': tokens.push_back(makeToken(TokenType::LPAREN, "(")); break;
            case ')': tokens.push_back(makeToken(TokenType::RPAREN, ")")); break;
            case '{': tokens.push_back(makeToken(TokenType::LBRACE, "{")); break;
            case '}': tokens.push_back(makeToken(TokenType::RBRACE, "}")); break;
            case '[': tokens.push_back(makeToken(TokenType::LBRACKET, "[")); break;
            case ']': tokens.push_back(makeToken(TokenType::RBRACKET, "]")); break;
            case ',': tokens.push_back(makeToken(TokenType::COMMA, ",")); break;
            case '.': tokens.push_back(makeToken(TokenType::DOT, ".")); break;
            case ':': tokens.push_back(makeToken(TokenType::COLON, ":")); break;
            case ';': tokens.push_back(makeToken(TokenType::SEMICOLON, ";")); break;
            case '+':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::PLUS_EQUAL, "+="));
                } else if (peek() == '+') {
                    advance();
                    tokens.push_back(makeToken(TokenType::PLUS_PLUS, "++"));
                } else {
                    tokens.push_back(makeToken(TokenType::PLUS, "+"));
                }
                break;
            case '-':
                 if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::MINUS_EQUAL, "-="));
                } else if (peek() == '-') {
                    advance();
                    tokens.push_back(makeToken(TokenType::MINUS_MINUS, "--"));
                } else {
                    tokens.push_back(makeToken(TokenType::MINUS, "-"));
                }
                break;
            case '*':
                 if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::STAR_EQUAL, "*="));
                } else {
                    tokens.push_back(makeToken(TokenType::STAR, "*"));
                }
                break;
            case '/':
                if (peek() == '/') {
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else if (peek() == '*') {
                    advance();
                    while (!isAtEnd()) {
                        if (peek() == '*' && peekNext() == '/') {
                            advance();
                            advance();
                            break;
                        }
                        advance();
                    }
                }
                else if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::SLASH_EQUAL, "/="));
                }
                else {
                    tokens.push_back(makeToken(TokenType::SLASH, "/"));
                }
                break;
            case '%':
                 if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::PERCENT_EQUAL, "%="));
                } else {
                    tokens.push_back(makeToken(TokenType::PERCENT, "%"));
                }
                break;
            case '=':
                tokens.push_back(peek() == '=' ? makeToken(TokenType::EQUAL_EQUAL, "==") : makeToken(TokenType::EQUAL, "="));
                if (peek() == '=') advance();
                break;
            case '!':
                tokens.push_back(peek() == '=' ? makeToken(TokenType::BANG_EQUAL, "!=") : makeToken(TokenType::BANG, "!"));
                if (peek() == '=') advance();
                break;
            case '>':
                tokens.push_back(peek() == '=' ? makeToken(TokenType::GREATER_EQUAL, ">=") : makeToken(TokenType::GREATER, ">"));
                if (peek() == '=') advance();
                break;
            case '<':
                tokens.push_back(peek() == '=' ? makeToken(TokenType::LESS_EQUAL, "<=") : makeToken(TokenType::LESS, "<"));
                if (peek() == '=') advance();
                break;
            case '&':
                tokens.push_back(peek() == '&' ? makeToken(TokenType::AMPERSAND_AMPERSAND, "&&") : makeToken(TokenType::AMPERSAND, "&"));
                if (peek() == '&') advance();
                break;
            case '|':
                tokens.push_back(peek() == '|' ? makeToken(TokenType::PIPE_PIPE, "||") : makeToken(TokenType::PIPE, "|"));
                if (peek() == '|') advance();
                break;
            case '"': tokens.push_back(stringLiteral()); break;
            case '\'': tokens.push_back(charLiteral()); break;

            default:
                tokens.push_back(makeToken(TokenType::UNKNOWN, std::string(1, c)));
                break;
        }
    }
    tokens.push_back(makeToken(TokenType::END_OF_FILE, ""));
    return tokens;
}

char Lexer::advance() {
    column++;
    return source[current++];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 1;
                advance();
                break;
            default:
                return;
        }
    }
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token{type, value, line, column - (int)value.length()};
}

Token Lexer::identifier() {
    int start = current - 1;
    while (isalnum(peek())) {
        advance();
    }
    std::string value = source.substr(start, current - start);

    std::unordered_map<std::string, TokenType> keywords = {
        {"fn", TokenType::FN}, {"let", TokenType::LET}, {"mut", TokenType::MUT},
        {"if", TokenType::IF}, {"else", TokenType::ELSE}, {"switch", TokenType::SWITCH},
        {"case", TokenType::CASE}, {"while", TokenType::WHILE}, {"for", TokenType::FOR},
        {"do", TokenType::DO}, {"return", TokenType::RETURN}, {"class", TokenType::CLASS},
        {"struct", TokenType::STRUCT}, {"enum", TokenType::ENUM}, {"public", TokenType::PUBLIC},
        {"private", TokenType::PRIVATE}, {"const", TokenType::CONST}
    };

    if (keywords.count(value)) {
        return makeToken(keywords[value], value);
    }

    return makeToken(TokenType::IDENTIFIER, value);
}

Token Lexer::number() {
    int start = current - 1;
    while (isdigit(peek())) {
        advance();
    }

    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) {
            advance();
        }
        return makeToken(TokenType::FLOAT, source.substr(start, current - start));
    }

    return makeToken(TokenType::INTEGER, source.substr(start, current - start));
}

Token Lexer::stringLiteral() {
    int start = current;
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        return makeToken(TokenType::UNKNOWN, "Unterminated string.");
    }

    advance(); // Consume the closing quote.
    return makeToken(TokenType::STRING, source.substr(start, current - start - 1));
}

Token Lexer::charLiteral() {
    int start = current;
    if (peek() != '\'' && !isAtEnd()) {
        advance();
    }

    if (peek() != '\'') {
        return makeToken(TokenType::UNKNOWN, "Unterminated char.");
    }
    advance(); // Consume the closing quote.
    return makeToken(TokenType::CHAR, source.substr(start, current - start -1));
}
