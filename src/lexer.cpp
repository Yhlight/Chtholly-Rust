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
            {"true", TokenType::TRUE},
            {"false", TokenType::FALSE},
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"while", TokenType::WHILE},
            {"for", TokenType::FOR},
            {"do", TokenType::DO},
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
        case '=':
            if (peek() == '=') {
                advance();
                return make_token(TokenType::EQ, "==");
            }
            return make_token(TokenType::ASSIGN, "=");
        case '+':
            if (peek() == '+') {
                advance();
                return make_token(TokenType::INC, "++");
            }
            return make_token(TokenType::PLUS, "+");
        case '-':
            if (peek() == '-') {
                advance();
                return make_token(TokenType::DEC, "--");
            }
            return make_token(TokenType::MINUS, "-");
        case '*': return make_token(TokenType::ASTERISK, "*");
        case '/': return make_token(TokenType::SLASH, "/");
        case '!':
            if (peek() == '=') {
                advance();
                return make_token(TokenType::NOT_EQ, "!=");
            }
            return make_token(TokenType::BANG, "!");
        case '<': return make_token(TokenType::LT, "<");
        case '>': return make_token(TokenType::GT, ">");
        case '"': {
            std::string literal;
            while (pos_ < code_.length() && peek() != '"') {
                literal += advance();
            }
            if (peek() == '"') {
                advance(); // consume the closing quote
                return make_token(TokenType::STRING, literal);
            }
            return make_token(TokenType::ILLEGAL, literal); // Unterminated string
        }
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
    while (pos_ < code_.length()) {
        if (isspace(code_[pos_])) {
            pos_++;
        } else if (code_[pos_] == '/' && pos_ + 1 < code_.length()) {
            if (code_[pos_ + 1] == '/') {
                // Single-line comment
                pos_ += 2;
                while (pos_ < code_.length() && code_[pos_] != '\n') {
                    pos_++;
                }
            } else if (code_[pos_ + 1] == '*') {
                // Multi-line comment
                pos_ += 2;
                while (pos_ + 1 < code_.length()) {
                    if (code_[pos_] == '*' && code_[pos_ + 1] == '/') {
                        pos_ += 2;
                        break;
                    }
                    pos_++;
                }
            } else {
                break;
            }
        } else {
            break;
        }
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
