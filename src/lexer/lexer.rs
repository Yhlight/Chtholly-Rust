use crate::lexer::token::Token;

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

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while self.ch.is_ascii_alphanumeric() || self.ch == b'_' {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_number(&mut self) -> Token {
        let position = self.position;
        while self.ch.is_ascii_digit() {
            self.read_char();
        }
        let number_str = &self.input[position..self.position];
        Token::Integer(number_str.parse().unwrap())
    }

    fn read_string(&mut self) -> Token {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == b'"' || self.ch == 0 {
                break;
            }
        }
        let value = self.input[position..self.position].to_string();
        self.read_char(); // Consume closing quote
        Token::String(value)
    }

    fn read_char_literal(&mut self) -> Token {
        self.read_char(); // consume opening quote
        let value = self.ch as char;
        self.read_char(); // consume char
        if self.ch != b'\'' {
            return Token::Illegal; // Unterminated char literal
        }
        self.read_char(); // consume closing quote
        Token::Char(value)
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        match self.ch {
            b'=' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    self.read_char();
                    Token::Eq
                } else {
                    self.read_char();
                    Token::Assign
                }
            }
            b'!' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    self.read_char();
                    Token::NotEq
                } else {
                    self.read_char();
                    Token::Illegal
                }
            }
            b'/' => {
                if self.peek_char() == b'/' {
                    while self.ch != b'\n' && self.ch != 0 {
                        self.read_char();
                    }
                    self.next_token() // Recursively call to get next token after comment
                } else if self.peek_char() == b'*' {
                    self.read_char(); // consume '/'
                    self.read_char(); // consume '*'
                    while !(self.ch == b'*' && self.peek_char() == b'/') && self.ch != 0 {
                        self.read_char();
                    }
                    if self.ch != 0 {
                        self.read_char(); // consume '*'
                        self.read_char(); // consume '/'
                    }
                    self.next_token()
                } else {
                    self.read_char();
                    Token::Slash
                }
            }
            b'>' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    self.read_char();
                    Token::GtEq
                } else {
                    self.read_char();
                    Token::Gt
                }
            }
            b'<' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    self.read_char();
                    Token::LtEq
                } else {
                    self.read_char();
                    Token::Lt
                }
            }
            b'+' => { self.read_char(); Token::Plus }
            b'-' => { self.read_char(); Token::Minus }
            b'*' => { self.read_char(); Token::Asterisk }
            b'(' => { self.read_char(); Token::LParen }
            b')' => { self.read_char(); Token::RParen }
            b'{' => { self.read_char(); Token::LBrace }
            b'}' => { self.read_char(); Token::RBrace }
            b',' => { self.read_char(); Token::Comma }
            b';' => { self.read_char(); Token::Semicolon }
            b'"' => self.read_string(),
            b'\'' => self.read_char_literal(),
            0 => Token::Eof,
            _ => {
                if self.ch.is_ascii_alphabetic() || self.ch == b'_' {
                    let ident = self.read_identifier();
                    lookup_ident(&ident)
                } else if self.ch.is_ascii_digit() {
                    self.read_number()
                } else {
                    self.read_char();
                    Token::Illegal
                }
            }
        }
    }
}

fn lookup_ident(ident: &str) -> Token {
    match ident {
        "fn" => Token::Fn,
        "let" => Token::Let,
        "mut" => Token::Mut,
        "if" => Token::If,
        "else" => Token::Else,
        "return" => Token::Return,
        "true" => Token::True,
        "false" => Token::False,
        _ => Token::Identifier(ident.to_string()),
    }
}
