//! Lexer for the Chtholly language.

pub mod token;

use token::Token;

pub struct Lexer<'a> {
    input: &'a str,
    position: usize,
    read_position: usize,
    ch: u8,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut lexer = Lexer {
            input,
            position: 0,
            read_position: 0,
            ch: 0,
        };
        lexer.read_char();
        lexer
    }

    fn read_char(&mut self) {
        if self.read_position >= self.input.len() {
            self.ch = 0;
        } else {
            self.ch = self.input.as_bytes()[self.read_position];
        }
        self.position = self.read_position;
        self.read_position += 1;
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        let tok = match self.ch {
            b'+' => Token::Plus,
            b'-' => Token::Minus,
            b'*' => Token::Asterisk,
            b'/' => Token::Slash,
            b'(' => Token::LParen,
            b')' => Token::RParen,
            b'{' => Token::LBrace,
            b'}' => Token::RBrace,
            b'0'..=b'9' => return self.read_number(),
            b'"' => return self.read_string(),
            b'a'..=b'z' | b'A'..=b'Z' | b'_' => return self.read_identifier(),
            0 => Token::Eof,
            _ => unimplemented!(),
        };

        self.read_char();
        tok
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_ascii_whitespace() {
            self.read_char();
        }
    }

    fn read_number(&mut self) -> Token {
        let start = self.position;
        while self.ch.is_ascii_digit() || self.ch == b'.' {
            self.read_char();
        }
        let number_str = &self.input[start..self.position];
        Token::Number(number_str.parse().unwrap())
    }

    fn read_string(&mut self) -> Token {
        let start = self.position + 1;
        self.read_char();
        while self.ch != b'"' && self.ch != 0 {
            self.read_char();
        }
        let string_val = &self.input[start..self.position];
        self.read_char(); // Skip the closing quote
        Token::String(string_val.to_string())
    }

    fn read_identifier(&mut self) -> Token {
        let start = self.position;
        while self.ch.is_ascii_alphanumeric() || self.ch == b'_' {
            self.read_char();
        }
        let ident = &self.input[start..self.position];
        match ident {
            "let" => Token::Let,
            "mut" => Token::Mut,
            "fn" => Token::Fn,
            _ => Token::Identifier(ident.to_string()),
        }
    }
}
