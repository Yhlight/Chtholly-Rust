use std::iter::Peekable;
use std::str::Chars;

#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    Fn,
    Identifier(String),
    LParen,
    RParen,
    LBrace,
    RBrace,
    Colon,
    Comma,
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
    type Item = Token;

    fn next(&mut self) -> Option<Token> {
        let c = self.input.next()?;

        match c {
            '(' => Some(Token::LParen),
            ')' => Some(Token::RParen),
            '{' => Some(Token::LBrace),
            '}' => Some(Token::RBrace),
            ':' => Some(Token::Colon),
            ',' => Some(Token::Comma),
            '-' if self.input.peek() == Some(&'>') => {
                self.input.next();
                Some(Token::Arrow)
            }
            '/' if self.input.peek() == Some(&'/') => {
                self.input.next();
                let mut comment = String::new();
                while let Some(&c) = self.input.peek() {
                    if c == '\n' {
                        break;
                    }
                    comment.push(self.input.next().unwrap());
                }
                Some(Token::Comment(comment))
            }
            c if c.is_whitespace() => {
                if c == '\n' {
                    Some(Token::Newline)
                } else {
                    Some(Token::Whitespace)
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
                    "fn" => Some(Token::Fn),
                    _ => Some(Token::Identifier(identifier)),
                }
            }
            _ => Some(Token::Unknown(c)),
        }
    }
}
