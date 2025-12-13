#include "Token.h"

namespace Chtholly {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Fn: return "Fn";
        case TokenType::Let: return "Let";
        case TokenType::Mut: return "Mut";
        case TokenType::If: return "If";
        case TokenType::Else: return "Else";
        case TokenType::While: return "While";
        case TokenType::For: return "For";
        case TokenType::Do: return "Do";
        case TokenType::Switch: return "Switch";
        case TokenType::Case: return "Case";
        case TokenType::Class: return "Class";
        case TokenType::Struct: return "Struct";
        case TokenType::Enum: return "Enum";
        case TokenType::Public: return "Public";
        case TokenType::Private: return "Private";
        case TokenType::Return: return "Return";
        case TokenType::Import: return "Import";
        case TokenType::Use: return "Use";
        case TokenType::As: return "As";
        case TokenType::Package: return "Package";
        case TokenType::Const: return "Const";
        case TokenType::Identifier: return "Identifier";
        case TokenType::Integer: return "Integer";
        case TokenType::Float: return "Float";
        case TokenType::String: return "String";
        case TokenType::Char: return "Char";
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Star: return "Star";
        case TokenType::Slash: return "Slash";
        case TokenType::Percent: return "Percent";
        case TokenType::Equal: return "Equal";
        case TokenType::EqualEqual: return "EqualEqual";
        case TokenType::Bang: return "Bang";
        case TokenType::BangEqual: return "BangEqual";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::Ampersand: return "Ampersand";
        case TokenType::AmpersandAmpersand: return "AmpersandAmpersand";
        case TokenType::Pipe: return "Pipe";
        case TokenType::PipePipe: return "PipePipe";
        case TokenType::Caret: return "Caret";
        case TokenType::Tilde: return "Tilde";
        case TokenType::LeftShift: return "LeftShift";
        case TokenType::RightShift: return "RightShift";
        case TokenType::PlusPlus: return "PlusPlus";
        case TokenType::MinusMinus: return "MinusMinus";
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        case TokenType::LeftBracket: return "LeftBracket";
        case TokenType::RightBracket: return "RightBracket";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";
        case TokenType::Colon: return "Colon";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Unknown: return "Unknown";
        default: return "Unknown";
    }
}

} // namespace Chtholly
