#include "Lexer.h"
#include <cctype>
#include <unordered_map>

namespace Chtholly {

static const std::unordered_map<std::string, TokenType> keywords = {
    {"fn", TokenType::Fn},
    {"main", TokenType::Main},
    {"let", TokenType::Let},
    {"mut", TokenType::Mut},
    {"class", TokenType::Class},
    {"struct", TokenType::Struct},
    {"enum", TokenType::Enum},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"for", TokenType::For},
    {"while", TokenType::While},
    {"do", TokenType::Do},
    {"break", TokenType::Break},
    {"continue", TokenType::Continue},
    {"fallthrough", TokenType::Fallthrough},
    {"return", TokenType::Return},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"import", TokenType::Import},
    {"use", TokenType::Use},
    {"as", TokenType::As},
    {"package", TokenType::Package},
    {"public", TokenType::Public},
    {"private", TokenType::Private},
    {"const", TokenType::Const},
    {"i8", TokenType::I8},
    {"i16", TokenType::I16},
    {"i32", TokenType::I32},
    {"i64", TokenType::I64},
    {"u8", TokenType::U8},
    {"u16", TokenType::U16},
    {"u32", TokenType::U32},
    {"u64", TokenType::U64},
    {"f32", TokenType::F32},
    {"f64", TokenType::F64},
    {"char", TokenType::Char},
    {"void", TokenType::Void},
    {"bool", TokenType::Bool},
    {"string", TokenType::String}
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::Eof);
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespaceAndComments();
    start = current;

    if (current >= source.length()) {
        return {TokenType::Eof, "", line, (int)start};
    }

    char c = advance();

    if (isalpha(c) || c == '_') {
        return identifier();
    }

    if (isdigit(c)) {
        return number();
    }

    switch (c) {
        case '+': return {peek() == '+' ? (advance(), TokenType::PlusPlus) : (peek() == '=' ? (advance(), TokenType::PlusEqual) : TokenType::Plus), source.substr(start, current - start), line, (int)start};
        case '-': return {peek() == '-' ? (advance(), TokenType::MinusMinus) : (peek() == '=' ? (advance(), TokenType::MinusEqual) : TokenType::Minus), source.substr(start, current - start), line, (int)start};
        case '*': return {peek() == '=' ? (advance(), TokenType::StarEqual) : TokenType::Star, source.substr(start, current - start), line, (int)start};
        case '/': return {peek() == '=' ? (advance(), TokenType::SlashEqual) : TokenType::Slash, source.substr(start, current - start), line, (int)start};
        case '%': return {peek() == '=' ? (advance(), TokenType::PercentEqual) : TokenType::Percent, source.substr(start, current - start), line, (int)start};
        case '=': return {peek() == '=' ? (advance(), TokenType::EqualEqual) : TokenType::Equal, source.substr(start, current - start), line, (int)start};
        case '!': return {peek() == '=' ? (advance(), TokenType::NotEqual) : TokenType::Not, source.substr(start, current - start), line, (int)start};
        case '>': return {peek() == '=' ? (advance(), TokenType::GreaterEqual) : (peek() == '>' ? (advance(), TokenType::GreaterGreater) : TokenType::Greater), source.substr(start, current - start), line, (int)start};
        case '<': return {peek() == '=' ? (advance(), TokenType::LessEqual) : (peek() == '<' ? (advance(), TokenType::LessLess) : TokenType::Less), source.substr(start, current - start), line, (int)start};
        case '&': return {peek() == '&' ? (advance(), TokenType::AmpAmp) : TokenType::Amp, source.substr(start, current - start), line, (int)start};
        case '|': return {peek() == '|' ? (advance(), TokenType::PipePipe) : TokenType::Pipe, source.substr(start, current - start), line, (int)start};
        case '^': return {TokenType::Caret, "^", line, (int)start};
        case '~': return {TokenType::Tilde, "~", line, (int)start};
        case '?': return {TokenType::Question, "?", line, (int)start};
        case '(': return {TokenType::LParen, "(", line, (int)start};
        case ')': return {TokenType::RParen, ")", line, (int)start};
        case '{': return {TokenType::LBrace, "{", line, (int)start};
        case '}': return {TokenType::RBrace, "}", line, (int)start};
        case '[': return {TokenType::LBracket, "[", line, (int)start};
        case ']': return {TokenType::RBracket, "]", line, (int)start};
        case ':': return {TokenType::Colon, ":", line, (int)start};
        case ';': return {TokenType::Semicolon, ";", line, (int)start};
        case ',': return {TokenType::Comma, ",", line, (int)start};
        case '.': return {TokenType::Dot, ".", line, (int)start};
        case '"': return stringLiteral();
        case '\'': return characterLiteral();
    }

    return {TokenType::Unknown, source.substr(start, 1), line, (int)start};
}

char Lexer::peek() {
    if (current >= source.length()) {
        return '\0';
    }
    return source[current];
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::skipWhitespaceAndComments() {
    while (current < source.length()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                advance();
                break;
            case '/':
                if (current + 1 < source.length() && source[current + 1] == '/') {
                    while (peek() != '\n' && current < source.length()) {
                        advance();
                    }
                } else if (current + 1 < source.length() && source[current + 1] == '*') {
                    advance(); // Consume '/'
                    advance(); // Consume '*'
                    while (current < source.length() && (peek() != '*' || (current + 1 < source.length() && source[current + 1] != '/'))) {
                        if (peek() == '\n') {
                            line++;
                        }
                        advance();
                    }
                    if (current < source.length()) {
                        advance(); // Consume '*'
                        advance(); // Consume '/'
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return {it->second, text, line, (int)start};
    }
    return {TokenType::Identifier, text, line, (int)start};
}

Token Lexer::number() {
    bool isFloat = false;
    while (isdigit(peek())) {
        advance();
    }
    if (peek() == '.' && isdigit(source[current + 1])) {
        isFloat = true;
        advance();
        while (isdigit(peek())) {
            advance();
        }
    }
    return {isFloat ? TokenType::Float : TokenType::Integer, source.substr(start, current - start), line, (int)start};
}

Token Lexer::stringLiteral() {
    while (peek() != '"' && current < source.length()) {
        if (peek() == '\n') {
            line++;
        }
        advance();
    }
    if (current >= source.length()) {
        // Unterminated string
        return {TokenType::Unknown, "Unterminated string", line, (int)start};
    }
    advance(); // Closing quote
    return {TokenType::StringLiteral, source.substr(start + 1, current - start - 2), line, (int)start};
}

Token Lexer::characterLiteral() {
    if (peek() != '\'' && current < source.length()) {
        advance();
    }
    if (peek() != '\'' || current >= source.length()) {
        // Unterminated or invalid char literal
        return {TokenType::Unknown, "Invalid character literal", line, (int)start};
    }
    advance(); // Closing quote
    return {TokenType::Character, source.substr(start + 1, 1), line, (int)start};
}

} // namespace Chtholly
