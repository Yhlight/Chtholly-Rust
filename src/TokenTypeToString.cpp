#include "Token.h"
#include <string>
#include <unordered_map>

std::string tokenTypeToString(TokenType type) {
    static const std::unordered_map<TokenType, std::string> type_to_string = {
        {TokenType::FN, "FN"}, {TokenType::LET, "LET"}, {TokenType::MUT, "MUT"},
        {TokenType::IF, "IF"}, {TokenType::ELSE, "ELSE"}, {TokenType::WHILE, "WHILE"},
        {TokenType::FOR, "FOR"}, {TokenType::RETURN, "RETURN"}, {TokenType::CLASS, "CLASS"},
        {TokenType::STRUCT, "STRUCT"}, {TokenType::ENUM, "ENUM"}, {TokenType::PUBLIC, "PUBLIC"},
        {TokenType::PRIVATE, "PRIVATE"}, {TokenType::REQUIRE, "REQUIRE"}, {TokenType::IMPORT, "IMPORT"},
        {TokenType::USE, "USE"}, {TokenType::AS, "AS"}, {TokenType::PACKAGE, "PACKAGE"},
        {TokenType::SWITCH, "SWITCH"}, {TokenType::CASE, "CASE"}, {TokenType::BREAK, "BREAK"},
        {TokenType::FALLTHROUGH, "FALLTHROUGH"}, {TokenType::DO, "DO"}, {TokenType::FOREACH, "FOREACH"},
        {TokenType::TRUE, "TRUE"}, {TokenType::FALSE, "FALSE"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::INTEGER_LITERAL, "INTEGER_LITERAL"},
        {TokenType::FLOAT_LITERAL, "FLOAT_LITERAL"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::CHAR_LITERAL, "CHAR_LITERAL"},
        {TokenType::PLUS, "PLUS"}, {TokenType::MINUS, "MINUS"}, {TokenType::STAR, "STAR"},
        {TokenType::SLASH, "SLASH"}, {TokenType::PERCENT, "PERCENT"},
        {TokenType::EQUAL, "EQUAL"}, {TokenType::DOUBLE_EQUAL, "DOUBLE_EQUAL"},
        {TokenType::NOT_EQUAL, "NOT_EQUAL"}, {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"}, {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"}, {TokenType::AND, "AND"},
        {TokenType::OR, "OR"}, {TokenType::NOT, "NOT"}, {TokenType::AMPERSAND, "AMPERSAND"},
        {TokenType::PIPE, "PIPE"}, {TokenType::CARET, "CARET"}, {TokenType::TILDE, "TILDE"},
        {TokenType::LEFT_SHIFT, "LEFT_SHIFT"}, {TokenType::RIGHT_SHIFT, "RIGHT_SHIFT"},
        {TokenType::PLUS_PLUS, "PLUS_PLUS"}, {TokenType::MINUS_MINUS, "MINUS_MINUS"},
        {TokenType::PLUS_EQUAL, "PLUS_EQUAL"}, {TokenType::MINUS_EQUAL, "MINUS_EQUAL"},
        {TokenType::STAR_EQUAL, "STAR_EQUAL"}, {TokenType::SLASH_EQUAL, "SLASH_EQUAL"},
        {TokenType::PERCENT_EQUAL, "PERCENT_EQUAL"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"}, {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"}, {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"}, {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::COMMA, "COMMA"}, {TokenType::DOT, "DOT"}, {TokenType::COLON, "COLON"},
        {TokenType::SEMICOLON, "SEMICOLON"}, {TokenType::ARROW, "ARROW"},
        {TokenType::END_OF_FILE, "END_OF_FILE"}, {TokenType::UNKNOWN, "UNKNOWN"}
    };
    auto it = type_to_string.find(type);
    return it == type_to_string.end() ? "UNKNOWN" : it->second;
}
