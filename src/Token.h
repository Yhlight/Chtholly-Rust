#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <string_view>

namespace Chtholly
{

enum class TokenType
{
    // Single-character tokens
    LeftParen, RightParen,
    LeftBrace, RightBrace,
    LeftBracket, RightBracket,
    Comma, Dot, Semicolon, Colon,

    // One or two character tokens
    Plus, PlusEqual, PlusPlus,
    Minus, MinusEqual, MinusMinus,
    Star, StarEqual,
    Slash, SlashEqual,
    Percent, PercentEqual,
    Bang, BangEqual,
    Equal, EqualEqual,
    Greater, GreaterEqual,
    Less, LessEqual,
    Amp, AmpAmp,
    Pipe, PipePipe,
    Caret,
    Tilde,
    LessLess, GreaterGreater,

    // Literals
    Identifier,
    String,
    Character,
    Number,

    // Keywords
    And, Class, Struct, Enum, Else, False,
    For, Fn, If, Mut, Let,
    Print, // placeholder for println
    Return, Super, Self, This, True,
    While, Switch, Case, Fallthrough,
    Public, Private, Import, Use, As,
    Package, Const,

    // Other
    Error,
    Eof
};

struct Token
{
    TokenType type;
    std::string_view lexeme;
    int line;
    int column;
};

// Helper function for debugging
inline std::string to_string(TokenType type)
{
    switch (type)
    {
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        case TokenType::LeftBracket: return "LeftBracket";
        case TokenType::RightBracket: return "RightBracket";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Colon: return "Colon";
        case TokenType::Plus: return "Plus";
        case TokenType::PlusEqual: return "PlusEqual";
        case TokenType::PlusPlus: return "PlusPlus";
        case TokenType::Minus: return "Minus";
        case TokenType::MinusEqual: return "MinusEqual";
        case TokenType::MinusMinus: return "MinusMinus";
        case TokenType::Star: return "Star";
        case TokenType::StarEqual: return "StarEqual";
        case TokenType::Slash: return "Slash";
        case TokenType::SlashEqual: return "SlashEqual";
        case TokenType::Percent: return "Percent";
        case TokenType::PercentEqual: return "PercentEqual";
        case TokenType::Bang: return "Bang";
        case TokenType::BangEqual: return "BangEqual";
        case TokenType::Equal: return "Equal";
        case TokenType::EqualEqual: return "EqualEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::Amp: return "Amp";
        case TokenType::AmpAmp: return "AmpAmp";
        case TokenType::Pipe: return "Pipe";
        case TokenType::PipePipe: return "PipePipe";
        case TokenType::Caret: return "Caret";
        case TokenType::Tilde: return "Tilde";
        case TokenType::LessLess: return "LessLess";
        case TokenType::GreaterGreater: return "GreaterGreater";
        case TokenType::Identifier: return "Identifier";
        case TokenType::String: return "String";
        case TokenType::Character: return "Character";
        case TokenType::Number: return "Number";
        case TokenType::And: return "And";
        case TokenType::Class: return "Class";
        case TokenType::Struct: return "Struct";
        case TokenType::Enum: return "Enum";
        case TokenType::Else: return "Else";
        case TokenType::False: return "False";
        case TokenType::For: return "For";
        case TokenType::Fn: return "Fn";
        case TokenType::If: return "If";
        case TokenType::Mut: return "Mut";
        case TokenType::Let: return "Let";
        case TokenType::Print: return "Print";
        case TokenType::Return: return "Return";
        case TokenType::Super: return "Super";
        case TokenType::Self: return "Self";
        case TokenType::This: return "This";
        case TokenType::True: return "True";
        case TokenType::While: return "While";
        case TokenType::Switch: return "Switch";
        case TokenType::Case: return "Case";
        case TokenType::Fallthrough: return "Fallthrough";
        case TokenType::Public: return "Public";
        case TokenType::Private: return "Private";
        case TokenType::Import: return "Import";
        case TokenType::Use: return "Use";
        case TokenType::As: return "As";
        case TokenType::Package: return "Package";
        case TokenType::Const: return "Const";
        case TokenType::Error: return "Error";
        case TokenType::Eof: return "Eof";
    }
    return "Unknown";
}

} // namespace Chtholly

#endif // CHTHOLLY_TOKEN_H
