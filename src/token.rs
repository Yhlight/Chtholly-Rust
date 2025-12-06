// TODO: Integrate Position into Tokens to provide better error messages.
#[derive(Debug, PartialEq, Clone)]
pub struct Position {
    pub line: usize,
    pub column: usize,
}

impl Position {
    pub fn new(line: usize, column: usize) -> Self {
        Self { line, column }
    }
}

#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    Illegal(String),
    Eof,

    // Identifiers + literals
    Identifier(String),
    Int(i64),
    String(String),

    // Operators
    Assign,   // =
    Plus,     // +
    Minus,    // -
    Asterisk, // *
    Slash,    // /

    // Delimiters
    Comma,     // ,
    Semicolon, // ;
    LParen,    // (
    RParen,    // )
    LBrace,    // {
    RBrace,    // }

    // Keywords
    Function, // fn
    Let,      // let
    Mut,      // mut
    Return,   // return
}

pub fn from_literal(literal: &str) -> Token {
    match literal {
        "fn" => Token::Function,
        "let" => Token::Let,
        "mut" => Token::Mut,
        "return" => Token::Return,
        _ => Token::Identifier(literal.to_string()),
    }
}
