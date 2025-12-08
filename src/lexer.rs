use std::iter::Peekable;
use std::str::Chars;

#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    Fn,
    Let,
    Mut,
    True,
    False,
    Identifier(String),
    IntegerLiteral(i64),
    FloatLiteral(f64),
    StringLiteral(String),
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
            '=' => Some(Token::Assign),
            ';' => Some(Token::Semicolon),
            '"' => {
                let mut string = String::new();
                while let Some(&c) = self.input.peek() {
                    if c == '"' {
                        break;
                    }
                    string.push(self.input.next().unwrap());
                }
                self.input.next(); // Consume the closing quote
                Some(Token::StringLiteral(string))
            }
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
                    "let" => Some(Token::Let),
                    "mut" => Some(Token::Mut),
                    "true" => Some(Token::True),
                    "false" => Some(Token::False),
                    _ => Some(Token::Identifier(identifier)),
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
                    Some(Token::FloatLiteral(number.parse().unwrap()))
                } else {
                    Some(Token::IntegerLiteral(number.parse().unwrap()))
                }
            }
            _ => Some(Token::Unknown(c)),
        }
    }
}
