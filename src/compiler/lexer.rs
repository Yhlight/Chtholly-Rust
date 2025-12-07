#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Keywords
    Fn,
    Let,
    Mut,
    Class,
    Struct,
    Enum,
    If,
    Else,
    Switch,
    Case,
    Break,
    Fallthrough,
    While,
    For,
    Foreach,
    Do,
    Return,
    True,
    False,
    Public,
    Private,
    Import,
    Use,
    As,
    Package,
    Request,
    Require,
    Type,

    // Operators
    Plus,
    Minus,
    Asterisk,
    Slash,
    Percent,
    EqualEqual,
    NotEqual,
    GreaterThan,
    GreaterThanOrEqual,
    LessThan,
    LessThanOrEqual,
    Bang,
    Ampersand,
    Pipe,
    Caret,
    Tilde,
    LeftShift,
    RightShift,
    Equal,
    PlusEqual,
    MinusEqual,
    AsteriskEqual,
    SlashEqual,
    PercentEqual,
    AmpersandAmpersand,
    PipePipe,
    PlusPlus,
    MinusMinus,

    // Punctuation
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Comma,
    Colon,
    Semicolon,
    Dot,

    // Literals
    Identifier(String),
    Integer(i64),
    Float(f64),
    Char(char),
    String(String),

    // End of File
    Eof,
}

pub struct Lexer<'a> {
    input: &'a str,
    position: usize,
    read_position: usize,
    ch: u8,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut l = Lexer {
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

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        let tok = match self.ch {
            b'=' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::EqualEqual
                } else {
                    Token::Equal
                }
            }
            b'+' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::PlusEqual
                } else if self.peek_char() == b'+' {
                    self.read_char();
                    Token::PlusPlus
                } else {
                    Token::Plus
                }
            }
            b'-' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::MinusEqual
                } else if self.peek_char() == b'-' {
                    self.read_char();
                    Token::MinusMinus
                } else {
                    Token::Minus
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
            b'*' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::AsteriskEqual
                } else {
                    Token::Asterisk
                }
            }
            b'/' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::SlashEqual
                } else {
                    Token::Slash
                }
            }
            b'%' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::PercentEqual
                } else {
                    Token::Percent
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
            b'&' => {
                if self.peek_char() == b'&' {
                    self.read_char();
                    Token::AmpersandAmpersand
                } else {
                    Token::Ampersand
                }
            }
            b'|' => {
                if self.peek_char() == b'|' {
                    self.read_char();
                    Token::PipePipe
                } else {
                    Token::Pipe
                }
            }
            b'^' => Token::Caret,
            b'~' => Token::Tilde,
            b';' => Token::Semicolon,
            b'(' => Token::LParen,
            b')' => Token::RParen,
            b',' => Token::Comma,
            b'{' => Token::LBrace,
            b'}' => Token::RBrace,
            b'[' => Token::LBracket,
            b']' => Token::RBracket,
            b':' => Token::Colon,
            0 => Token::Eof,
            b'a'..=b'z' | b'A'..=b'Z' | b'_' => {
                let ident = self.read_identifier();
                return match ident.as_str() {
                    "fn" => Token::Fn,
                    "let" => Token::Let,
                    "mut" => Token::Mut,
                    "class" => Token::Class,
                    "struct" => Token::Struct,
                    "enum" => Token::Enum,
                    "if" => Token::If,
                    "else" => Token::Else,
                    "switch" => Token::Switch,
                    "case" => Token::Case,
                    "break" => Token::Break,
                    "fallthrough" => Token::Fallthrough,
                    "while" => Token::While,
                    "for" => Token::For,
                    "foreach" => Token::Foreach,
                    "do" => Token::Do,
                    "return" => Token::Return,
                    "true" => Token::True,
                    "false" => Token::False,
                    "public" => Token::Public,
                    "private" => Token::Private,
                    "import" => Token::Import,
                    "use" => Token::Use,
                    "as" => Token::As,
                    "package" => Token::Package,
                    "request" => Token::Request,
                    "require" => Token::Require,
                    "type" => Token::Type,
                    _ => Token::Identifier(ident),
                };
            }
            b'0'..=b'9' => {
                return Token::Integer(self.read_number());
            }
            b'"' => {
                return Token::String(self.read_string());
            }
            _ => panic!("Unsupported character"),
        };

        self.read_char();
        tok
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

    fn read_number(&mut self) -> i64 {
        let position = self.position;
        while self.ch.is_ascii_digit() {
            self.read_char();
        }
        self.input[position..self.position].parse().unwrap()
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

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input.as_bytes()[self.read_position]
        }
    }
}
