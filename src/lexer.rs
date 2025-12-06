
use crate::token::{Token, TokenKind};

pub struct Lexer<'a> {
    input: &'a str,
    position: usize,      // current position in input (points to current char)
    read_position: usize, // current reading position in input (after current char)
    ch: u8,               // current char under examination
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut l = Self {
            input,
            position: 0,
            read_position: 0,
            ch: 0,
        };
        l.read_char();
        l
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

    fn skip_whitespace(&mut self) {
        while self.ch.is_ascii_whitespace() {
            self.read_char();
        }
    }

    fn skip_single_line_comment(&mut self) {
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
    }

    fn skip_multi_line_comment(&mut self) {
        loop {
            if self.ch == b'*' && self.peek_char() == b'/' {
                self.read_char();
                self.read_char();
                break;
            }
            if self.ch == 0 {
                break;
            }
            self.read_char();
        }
    }

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input.as_bytes()[self.read_position]
        }
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();
        let tok = match self.ch {
            b'=' => Token::new(TokenKind::Assign, "=".to_string()),
            b';' => Token::new(TokenKind::Semicolon, ";".to_string()),
            b'(' => Token::new(TokenKind::LParen, "(".to_string()),
            b')' => Token::new(TokenKind::RParen, ")".to_string()),
            b'{' => Token::new(TokenKind::LBrace, "{".to_string()),
            b'}' => Token::new(TokenKind::RBrace, "}".to_string()),
            b'/' => {
                if self.peek_char() == b'/' {
                    self.read_char();
                    self.skip_single_line_comment();
                    return self.next_token();
                } else if self.peek_char() == b'*' {
                    self.read_char();
                    self.skip_multi_line_comment();
                    return self.next_token();
                }
                Token::new(TokenKind::Illegal, "/".to_string())
            }
            b'a'..=b'z' | b'A'..=b'Z' | b'_' => {
                let literal = self.read_identifier();
                let kind = match literal.as_str() {
                    "let" => TokenKind::Let,
                    "fn" => TokenKind::Function,
                    _ => TokenKind::Ident,
                };
                return Token::new(kind, literal);
            }
            b'0'..=b'9' => {
                let literal = self.read_number();
                return Token::new(TokenKind::Int, literal);
            }
            0 => Token::new(TokenKind::Eof, "".to_string()),
            _ => Token::new(TokenKind::Illegal, (self.ch as char).to_string()),
        };
        self.read_char();
        tok
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while self.ch.is_ascii_alphabetic() || self.ch == b'_' {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_number(&mut self) -> String {
        let position = self.position;
        while self.ch.is_ascii_digit() {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    pub fn lex(&mut self) -> Vec<Token> {
        let mut tokens = Vec::new();
        loop {
            let tok = self.next_token();
            if tok.kind == TokenKind::Eof {
                tokens.push(tok);
                break;
            }
            tokens.push(tok);
        }
        tokens
    }
}
