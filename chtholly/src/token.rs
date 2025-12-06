#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Keywords
    Fn,
    Let,
    Mut,
    If,
    Else,
    While,
    For,
    Return,
    True,
    False,
    Null,

    // Types
    IntType,
    DoubleType,
    CharType,
    StringType,
    BoolType,
    VoidType,

    // Identifiers and literals
    Identifier(String),
    Int(i64),
    Float(f64),
    String(String),
    Char(char),

    // Operators
    Plus,
    Minus,
    Asterisk,
    Slash,
    Equal,
    NotEqual,
    Bang,
    LessThan,
    GreaterThan,
    LessThanOrEqual,
    GreaterThanOrEqual,
    Assign,

    // Delimiters
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Comma,
    Semicolon,
    Colon,

    // End of file
    Eof,

    // Illegal token
    Illegal,
}
