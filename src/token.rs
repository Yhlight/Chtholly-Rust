use std::fmt;

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
    Bang,     // !

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
    True,     // true
    False,    // false
}

impl fmt::Display for Token {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Token::Illegal(s) => write!(f, "Illegal({})", s),
            Token::Eof => write!(f, "EOF"),
            Token::Identifier(s) => write!(f, "{}", s),
            Token::Int(i) => write!(f, "{}", i),
            Token::String(s) => write!(f, "{}", s),
            Token::Assign => write!(f, "="),
            Token::Plus => write!(f, "+"),
            Token::Minus => write!(f, "-"),
            Token::Asterisk => write!(f, "*"),
            Token::Slash => write!(f, "/"),
            Token::Bang => write!(f, "!"),
            Token::Comma => write!(f, ","),
            Token::Semicolon => write!(f, ";"),
            Token::LParen => write!(f, "("),
            Token::RParen => write!(f, ")"),
            Token::LBrace => write!(f, "{{"),
            Token::RBrace => write!(f, "}}"),
            Token::Function => write!(f, "fn"),
            Token::Let => write!(f, "let"),
            Token::Mut => write!(f, "mut"),
            Token::Return => write!(f, "return"),
            Token::True => write!(f, "true"),
            Token::False => write!(f, "false"),
        }
    }
}


pub fn from_literal(literal: &str) -> Token {
    match literal {
        "fn" => Token::Function,
        "let" => Token::Let,
        "mut" => Token::Mut,
        "return" => Token::Return,
        "true" => Token::True,
        "false" => Token::False,
        _ => Token::Identifier(literal.to_string()),
    }
}
