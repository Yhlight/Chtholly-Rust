#[derive(Debug, PartialEq, Eq, Hash, Clone, Copy)]
pub enum TokenKind {
    Eof,
    Illegal,

    // Identifiers + literals
    Ident, // add, foobar, x, y, ...
    Int, // 1343456
    String,
    Char,
    Float,

    // Operators
    Assign,
    Plus,
    Minus,
    Asterisk,
    Slash,
    Bang,
    Eq,
    NotEq,
    Lt,
    Gt,
    GtEq,
    LtEq,
    And,
    Or,

    // Delimiters
    Semicolon,
    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Colon,

    // Keywords
    Let,
    Mut,
    Function,
    True,
    False,
    If,
    Else,
    While,
    Continue,
    Break,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Token {
    pub kind: TokenKind,
    pub literal: String,
}

impl Token {
    pub fn new(kind: TokenKind, literal: String) -> Self {
        Self { kind, literal }
    }
}
