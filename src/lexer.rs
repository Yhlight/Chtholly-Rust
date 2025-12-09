use std::iter::Peekable;
use std::str::Chars;
use thiserror::Error;

#[derive(Debug, Error, PartialEq)]
pub enum LexerError {
    #[error("Invalid number literal")]
    InvalidNumberLiteral,
    #[error("Unexpected character: {0}")]
    UnexpectedCharacter(char),
}

#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    Fn,
    Let,
    Mut,
    If,
    Else,
    While,
    For,
    True,
    False,
    Identifier(String),
    IntegerLiteral(i64),
    FloatLiteral(f64),
    StringLiteral(String),

    // Operators
    Plus,
    Minus,
    Asterisk,
    Slash,
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    LessThanOrEqual,
    GreaterThanOrEqual,

    LParen,
    RParen,
    LBrace,
    RBrace,
    Colon,
    Comma,
    Assign,
    Semicolon,
    Arrow,
    Comment(String),
    Whitespace,
    Newline,
    Unknown(char),
}

pub struct Lexer<'a> {
    input: Peekable<Chars<'a>>,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        Lexer {
            input: input.chars().peekable(),
        }
    }
}

impl<'a> Iterator for Lexer<'a> {
    type Item = Result<Token, LexerError>;

    fn next(&mut self) -> Option<Self::Item> {
        let c = self.input.next()?;

        let result = match c {
            '(' => Ok(Token::LParen),
            ')' => Ok(Token::RParen),
            '{' => Ok(Token::LBrace),
            '}' => Ok(Token::RBrace),
            ':' => Ok(Token::Colon),
            ',' => Ok(Token::Comma),
            ';' => Ok(Token::Semicolon),
            '+' => Ok(Token::Plus),
            '*' => Ok(Token::Asterisk),
            '=' => {
                if self.input.peek() == Some(&'=') {
                    self.input.next();
                    Ok(Token::Equal)
                } else {
                    Ok(Token::Assign)
                }
            }
            '!' => {
                if self.input.peek() == Some(&'=') {
                    self.input.next();
                    Ok(Token::NotEqual)
                } else {
                    Err(LexerError::UnexpectedCharacter('!'))
                }
            }
            '<' => {
                if self.input.peek() == Some(&'=') {
                    self.input.next();
                    Ok(Token::LessThanOrEqual)
                } else {
                    Ok(Token::LessThan)
                }
            }
            '>' => {
                if self.input.peek() == Some(&'=') {
                    self.input.next();
                    Ok(Token::GreaterThanOrEqual)
                } else {
                    Ok(Token::GreaterThan)
                }
            }
            '"' => {
                let mut string = String::new();
                while let Some(&c) = self.input.peek() {
                    if c == '"' {
                        break;
                    }
                    string.push(self.input.next().unwrap());
                }
                self.input.next(); // Consume the closing quote
                Ok(Token::StringLiteral(string))
            }
            '-' => {
                if self.input.peek() == Some(&'>') {
                    self.input.next();
                    Ok(Token::Arrow)
                } else {
                    Ok(Token::Minus)
                }
            }
            '/' => {
                if self.input.peek() == Some(&'/') {
                    self.input.next();
                    let mut comment = String::new();
                    while let Some(&c) = self.input.peek() {
                        if c == '\n' {
                            break;
                        }
                        comment.push(self.input.next().unwrap());
                    }
                    Ok(Token::Comment(comment))
                } else {
                    Ok(Token::Slash)
                }
            }
            c if c.is_whitespace() => {
                if c == '\n' {
                    Ok(Token::Newline)
                } else {
                    Ok(Token::Whitespace)
                }
            }
            c if c.is_alphabetic() => {
                let mut identifier = String::new();
                identifier.push(c);
                while let Some(&c) = self.input.peek() {
                    if !c.is_alphanumeric() {
                        break;
                    }
                    identifier.push(self.input.next().unwrap());
                }
                match identifier.as_str() {
                    "fn" => Ok(Token::Fn),
                    "let" => Ok(Token::Let),
                    "mut" => Ok(Token::Mut),
                    "if" => Ok(Token::If),
                    "else" => Ok(Token::Else),
                    "while" => Ok(Token::While),
                    "for" => Ok(Token::For),
                    "true" => Ok(Token::True),
                    "false" => Ok(Token::False),
                    _ => Ok(Token::Identifier(identifier)),
                }
            }
            c if c.is_digit(10) => {
                let mut number = String::new();
                number.push(c);
                while let Some(&c) = self.input.peek() {
                    if !c.is_digit(10) && c != '.' {
                        break;
                    }
                    number.push(self.input.next().unwrap());
                }
                if number.contains('.') {
                    number.parse().map(Token::FloatLiteral).map_err(|_| LexerError::InvalidNumberLiteral)
                } else {
                    number.parse().map(Token::IntegerLiteral).map_err(|_| LexerError::InvalidNumberLiteral)
                }
            }
            _ => Ok(Token::Unknown(c)),
        };
        Some(result)
    }
}
