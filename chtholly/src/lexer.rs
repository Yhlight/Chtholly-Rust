use crate::token::Token;

pub struct Lexer {
    input: String,
    position: usize,
    read_position: usize,
    ch: u8,
}

impl Lexer {
    pub fn new(input: String) -> Self {
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
            b'=' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::Equal
                } else {
                    Token::Assign
                }
            }
            b'!' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::NotEqual
                } else {
                    Token::Bang
                }
            }
            b'<' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::LessThanOrEqual
                } else {
                    Token::LessThan
                }
            }
            b'>' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::GreaterThanOrEqual
                } else {
                    Token::GreaterThan
                }
            }
            b'+' => Token::Plus,
            b'-' => Token::Minus,
            b'*' => Token::Asterisk,
            b'/' => Token::Slash,
            b'(' => Token::LParen,
            b')' => Token::RParen,
            b'{' => Token::LBrace,
            b'}' => Token::RBrace,
            b'[' => Token::LBracket,
            b']' => Token::RBracket,
            b',' => Token::Comma,
            b';' => Token::Semicolon,
            b':' => Token::Colon,
            b'a'..=b'z' | b'A'..=b'Z' | b'_' => {
                let ident = self.read_identifier();
                return match ident.as_str() {
                    "fn" => Token::Fn,
                    "let" => Token::Let,
                    "mut" => Token::Mut,
                    "if" => Token::If,
                    "else" => Token::Else,
                    "while" => Token::While,
                    "for" => Token::For,
                    "return" => Token::Return,
                    "true" => Token::True,
                    "false" => Token::False,
                    "int" => Token::IntType,
                    "double" => Token::DoubleType,
                    "char" => Token::CharType,
                    "string" => Token::StringType,
                    "bool" => Token::BoolType,
                    "void" => Token::VoidType,
                    "struct" => Token::Struct,
                    _ => Token::Identifier(ident),
                };
            }
            b'0'..=b'9' => return self.read_number(),
            b'"' => return self.read_string(),
            b'\'' => return self.read_char_literal(),
            0 => Token::Eof,
            _ => Token::Illegal,
        };

        self.read_char();
        tok
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while self.ch.is_ascii_alphanumeric() || self.ch == b'_' {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_number(&mut self) -> Token {
        let position = self.position;
        let mut is_float = false;
        while self.ch.is_ascii_digit() {
            self.read_char();
        }
        if self.ch == b'.' && self.peek_char().is_ascii_digit() {
            is_float = true;
            self.read_char();
            while self.ch.is_ascii_digit() {
                self.read_char();
            }
        }
        let number = &self.input[position..self.position];
        if is_float {
            number
                .parse()
                .map(Token::Float)
                .unwrap_or(Token::Illegal)
        } else {
            number.parse().map(Token::Int).unwrap_or(Token::Illegal)
        }
    }

    fn read_string(&mut self) -> Token {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == b'"' || self.ch == 0 {
                break;
            }
        }
        let s = self.input[position..self.position].to_string();
        self.read_char();
        Token::String(s)
    }

    fn read_char_literal(&mut self) -> Token {
        self.read_char();
        let ch = self.ch as char;
        self.read_char();
        if self.ch != b'\'' {
            return Token::Illegal;
        }
        self.read_char();
        Token::Char(ch)
    }

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input.as_bytes()[self.read_position]
        }
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_ascii_whitespace() {
            self.read_char();
        }
    }
}

#[cfg(test)]
mod tests;
