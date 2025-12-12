use std::iter::Peekable;
use std::str::Chars;

#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Keywords
    Fn,
    Let,
    Mut,
    If,
    Else,
    While,
    For,
    Do,
    Switch,
    Case,
    Fallthrough,
    Return,
    Class,
    Struct,
    Enum,
    Public,
    Private,
    Const,
    Import,
    Use,
    As,
    Package,

    // Literals
    Identifier(String),
    Integer(i64),
    Float(f64),
    String(String),
    Char(char),

    // Operators
    Plus,
    Minus,
    Asterisk,
    Slash,
    Percent,
    Equal,
    EqualEqual,
    NotEqual,
    GreaterThan,
    GreaterThanOrEqual,
    LessThan,
    LessThanOrEqual,
    Bang,
    And,
    Or,
    Xor,
    Tilde,
    LeftShift,
    RightShift,
    AndAnd,
    OrOr,
    PlusPlus,
    MinusMinus,

    // Punctuation
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Semicolon,
    Colon,
    Comma,
    Dot,

    // Others
    Illegal(String),
    Eof,
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

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        match self.read_char() {
            Some(ch) => match ch {
                '+' => Token::Plus,
                '-' => Token::Minus,
                '*' => Token::Asterisk,
                '/' => {
                    if self.peek() == Some(&'/') {
                        self.skip_line_comment();
                        self.next_token()
                    } else if self.peek() == Some(&'*') {
                        self.skip_block_comment();
                        self.next_token()
                    } else {
                        Token::Slash
                    }
                }
                '%' => Token::Percent,
                '=' => {
                    if self.peek() == Some(&'=') {
                        self.read_char();
                        Token::EqualEqual
                    } else {
                        Token::Equal
                    }
                }
                '!' => {
                    if self.peek() == Some(&'=') {
                        self.read_char();
                        Token::NotEqual
                    } else {
                        Token::Bang
                    }
                }
                '>' => {
                    if self.peek() == Some(&'=') {
                        self.read_char();
                        Token::GreaterThanOrEqual
                    } else if self.peek() == Some(&'>') {
                        self.read_char();
                        Token::RightShift
                    } else {
                        Token::GreaterThan
                    }
                }
                '<' => {
                    if self.peek() == Some(&'=') {
                        self.read_char();
                        Token::LessThanOrEqual
                    } else if self.peek() == Some(&'<') {
                        self.read_char();
                        Token::LeftShift
                    } else {
                        Token::LessThan
                    }
                }
                '&' => {
                    if self.peek() == Some(&'&') {
                        self.read_char();
                        Token::AndAnd
                    } else {
                        Token::And
                    }
                }
                '|' => {
                    if self.peek() == Some(&'|') {
                        self.read_char();
                        Token::OrOr
                    } else {
                        Token::Or
                    }
                }
                '^' => Token::Xor,
                '~' => Token::Tilde,
                '(' => Token::LParen,
                ')' => Token::RParen,
                '{' => Token::LBrace,
                '}' => Token::RBrace,
                '[' => Token::LBracket,
                ']' => Token::RBracket,
                ';' => Token::Semicolon,
                ':' => Token::Colon,
                ',' => Token::Comma,
                '.' => Token::Dot,
                '"' => self.read_string(),
                '\'' => self.read_char_literal(),
                _ if ch.is_alphabetic() || ch == '_' => self.read_identifier(ch),
                _ if ch.is_digit(10) => self.read_number(ch),
                _ => Token::Illegal(ch.to_string()),
            },
            None => Token::Eof,
        }
    }

    fn read_char(&mut self) -> Option<char> {
        self.input.next()
    }

    fn peek(&mut self) -> Option<&char> {
        self.input.peek()
    }

    fn skip_whitespace(&mut self) {
        while let Some(&ch) = self.peek() {
            if ch.is_whitespace() {
                self.read_char();
            } else {
                break;
            }
        }
    }

    fn skip_line_comment(&mut self) {
        self.read_char(); // Consume the second '/'
        while let Some(ch) = self.read_char() {
            if ch == '\n' {
                break;
            }
        }
    }

    fn skip_block_comment(&mut self) {
        self.read_char(); // Consume the '*'
        while let Some(ch) = self.read_char() {
            if ch == '*' && self.peek() == Some(&'/') {
                self.read_char();
                break;
            }
        }
    }

    fn read_identifier(&mut self, first_char: char) -> Token {
        let mut ident = String::new();
        ident.push(first_char);

        while let Some(&ch) = self.peek() {
            if ch.is_alphanumeric() || ch == '_' {
                ident.push(self.read_char().unwrap());
            } else {
                break;
            }
        }

        match ident.as_str() {
            "fn" => Token::Fn,
            "let" => Token::Let,
            "mut" => Token::Mut,
            "if" => Token::If,
            "else" => Token::Else,
            "while" => Token::While,
            "for" => Token::For,
            "do" => Token::Do,
            "switch" => Token::Switch,
            "case" => Token::Case,
            "fallthrough" => Token::Fallthrough,
            "return" => Token::Return,
            "class" => Token::Class,
            "struct" => Token::Struct,
            "enum" => Token::Enum,
            "public" => Token::Public,
            "private" => Token::Private,
            "const" => Token::Const,
            "import" => Token::Import,
            "use" => Token::Use,
            "as" => Token::As,
            "package" => Token::Package,
            _ => Token::Identifier(ident),
        }
    }

    fn read_number(&mut self, first_char: char) -> Token {
        let mut number = String::new();
        number.push(first_char);

        while let Some(&ch) = self.peek() {
            if ch.is_digit(10) || ch == '.' {
                number.push(self.read_char().unwrap());
            } else {
                break;
            }
        }

        if number.contains('.') {
            Token::Float(number.parse().unwrap())
        } else {
            Token::Integer(number.parse().unwrap())
        }
    }

    fn read_string(&mut self) -> Token {
        let mut string = String::new();
        while let Some(ch) = self.read_char() {
            if ch == '"' {
                break;
            }
            string.push(ch);
        }
        Token::String(string)
    }

    fn read_char_literal(&mut self) -> Token {
        let ch = self.read_char().unwrap();
        if self.read_char() != Some('\'') {
            Token::Illegal(format!("'{}'", ch))
        } else {
            Token::Char(ch)
        }
    }
}
