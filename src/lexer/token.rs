//! Tokens for the Chtholly language.

#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Literals
    Number(f64),
    String(String),

    // Operators
    Plus,
    Minus,
    Asterisk,
    Slash,

    // Delimiters
    LParen,
    RParen,
    LBrace,
    RBrace,

    // Keywords
    Let,
    Mut,
    Fn,

    // Identifier
    Identifier(String),

    // End of File
    Eof,
}
