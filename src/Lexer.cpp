#include "Lexer.h"
#include <cctype>
#include <cstdlib>

namespace chtholly {

Lexer::Lexer(const char* source) : m_bufferPtr(source) {
    // Prime the lexer with the first character.
    m_lastChar = *m_bufferPtr++;
}

Token Lexer::getNextToken() {
    // Skip any whitespace.
    while (isspace(m_lastChar)) {
        m_lastChar = *m_bufferPtr++;
    }

    // Identifier: [a-zA-Z][a-zA-Z0-9]*
    if (isalpha(m_lastChar)) {
        std::string identifier;
        identifier += m_lastChar;
        while (isalnum(*m_bufferPtr)) {
            identifier += *m_bufferPtr++;
        }
        m_identifier = identifier;
        m_lastChar = *m_bufferPtr++;

        if (m_identifier == "fn") return Token::Fn;
        if (m_identifier == "let") return Token::Let;
        return Token::Identifier;
    }

    // Number: [0-9.]+
    if (isdigit(m_lastChar) || m_lastChar == '.') {
        std::string numStr;
        const char* numStart = m_bufferPtr - 1;
        while (isdigit(*m_bufferPtr) || *m_bufferPtr == '.') {
            m_bufferPtr++;
        }
        numStr.assign(numStart, m_bufferPtr - numStart);

        char* endPtr;
        m_number = strtod(numStr.c_str(), &endPtr);

        if (endPtr != numStr.c_str() + numStr.length()) {
             // Invalid number, but we'll let the parser handle it for now.
        }

        m_lastChar = *m_bufferPtr++;

        return Token::Number;
    }

    // Comments
    if (m_lastChar == '/') {
        if (*m_bufferPtr == '/') {
            // Single line comment
            do {
                m_lastChar = *m_bufferPtr++;
            } while (m_lastChar != '\n' && m_lastChar != '\r' && m_lastChar != 0);

            if (m_lastChar != 0)
                return getNextToken();
        }
    }

    // Check for end of file.
    if (m_lastChar == 0) {
        return Token::Eof;
    }

    // Otherwise, just return the character as its token value.
    int thisChar = m_lastChar;
    m_lastChar = *m_bufferPtr++;
    switch (thisChar) {
        case '(': return Token::LParen;
        case ')': return Token::RParen;
        case '{': return Token::LBrace;
        case '}': return Token::RBrace;
        case ':': return Token::Colon;
        case ';': return Token::Semicolon;
        case '=': return Token::Assign;
        case ',': return Token::Comma;
        default: return static_cast<Token>(thisChar);
    }
}

} // namespace chtholly
