#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Keywords
    Fn,
    Let,
    Mut,
    If,
    Else,
    Return,
    True,
    False,

    // Operators
    Plus,
    Minus,
    Asterisk,
    Slash,
    Assign,
    Bang,
    Eq,
    NotEq,
    Gt,
    Lt,
    GtEq,
    LtEq,

    // Delimiters
    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Semicolon,

    // Other
    Identifier(String),
    Integer(i64),
    String(String),
    Char(char),
    Illegal,
    Eof,
}
