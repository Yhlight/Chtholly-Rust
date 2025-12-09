#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <variant>

namespace Chtholly
{
    enum class TokenType
    {
        // Keywords
        FN, LET, MUT, IF, ELSE, SWITCH, CASE, DEFAULT, FOR, WHILE, DO, BREAK, CONTINUE, RETURN,
        CLASS, STRUCT, ENUM, CONTRACT, REQUIRE, PUBLIC, PRIVATE,
        TRUE, FALSE, SELF,
        FALLTHROUGH,

        // Literals
        IDENTIFIER,
        INTEGER,
        FLOAT,
        STRING,
        CHAR,

        // Operators
        PLUS, MINUS, STAR, SLASH, PERCENT,
        EQUAL, EQUAL_EQUAL, BANG, BANG_EQUAL,
        GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,
        AMPERSAND, AMPERSAND_AMPERSAND, PIPE, PIPE_PIPE,
        CARET, TILDE, LEFT_SHIFT, RIGHT_SHIFT,

        // Assignment
        PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL,

        // Unary
        PLUS_PLUS, MINUS_MINUS,

        // Delimiters
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        LEFT_BRACKET, RIGHT_BRACKET,
        COMMA, DOT, COLON, SEMICOLON,
        ARROW,

        // Misc
        COMMENT,
        NEWLINE,
        END_OF_FILE,
        ERROR,

        // Types
        I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, VOID, BOOL,

        // Misc Keywords
        AS, IMPORT, USE, PACKAGE,
    };

    struct Token
    {
        TokenType type;
        std::string lexeme;
        std::variant<std::monostate, int, double, std::string, char> literal;
        int line;
    };

} // namespace Chtholly

#endif // CHTHOLLY_TOKEN_H
