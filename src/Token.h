#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <vector>

namespace Chtholly {

enum class TokenType {
  // Single-character tokens
  LEFT_PAREN, RIGHT_PAREN,
  LEFT_BRACE, RIGHT_BRACE,
  LEFT_BRACKET, RIGHT_BRACKET,
  COMMA, DOT, MINUS, PLUS,
  SEMICOLON, STAR, SLASH,
  MODULO,

  // One or two character tokens
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,
  PLUS_EQUAL, MINUS_EQUAL,
  STAR_EQUAL, SLASH_EQUAL,
  MODULO_EQUAL,
  PLUS_PLUS, MINUS_MINUS,
  AMPERSAND, AMPERSAND_AMPERSAND,
  PIPE, PIPE_PIPE,
  CARET, TILDE,
  LEFT_SHIFT, RIGHT_SHIFT,
  COLON, COLON_COLON,
  QUESTION,

  // Literals
  IDENTIFIER, STRING, NUMBER, CHARACTER,

  // Keywords
  FN, LET, MUT, CLASS, STRUCT, ENUM,
  IF, ELSE, SWITCH, CASE, FALLTHROUGH,
  WHILE, FOR, FOREACH, DO, RETURN,
  PUBLIC, PRIVATE, IMPORT, USE, AS,
  PACKAGE, REQUIRE, TYPE, SELF,
  TRUE, FALSE,

  // Special
  COMMENT,
  END_OF_FILE
};

// Helper function to convert TokenType to string for debugging
inline std::string tokenTypeToString(TokenType type) {
    static const char* const names[] = {
        "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
        "LEFT_BRACKET", "RIGHT_BRACKET", "COMMA", "DOT", "MINUS",
        "PLUS", "SEMICOLON", "STAR", "SLASH", "MODULO", "BANG",
        "BANG_EQUAL", "EQUAL", "EQUAL_EQUAL", "GREATER", "GREATER_EQUAL",
        "LESS", "LESS_EQUAL", "PLUS_EQUAL", "MINUS_EQUAL", "STAR_EQUAL",
        "SLASH_EQUAL", "MODULO_EQUAL", "PLUS_PLUS", "MINUS_MINUS",
        "AMPERSAND", "AMPERSAND_AMPERSAND", "PIPE", "PIPE_PIPE", "CARET",
        "TILDE", "LEFT_SHIFT", "RIGHT_SHIFT", "COLON", "COLON_COLON",
        "QUESTION", "IDENTIFIER", "STRING", "NUMBER", "CHARACTER", "FN",
        "LET", "MUT", "CLASS", "STRUCT", "ENUM", "IF", "ELSE", "SWITCH",
        "CASE", "FALLTHROUGH", "WHILE", "FOR", "FOREACH", "DO", "RETURN",
        "PUBLIC", "PRIVATE", "IMPORT", "USE", "AS", "PACKAGE", "REQUIRE",
        "TYPE", "SELF", "TRUE", "FALSE", "COMMENT", "END_OF_FILE"
    };
    return names[static_cast<int>(type)];
}


struct Token {
  TokenType type;
  std::string lexeme;
  int line;
};

} // namespace Chtholly

#endif // CHTHOLLY_TOKEN_H
