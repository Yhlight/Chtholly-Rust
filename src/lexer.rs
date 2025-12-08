#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Keywords
    Let,
    Mut,
    Fn,
    Return,
    If,
    Else,
    While,
    For,
    True,
    False,
    Null,

    // Identifiers and literals
    Identifier(String),
    Integer(i64),
    Float(f64),
    String(String),

    // Operators
    Plus,
    Minus,
    Multiply,
    Divide,
    Assign,
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    LessThanOrEqual,
    GreaterThanOrEqual,
    LogicalAnd,
    LogicalOr,

    // Delimiters
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Comma,
    Semicolon,
    Colon,

    // End of File
    Eof,
    Illegal,
    Bang,
}

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

        let token = match self.ch {
            b'=' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::Equal
                } else {
                    Token::Assign
                }
            }
            b'+' => Token::Plus,
            b'-' => Token::Minus,
            b'*' => Token::Multiply,
            b'/' => Token::Divide,
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
            b'!' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::NotEqual
                } else {
                    Token::Bang
                }
            }
            b'&' => {
                if self.peek_char() == b'&' {
                    self.read_char();
                    Token::LogicalAnd
                } else {
                    Token::Illegal
                }
            }
            b'|' => {
                if self.peek_char() == b'|' {
                    self.read_char();
                    Token::LogicalOr
                } else {
                    Token::Illegal
                }
            }
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
                let identifier = self.read_identifier();
                return match identifier.as_str() {
                    "let" => Token::Let,
                    "mut" => Token::Mut,
                    "fn" => Token::Fn,
                    "return" => Token::Return,
                    "if" => Token::If,
                    "else" => Token::Else,
                    "while" => Token::While,
                    "for" => Token::For,
                    "true" => Token::True,
                    "false" => Token::False,
                    "null" => Token::Null,
                    _ => Token::Identifier(identifier),
                };
            }
            b'0'..=b'9' => return self.read_number(),
            b'"' => Token::String(self.read_string()),
            0 => Token::Eof,
            _ => Token::Illegal,
        };

        self.read_char();
        token
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
        let mut is_float = false;
        while self.ch.is_ascii_digit() || self.ch == b'.' {
            if self.ch == b'.' {
                if is_float {
                    // a second dot is not a valid float
                    return Token::Illegal;
                }
                is_float = true;
            }
            self.read_char();
        }
        let number = &self.input[position..self.position];
        if is_float {
            match number.parse() {
                Ok(n) => Token::Float(n),
                Err(_) => Token::Illegal,
            }
        } else {
            match number.parse() {
                Ok(n) => Token::Integer(n),
                Err(_) => Token::Illegal,
            }
        }
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_next_token() {
        let input = r#"
            let mut five = 5;
            let ten = 10.0;
            let add = fn(x, y) {
                x + y;
            };
            let result = add(five, ten);
            if (result == 15) {
                return true;
            } else {
                return false;
            }
            true && false;
            true || false;
        "#;

        let tests = vec![
            Token::Let,
            Token::Mut,
            Token::Identifier("five".to_string()),
            Token::Assign,
            Token::Integer(5),
            Token::Semicolon,
            Token::Let,
            Token::Identifier("ten".to_string()),
            Token::Assign,
            Token::Float(10.0),
            Token::Semicolon,
            Token::Let,
            Token::Identifier("add".to_string()),
            Token::Assign,
            Token::Fn,
            Token::LParen,
            Token::Identifier("x".to_string()),
            Token::Comma,
            Token::Identifier("y".to_string()),
            Token::RParen,
            Token::LBrace,
            Token::Identifier("x".to_string()),
            Token::Plus,
            Token::Identifier("y".to_string()),
            Token::Semicolon,
            Token::RBrace,
            Token::Semicolon,
            Token::Let,
            Token::Identifier("result".to_string()),
            Token::Assign,
            Token::Identifier("add".to_string()),
            Token::LParen,
            Token::Identifier("five".to_string()),
            Token::Comma,
            Token::Identifier("ten".to_string()),
            Token::RParen,
            Token::Semicolon,
            Token::If,
            Token::LParen,
            Token::Identifier("result".to_string()),
            Token::Equal,
            Token::Integer(15),
            Token::RParen,
            Token::LBrace,
            Token::Return,
            Token::True,
            Token::Semicolon,
            Token::RBrace,
            Token::Else,
            Token::LBrace,
            Token::Return,
            Token::False,
            Token::Semicolon,
            Token::RBrace,
            Token::True,
            Token::LogicalAnd,
            Token::False,
            Token::Semicolon,
            Token::True,
            Token::LogicalOr,
            Token::False,
            Token::Semicolon,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input);

        for (i, tt) in tests.iter().enumerate() {
            let tok = lexer.next_token();
            assert_eq!(*tt, tok, "tests[{}] - token wrong.", i);
        }
    }
}
