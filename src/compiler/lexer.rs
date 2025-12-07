#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Special Tokens
    Illegal,
    Eof,

    // Keywords
    Let,
    Mut,
    Fn,
    Return,
    If,
    Else,
    True,
    False,

    // Identifiers and Literals
    Identifier(String),
    IntLiteral(i64),
    FloatLiteral(f64),
    StringLiteral(String),

    // Operators
    Assign,      // =
    Plus,        // +
    Minus,       // -
    Asterisk,    // *
    Slash,       // /
    Percent,     // %
    Bang,        // !

    // Comparison
    Equal,       // ==
    NotEqual,    // !=
    LessThan,    // <
    GreaterThan, // >
    LessEqual,   // <=
    GreaterEqual,// >=

    // Punctuation
    Semicolon,   // ;
    Colon,       // :
    Comma,       // ,
    LParen,      // (
    RParen,      // )
    LBrace,      // {
    RBrace,      // }
}

pub struct Lexer<'a> {
    input: &'a [u8],
    position: usize,
    read_position: usize,
    ch: u8,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut l = Lexer {
            input: input.as_bytes(),
            position: 0,
            read_position: 0,
            ch: 0,
        };
        l.read_char();
        l
    }

    fn read_char(&mut self) {
        self.ch = if self.read_position >= self.input.len() {
            0
        } else {
            self.input[self.read_position]
        };
        self.position = self.read_position;
        self.read_position += 1;
    }

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input[self.read_position]
        }
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
            b';' => Token::Semicolon,
            b':' => Token::Colon,
            b',' => Token::Comma,
            b'(' => Token::LParen,
            b')' => Token::RParen,
            b'{' => Token::LBrace,
            b'}' => Token::RBrace,
            b'+' => Token::Plus,
            b'-' => Token::Minus,
            b'*' => Token::Asterisk,
            b'%' => Token::Percent,
            b'<' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::LessEqual
                } else {
                    Token::LessThan
                }
            }
            b'>' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::GreaterEqual
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
            b'/' => {
                if self.peek_char() == b'/' {
                    self.skip_single_line_comment();
                    return self.next_token();
                } else if self.peek_char() == b'*' {
                    self.skip_multi_line_comment();
                    return self.next_token();
                } else {
                    Token::Slash
                }
            }
            b'"' => self.read_string(),
            0 => Token::Eof,
            _ => {
                if self.ch.is_ascii_alphabetic() || self.ch == b'_' {
                    let ident = self.read_identifier();
                    return match ident.as_str() {
                        "let" => Token::Let,
                        "mut" => Token::Mut,
                        "fn" => Token::Fn,
                        "return" => Token::Return,
                        "if" => Token::If,
                        "else" => Token::Else,
                        "true" => Token::True,
                        "false" => Token::False,
                        _ => Token::Identifier(ident),
                    };
                } else if self.ch.is_ascii_digit() {
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

    fn skip_single_line_comment(&mut self) {
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
    }

    fn skip_multi_line_comment(&mut self) {
        self.read_char(); // consume '/'
        self.read_char(); // consume '*'

        loop {
            if self.ch == 0 {
                // End of input, stop
                break;
            }
            if self.ch == b'*' && self.peek_char() == b'/' {
                self.read_char(); // consume '*'
                self.read_char(); // consume '/'
                break;
            }
            self.read_char();
        }
    }

    fn read_identifier(&mut self) -> String {
        let start_pos = self.position;
        while self.ch.is_ascii_alphanumeric() || self.ch == b'_' {
            self.read_char();
        }
        String::from_utf8_lossy(&self.input[start_pos..self.position]).to_string()
    }

    fn read_number(&mut self) -> Token {
        let start_pos = self.position;
        let mut is_float = false;
        while self.ch.is_ascii_digit() {
            self.read_char();
        }

        if self.ch == b'.' && self.peek_char().is_ascii_digit() {
            is_float = true;
            self.read_char(); // consume '.'
            while self.ch.is_ascii_digit() {
                self.read_char();
            }
        }

        let num_str = String::from_utf8_lossy(&self.input[start_pos..self.position]);
        if is_float {
            Token::FloatLiteral(num_str.parse().unwrap_or(0.0))
        } else {
            Token::IntLiteral(num_str.parse().unwrap_or(0))
        }
    }

    fn read_string(&mut self) -> Token {
        let start_pos = self.position + 1;
        loop {
            self.read_char();
            if self.ch == b'"' || self.ch == 0 {
                break;
            }
        }
        let s = String::from_utf8_lossy(&self.input[start_pos..self.position]).to_string();
        Token::StringLiteral(s)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_next_token() {
        let input = r#"let five = 5;
let ten = 10.0;
mut result = "hello";

let add = fn(x, y) {
  x + y;
};

let result = add(five, ten);
! - / * 5;
5 < 10 > 5;

if (5 < 10) {
	return true;
} else {
	return false;
}

10 == 10;
10 != 9;
// this is a comment
/* this is a
multi-line comment */
"#;

        let tests = vec![
            Token::Let,
            Token::Identifier("five".to_string()),
            Token::Assign,
            Token::IntLiteral(5),
            Token::Semicolon,
            Token::Let,
            Token::Identifier("ten".to_string()),
            Token::Assign,
            Token::FloatLiteral(10.0),
            Token::Semicolon,
            Token::Mut,
            Token::Identifier("result".to_string()),
            Token::Assign,
            Token::StringLiteral("hello".to_string()),
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
            Token::Bang,
            Token::Minus,
            Token::Slash,
            Token::Asterisk,
            Token::IntLiteral(5),
            Token::Semicolon,
            Token::IntLiteral(5),
            Token::LessThan,
            Token::IntLiteral(10),
            Token::GreaterThan,
            Token::IntLiteral(5),
            Token::Semicolon,
            Token::If,
            Token::LParen,
            Token::IntLiteral(5),
            Token::LessThan,
            Token::IntLiteral(10),
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
            Token::IntLiteral(10),
            Token::Equal,
            Token::IntLiteral(10),
            Token::Semicolon,
            Token::IntLiteral(10),
            Token::NotEqual,
            Token::IntLiteral(9),
            Token::Semicolon,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input);

        for expected_token in tests {
            let received_token = lexer.next_token();
            assert_eq!(received_token, expected_token);
        }
    }
}
