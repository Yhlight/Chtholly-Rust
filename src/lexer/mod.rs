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
            b'=' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::Equal
                } else {
                    Token::Assign
                }
            }
            b'+' => {
                if self.peek_char() == b'+' {
                    self.read_char();
                    Token::Increment
                } else if self.peek_char() == b'=' {
                    self.read_char();
                    Token::PlusAssign
                } else {
                    Token::Plus
                }
            }
            b'-' => {
                if self.peek_char() == b'-' {
                    self.read_char();
                    Token::Decrement
                } else if self.peek_char() == b'=' {
                    self.read_char();
                    Token::MinusAssign
                } else {
                    Token::Minus
                }
            }
            b'*' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::StarAssign
                } else {
                    Token::Star
                }
            }
            b'/' => {
                if self.peek_char() == b'/' {
                    self.skip_line_comment();
                    Token::Comment
                } else if self.peek_char() == b'*' {
                    self.skip_block_comment();
                    Token::Comment
                } else if self.peek_char() == b'=' {
                    self.read_char();
                    Token::SlashAssign
                } else {
                    Token::Slash
                }
            }
            b'%' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::PercentAssign
                } else {
                    Token::Percent
                }
            }
            b'!' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::NotEqual
                } else {
                    Token::Not
                }
            }
            b'>' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::GreaterThanOrEqual
                } else if self.peek_char() == b'>' {
                    self.read_char();
                    Token::RightShift
                } else {
                    Token::GreaterThan
                }
            }
            b'<' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::LessThanOrEqual
                } else if self.peek_char() == b'<' {
                    self.read_char();
                    Token::LeftShift
                } else {
                    Token::LessThan
                }
            }
            b'&' => {
                if self.peek_char() == b'&' {
                    self.read_char();
                    Token::LogicalAnd
                } else {
                    Token::Ampersand
                }
            }
            b'|' => {
                if self.peek_char() == b'|' {
                    self.read_char();
                    Token::LogicalOr
                } else {
                    Token::Or
                }
            }
            b'^' => Token::Xor,
            b'~' => Token::Not,
            b'(' => Token::LParen,
            b')' => Token::RParen,
            b'{' => Token::LBrace,
            b'}' => Token::RBrace,
            b'[' => Token::LBracket,
            b']' => Token::RBracket,
            b',' => Token::Comma,
            b':' => Token::Colon,
            b';' => Token::Semicolon,
            b'.' => Token::Dot,
            b'?' => Token::Question,
            b'\'' => self.read_char_literal(),
            b'"' => self.read_string_literal(),
            0 => Token::Eof,
            _ => {
                if is_letter(self.ch) {
                    let ident = self.read_identifier();
                    return match ident.as_str() {
                        "fn" => Token::Fn,
                        "let" => Token::Let,
                        "mut" => Token::Mut,
                        "if" => Token::If,
                        "else" => Token::Else,
                        "switch" => Token::Switch,
                        "case" => Token::Case,
                        "for" => Token::For,
                        "while" => Token::While,
                        "do" => Token::Do,
                        "foreach" => Token::Foreach,
                        "class" => Token::Class,
                        "struct" => Token::Struct,
                        "enum" => Token::Enum,
                        "public" => Token::Public,
                        "private" => Token::Private,
                        "return" => Token::Return,
                        "break" => Token::Break,
                        "continue" => Token::Continue,
                        "fallthrough" => Token::Fallthrough,
                        "import" => Token::Import,
                        "use" => Token::Use,
                        "as" => Token::As,
                        "package" => Token::Package,
                        "request" => Token::Request,
                        "require" => Token::Require,
                        "type" => Token::Type,
                        "true" => Token::Bool(true),
                        "false" => Token::Bool(false),
                        _ => Token::Identifier(ident),
                    };
                } else if is_digit(self.ch) {
                    return self.read_number();
                } else {
                    Token::Illegal
                }
            }
        };
        self.read_char();
        tok
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_ascii_whitespace() {
            self.read_char();
        }
    }

    fn skip_line_comment(&mut self) {
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
    }

    fn skip_block_comment(&mut self) {
        self.read_char(); // Consume '*'
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

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while is_letter(self.ch) || is_digit(self.ch) {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_number(&mut self) -> Token {
        let position = self.position;
        let mut is_float = false;
        while is_digit(self.ch) {
            self.read_char();
        }
        if self.ch == b'.' && is_digit(self.peek_char()) {
            is_float = true;
            self.read_char();
            while is_digit(self.ch) {
                self.read_char();
            }
        }
        let number = &self.input[position..self.position];
        if is_float {
            Token::Double(number.parse().unwrap())
        } else {
            Token::Int(number.parse().unwrap())
        }
    }

    fn read_char_literal(&mut self) -> Token {
        self.read_char(); // consume the opening '
        let ch = self.ch as char;
        self.read_char(); // consume the character
        self.read_char(); // consume the closing '
        Token::Char(ch)
    }

    fn read_string_literal(&mut self) -> Token {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == b'"' || self.ch == 0 {
                break;
            }
        }
        let s = self.input[position..self.position].to_string();
        Token::String(s)
    }

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input.as_bytes()[self.read_position]
        }
    }
}

fn is_letter(ch: u8) -> bool {
    ch.is_ascii_alphabetic() || ch == b'_'
}

fn is_digit(ch: u8) -> bool {
    ch.is_ascii_digit()
}
