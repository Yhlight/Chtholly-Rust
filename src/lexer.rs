#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Keywords
    Fn,
    Let,
    Mut,
    True,
    False,
    If,
    Else,

    // Identifiers
    Ident(String),

    // Literals
    Int(i64),
    Double(f64),
    String(String),
    Char(char),

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Assign,
    Lt,

    // Delimiters
    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Colon,
    Semicolon,

    // End of file
    Eof,

    // Illegal token
    Illegal,
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
            b'/' => {
                if self.peek_char() == b'/' {
                    while self.ch != b'\n' && self.ch != 0 {
                        self.read_char();
                    }
                    return self.next_token();
                } else if self.peek_char() == b'*' {
                    loop {
                        self.read_char();
                        if self.ch == b'*' && self.peek_char() == b'/' {
                            break;
                        }
                    }
                    self.read_char();
                    self.read_char();
                    return self.next_token();
                } else {
                    Token::Slash
                }
            }
            b'=' => Token::Assign,
            b'+' => Token::Plus,
            b'-' => Token::Minus,
            b'*' => Token::Star,
            b'<' => Token::Lt,
            b'(' => Token::LParen,
            b')' => Token::RParen,
            b'{' => Token::LBrace,
            b'}' => Token::RBrace,
            b',' => Token::Comma,
            b':' => Token::Colon,
            b';' => Token::Semicolon,
            b'a'..=b'z' | b'A'..=b'Z' | b'_' => {
                let ident = self.read_identifier();
                return match ident.as_str() {
                    "fn" => Token::Fn,
                    "let" => Token::Let,
                    "mut" => Token::Mut,
                    "true" => Token::True,
                    "false" => Token::False,
                    "if" => Token::If,
                    "else" => Token::Else,
                    _ => Token::Ident(ident),
                };
            }
            b'0'..=b'9' => {
                return self.read_number();
            }
            b'"' => {
                let string = self.read_string();
                Token::String(string)
            }
            b'\'' => {
                self.read_char();
                let ch = self.ch as char;
                self.read_char();
                if self.ch != b'\'' {
                    return Token::Illegal;
                }
                Token::Char(ch)
            }
            0 => Token::Eof,
            _ => Token::Illegal,
        };

        self.read_char();
        token
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
        if self.ch == b'.' {
            self.read_char();
            while self.ch.is_ascii_digit() {
                self.read_char();
            }
            let literal = &self.input[position..self.position];
            Token::Double(literal.parse().expect("Failed to parse double"))
        } else {
            let literal = &self.input[position..self.position];
            Token::Int(literal.parse().expect("Failed to parse int"))
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
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_next_token() {
        let input = r#"
            let five = 5;
            let ten = 10.0;
            fn add(x, y) {
                x + y;
            }
            let result = add(five, ten);
            true;
            false;
            'a';
        "#;

        let tests = vec![
            Token::Let,
            Token::Ident("five".to_string()),
            Token::Assign,
            Token::Int(5),
            Token::Semicolon,
            Token::Let,
            Token::Ident("ten".to_string()),
            Token::Assign,
            Token::Double(10.0),
            Token::Semicolon,
            Token::Fn,
            Token::Ident("add".to_string()),
            Token::LParen,
            Token::Ident("x".to_string()),
            Token::Comma,
            Token::Ident("y".to_string()),
            Token::RParen,
            Token::LBrace,
            Token::Ident("x".to_string()),
            Token::Plus,
            Token::Ident("y".to_string()),
            Token::Semicolon,
            Token::RBrace,
            Token::Let,
            Token::Ident("result".to_string()),
            Token::Assign,
            Token::Ident("add".to_string()),
            Token::LParen,
            Token::Ident("five".to_string()),
            Token::Comma,
            Token::Ident("ten".to_string()),
            Token::RParen,
            Token::Semicolon,
            Token::True,
            Token::Semicolon,
            Token::False,
            Token::Semicolon,
            Token::Char('a'),
            Token::Semicolon,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input);

        for expected_token in tests {
            let token = lexer.next_token();
            assert_eq!(token, expected_token);
        }
    }
}
