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
        m_identifier = m_lastChar;
        const char* idStart = m_bufferPtr - 1;
        while (isalnum(*m_bufferPtr)) {
            m_bufferPtr++;
        }
        m_identifier.assign(idStart, m_bufferPtr - idStart);

        m_lastChar = *m_bufferPtr++;

        if (m_identifier == "fn") return Token::Fn;
        if (m_identifier == "let") return Token::Let;
        return Token::Identifier;
    }

    // Number: [0-9.]+
    if (isdigit(m_lastChar) || m_lastChar == '.') {
        const char* numStart = m_bufferPtr - 1;
        while (isdigit(*m_bufferPtr) || *m_bufferPtr == '.') {
            m_bufferPtr++;
        }

        char* endPtr;
        m_number = strtod(numStart, &endPtr);

        if (endPtr == numStart) {
            return Token::Unknown; // No valid number found
        }

        m_lastChar = *m_bufferPtr++;

        return Token::Number;
    }

    // Comments: //
    if (m_lastChar == '/') {
        if (*m_bufferPtr == '/') {
            // This is a single-line comment.
            // Consume the rest of the line.
            do {
                m_lastChar = *m_bufferPtr++;
            } while (m_lastChar != '\n' && m_lastChar != '\r' && m_lastChar != 0);

            // If we're not at the end of the file, recurse to get the next token.
            if (m_lastChar != 0) {
                return getNextToken();
            }
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
        default: return Token::Unknown;
    }
}

} // namespace chtholly
