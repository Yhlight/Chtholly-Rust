#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include <string>
#include <vector>

namespace chtholly {

enum class Token {
    // End of file
    Eof,

    // Commands
    Fn,      // fn
    Let,     // let
    Mut,     // mut
    If,      // if
    Else,    // else
    Struct,  // struct

    // Primary
    Identifier,
    Number,

    // Types
    I32,      // i32
    F64,      // f64
    Bool,     // bool
    Char,     // char
    Void,     // void

    // Punctuation
    LParen,   // (
    RParen,   // )
    LBrace,   // {
    RBrace,   // }
    Colon,    // :
    Semicolon,// ;
    Dot,      // .
    Assign,   // =
    Comma,    // ,

    // Unknown character
    Unknown
};

class Lexer {
public:
    Lexer(const char* source);

    Token getNextToken();
    const std::string& getIdentifier() const { return m_identifier; }
    double getNumber() const { return m_number; }

private:
    const char* m_bufferPtr;
    int m_lastChar;
    std::string m_identifier;
    double m_number;
};

} // namespace chtholly

#endif // CHTHOLLY_LEXER_H
