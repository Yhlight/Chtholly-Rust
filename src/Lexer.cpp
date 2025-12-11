#include "Lexer.h"
#include <iostream>
#include <unordered_map>

namespace Chtholly
{

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

Lexer::Lexer(std::string_view source)
    : m_source(source)
{
}

std::vector<Token> Lexer::scanTokens()
{
    std::vector<Token> tokens;
    while (!isAtEnd())
    {
        m_start = m_current;
        Token token = scanToken();

        // scanToken can return an empty token if it only finds whitespace/comments
        // so we need to check for that.
        if (token.type == TokenType::Error)
        {
            tokens.push_back(token);
            break;
        }
        if (token.type != TokenType::Eof || token.lexeme.length() > 0)
        {
             tokens.push_back(token);
        }
    }

    m_start = m_current;
    tokens.push_back(makeToken(TokenType::Eof));
    return tokens;
}

Token Lexer::scanToken()
{
    skipWhitespace();
    m_start = m_current;

    if (isAtEnd()) return makeToken(TokenType::Eof);

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c)
    {
        case '(': return makeToken(TokenType::LeftParen);
        case ')': return makeToken(TokenType::RightParen);
        case '{': return makeToken(TokenType::LeftBrace);
        case '}': return makeToken(TokenType::RightBrace);
        case '[': return makeToken(TokenType::LeftBracket);
        case ']': return makeToken(TokenType::RightBracket);
        case ';': return makeToken(TokenType::Semicolon);
        case ':': return makeToken(TokenType::Colon);
        case ',': return makeToken(TokenType::Comma);
        case '.': return makeToken(TokenType::Dot);
        case '~': return makeToken(TokenType::Tilde);
        case '^': return makeToken(TokenType::Caret);
        case '+':
            if (match('=')) return makeToken(TokenType::PlusEqual);
            if (match('+')) return makeToken(TokenType::PlusPlus);
            return makeToken(TokenType::Plus);
        case '-':
            if (match('=')) return makeToken(TokenType::MinusEqual);
            if (match('-')) return makeToken(TokenType::MinusMinus);
            return makeToken(TokenType::Minus);
        case '*':
            return makeToken(match('=') ? TokenType::StarEqual : TokenType::Star);
        case '/':
            return makeToken(match('=') ? TokenType::SlashEqual : TokenType::Slash);
        case '%':
            return makeToken(match('=') ? TokenType::PercentEqual : TokenType::Percent);
        case '!':
            return makeToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
        case '=':
            return makeToken(match('=') ? TokenType::EqualEqual : TokenType::Equal);
        case '<':
            return makeToken(match('=') ? TokenType::LessEqual : match('<') ? TokenType::LessLess : TokenType::Less);
        case '>':
            return makeToken(match('=') ? TokenType::GreaterEqual : match('>') ? TokenType::GreaterGreater : TokenType::Greater);
        case '&':
            return makeToken(match('&') ? TokenType::AmpAmp : TokenType::Amp);
        case '|':
            return makeToken(match('|') ? TokenType::PipePipe : TokenType::Pipe);
        case '"': return string();
        case '\'': return makeToken(TokenType::Character); // Simplified for now.
    }

    return errorToken("Unexpected character.");
}

char Lexer::advance()
{
    m_current++;
    m_column++;
    return m_source[m_current - 1];
}

bool Lexer::isAtEnd()
{
    return m_current >= m_source.length();
}

bool Lexer::match(char expected)
{
    if (isAtEnd()) return false;
    if (m_source[m_current] != expected) return false;
    m_current++;
    m_column++;
    return true;
}

char Lexer::peek()
{
    if (isAtEnd()) return '\0';
    return m_source[m_current];
}

char Lexer::peekNext()
{
    if (m_current + 1 >= m_source.length()) return '\0';
    return m_source[m_current + 1];
}

void Lexer::skipWhitespace()
{
    for (;;)
    {
        char c = peek();
        switch (c)
        {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                m_line++;
                m_column = 0;
                advance();
                break;
            case '/':
                if (peekNext() == '/')
                {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd())
                    {
                        advance();
                    }
                }
                else if (peekNext() == '*')
                {
                    advance(); // Consume '/'.
                    advance(); // Consume '*'.

                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd())
                    {
                        if (peek() == '\n')
                        {
                            m_line++;
                            m_column = 0;
                        }
                        advance();
                    }

                    if (!isAtEnd()) advance(); // Consume '*'.
                    if (!isAtEnd()) advance(); // Consume '/'.
                }
                else
                {
                    return;
                }
                break;
            default:
                return;
        }
    }
}


Token Lexer::makeToken(TokenType type) const
{
    return Token{ type, m_source.substr(m_start, m_current - m_start), m_line, m_column };
}

Token Lexer::errorToken(const char* message) const
{
    return Token{ TokenType::Error, message, m_line, m_column };
}

Token Lexer::string()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n')
        {
            m_line++;
            m_column = 0;
        }
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    // The closing ".
    advance();
    return makeToken(TokenType::String);
}

Token Lexer::number()
{
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext()))
    {
        // Consume the ".".
        advance();
        while (isDigit(peek())) advance();
    }

    return makeToken(TokenType::Number);
}

Token Lexer::identifier()
{
    while (isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifierType());
}

TokenType Lexer::identifierType()
{
    switch (m_source[m_start])
    {
        case 'a':
            if (m_current - m_start > 1)
            {
                switch (m_source[m_start + 1])
                {
                    case 'n': return checkKeyword(2, 1, "d", TokenType::And);
                    case 's': return checkKeyword(2, 0, "", TokenType::As);
                }
            }
            break;
        case 'c':
            if (m_current - m_start > 1)
            {
                 switch (m_source[m_start + 1])
                {
                    case 'l': return checkKeyword(2, 3, "ass", TokenType::Class);
                    case 'a': return checkKeyword(2, 2, "se", TokenType::Case);
                    case 'o': return checkKeyword(2, 3, "nst", TokenType::Const);
                }
            }
            break;
        case 'e':
            if (m_current - m_start > 1)
            {
                switch (m_source[m_start + 1])
                {
                    case 'l': return checkKeyword(2, 2, "se", TokenType::Else);
                    case 'n': return checkKeyword(2, 2, "um", TokenType::Enum);
                }
            }
            break;
        case 'f':
            if (m_current - m_start > 1)
            {
                switch (m_source[m_start + 1])
                {
                    case 'a': // false, fallthrough
                        if (m_current - m_start > 3 && m_source[m_start + 3] == 's')
                            return checkKeyword(2, 3, "lse", TokenType::False);
                        return checkKeyword(2, 9, "llthrough", TokenType::Fallthrough);
                    case 'o': return checkKeyword(2, 1, "r", TokenType::For);
                    case 'n': return checkKeyword(2, 0, "", TokenType::Fn);
                }
            }
            break;
        case 'i':
            if (m_current - m_start > 1)
            {
                switch (m_source[m_start + 1])
                {
                    case 'f': return checkKeyword(2, 0, "", TokenType::If);
                    case 'm': return checkKeyword(2, 4, "port", TokenType::Import);
                }
            }
            break;
        case 'l': return checkKeyword(1, 2, "et", TokenType::Let);
        case 'm': return checkKeyword(1, 2, "ut", TokenType::Mut);
        case 'p':
            if (m_current - m_start > 1) {
                switch (m_source[m_start + 1]) {
                    case 'r': // print, private
                        if (m_current - m_start > 3) {
                            switch (m_source[m_start + 3]) {
                                case 'n': return checkKeyword(4, 1, "t", TokenType::Print);
                                case 'v': return checkKeyword(4, 3, "ate", TokenType::Private);
                            }
                        }
                        break;
                    case 'u': return checkKeyword(2, 4, "blic", TokenType::Public);
                    case 'a': return checkKeyword(2, 5, "ckage", TokenType::Package);
                }
            }
            break;
        case 'r': return checkKeyword(1, 5, "eturn", TokenType::Return);
        case 's':
            if (m_current - m_start > 1)
            {
                switch (m_source[m_start + 1])
                {
                    case 'u': return checkKeyword(2, 3, "per", TokenType::Super);
                    case 'e': return checkKeyword(2, 2, "lf", TokenType::Self);
                    case 't': return checkKeyword(2, 4, "ruct", TokenType::Struct);
                    case 'w': return checkKeyword(2, 4, "itch", TokenType::Switch);
                }
            }
            break;
        case 't':
            if (m_current - m_start > 1)
            {
                switch (m_source[m_start + 1])
                {
                    case 'h': return checkKeyword(2, 2, "is", TokenType::This);
                    case 'r': return checkKeyword(2, 2, "ue", TokenType::True);
                }
            }
            break;
        case 'u': return checkKeyword(1, 2, "se", TokenType::Use);
        case 'w': return checkKeyword(1, 4, "hile", TokenType::While);
    }

    return TokenType::Identifier;
}

TokenType Lexer::checkKeyword(int start, int length, const char* rest, TokenType type)
{
    if (m_current - m_start == start + length && m_source.substr(m_start + start, length) == rest)
    {
        return type;
    }

    return TokenType::Identifier;
}


} // namespace Chtholly
