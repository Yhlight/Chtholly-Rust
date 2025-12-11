#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include <string>
#include <vector>

namespace Chtholly {

enum class TokenType {
    // End of file
    Eof,

    // Comments
    Comment,

    // Keywords
    Fn, Main, Let, Mut, Class, Struct, Enum, If, Else, Switch, Case, For, While, Do, Break, Continue, Fallthrough, Return, True, False, Import, Use, As, Package, Public, Private, Const,

    // Types
    I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, Char, Void, Bool, String,

    // Operators
    Plus, Minus, Star, Slash, Percent,
    EqualEqual, NotEqual, Greater, GreaterEqual, Less, LessEqual,
    Not, Amp, Pipe, Caret, Tilde, LessLess, GreaterGreater,
    Equal, PlusEqual, MinusEqual, StarEqual, SlashEqual, PercentEqual,
    AmpAmp, PipePipe,
    PlusPlus, MinusMinus,
    Question,

    // Punctuation
    LParen, RParen, LBrace, RBrace, LBracket, RBracket,
    Colon, Semicolon, Comma, Dot,

    // Literals
    Identifier,
    Integer,
    Float,
    Character,
    StringLiteral,

    // Misc
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    Token nextToken();
    char peek();
    char advance();
    void skipWhitespaceAndComments();
    Token identifier();
    Token number();
    Token stringLiteral();
    Token characterLiteral();

    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
};

} // namespace Chtholly

#endif // CHTHOLLY_LEXER_H
