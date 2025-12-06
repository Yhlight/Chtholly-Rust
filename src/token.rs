
#[derive(Debug, PartialEq, Eq, Hash, Clone, Copy)]
pub enum TokenKind {
    Eof,
    Illegal,

    // Identifiers + literals
    Ident, // add, foobar, x, y, ...
    Int,   // 1343456

    // Operators
    Assign,
    Plus,
    Minus,
    Asterisk,
    Slash,

    // Delimiters
    Semicolon,
    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,

    // Keywords
    Let,
    Mut,
    Function,
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
