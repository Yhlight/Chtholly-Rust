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
            b'=' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::new(TokenKind::Eq, "==".to_string())
                } else {
                    Token::new(TokenKind::Assign, "=".to_string())
                }
            }
            b'!' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::new(TokenKind::NotEq, "!=".to_string())
                } else {
                    Token::new(TokenKind::Bang, "!".to_string())
                }
            }
            b'<' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::new(TokenKind::LtEq, "<=".to_string())
                } else {
                    Token::new(TokenKind::Lt, "<".to_string())
                }
            }
            b'>' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::new(TokenKind::GtEq, ">=".to_string())
                } else {
                    Token::new(TokenKind::Gt, ">".to_string())
                }
            }
            b'&' => {
                if self.peek_char() == b'&' {
                    self.read_char();
                    Token::new(TokenKind::And, "&&".to_string())
                } else {
                    Token::new(TokenKind::Illegal, "&".to_string())
                }
            }
            b'|' => {
                if self.peek_char() == b'|' {
                    self.read_char();
                    Token::new(TokenKind::Or, "||".to_string())
                } else {
                    Token::new(TokenKind::Illegal, "|".to_string())
                }
            }
            b'+' => Token::new(TokenKind::Plus, "+".to_string()),
            b'-' => Token::new(TokenKind::Minus, "-".to_string()),
            b'*' => Token::new(TokenKind::Asterisk, "*".to_string()),
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
                Token::new(TokenKind::Slash, "/".to_string())
            }
            b';' => Token::new(TokenKind::Semicolon, ";".to_string()),
            b'(' => Token::new(TokenKind::LParen, "(".to_string()),
            b')' => Token::new(TokenKind::RParen, ")".to_string()),
            b',' => Token::new(TokenKind::Comma, ",".to_string()),
            b':' => Token::new(TokenKind::Colon, ":".to_string()),
            b'{' => Token::new(TokenKind::LBrace, "{".to_string()),
            b'}' => Token::new(TokenKind::RBrace, "}".to_string()),
            b'a'..=b'z' | b'A'..=b'Z' | b'_' => {
                let literal = self.read_identifier();
                let kind = match literal.as_str() {
                    "let" => TokenKind::Let,
                    "mut" => TokenKind::Mut,
                    "fn" => TokenKind::Function,
                    "true" => TokenKind::True,
                    "false" => TokenKind::False,
                    "if" => TokenKind::If,
                    "else" => TokenKind::Else,
                    "while" => TokenKind::While,
                    "continue" => TokenKind::Continue,
                    "break" => TokenKind::Break,
                    _ => TokenKind::Ident,
                };
                return Token::new(kind, literal);
            }
            b'"' => {
                let literal = self.read_string();
                self.read_char(); // Consume the closing quote
                return Token::new(TokenKind::String, literal);
            }
            b'\'' => {
                let literal = self.read_char_literal();
                return Token::new(TokenKind::Char, literal);
            }
            b'0'..=b'9' => {
                let (literal, is_float) = self.read_number();
                if is_float {
                    return Token::new(TokenKind::Float, literal);
                }
                return Token::new(TokenKind::Int, literal);
            }
            0 => Token::new(TokenKind::Eof, "".to_string()),
            _ => Token::new(TokenKind::Illegal, (self.ch as char).to_string()),
        };
        self.read_char();
        tok
    }

    fn read_string(&mut self) -> String {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == b'"' || self.ch == 0 {
                break;
            }
        }
        self.input[position..self.position].to_string()
    }

    fn read_char_literal(&mut self) -> String {
        let position = self.position + 1;
        self.read_char(); // consumes the char, e.g. 'a'
        let literal = &self.input[position..self.read_position];
        if self.peek_char() != b'\'' {
            // better error handling would be good
            return "".to_string();
        }
        self.read_char(); // consumes the closing quote, e.g. '\''
        literal.to_string()
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while self.ch.is_ascii_alphanumeric() || self.ch == b'_' {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_number(&mut self) -> (String, bool) {
        let position = self.position;
        let mut is_float = false;
        while self.ch.is_ascii_digit() {
            self.read_char();
        }
        if self.ch == b'.' {
            is_float = true;
            self.read_char();
            while self.ch.is_ascii_digit() {
                self.read_char();
            }
        }
        (self.input[position..self.position].to_string(), is_float)
    }
}
