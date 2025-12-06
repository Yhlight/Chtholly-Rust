use crate::token::{self, Token};

pub struct Lexer<'a> {
    input: &'a str,
    position: usize,      // current position in input (points to current char)
    read_position: usize, // current reading position in input (after current char)
    ch: u8,               // current char under examination
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
            self.ch = 0; // ASCII code for "NUL"
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
                    Token::Eq
                } else {
                    Token::Assign
                }
            }
            b'!' => {
                if self.peek_char() == b'=' {
                    self.read_char();
                    Token::NotEq
                } else {
                    Token::Bang
                }
            }
            b';' => Token::Semicolon,
            b'(' => Token::LParen,
            b')' => Token::RParen,
            b',' => Token::Comma,
            b'+' => Token::Plus,
            b'{' => Token::LBrace,
            b'}' => Token::RBrace,
            b'[' => Token::LBracket,
            b']' => Token::RBracket,
            b'-' => Token::Minus,
            b'*' => Token::Asterisk,
            b'/' => Token::Slash,
            b'<' => Token::Lt,
            b'>' => Token::Gt,
            b'&' => {
                if self.peek_char() == b'&' {
                    self.read_char();
                    Token::And
                } else {
                    Token::Illegal(self.ch.to_string())
                }
            }
            b'|' => {
                if self.peek_char() == b'|' {
                    self.read_char();
                    Token::Or
                } else {
                    Token::Illegal(self.ch.to_string())
                }
            }
            b'"' => return self.read_string(),
            0 => Token::Eof,
            _ => {
                if is_letter(self.ch) {
                    let literal = self.read_identifier();
                    return token::from_literal(&literal);
                } else if is_digit(self.ch) {
                    return self.read_number();
                } else {
                    Token::Illegal(self.ch.to_string())
                }
            }
        };

        self.read_char();
        tok
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while is_letter(self.ch) {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_number(&mut self) -> Token {
        let position = self.position;
        while is_digit(self.ch) {
            self.read_char();
        }
        let number_str = &self.input[position..self.position];
        match number_str.parse() {
            Ok(val) => Token::Int(val),
            Err(_) => Token::Illegal(number_str.to_string()),
        }
    }

    fn skip_whitespace(&mut self) {
        loop {
            match self.ch {
                b' ' | b'\t' | b'\n' | b'\r' => {
                    self.read_char();
                }
                b'/' => {
                    if self.peek_char() == b'/' {
                        self.skip_single_line_comment();
                    } else if self.peek_char() == b'*' {
                        self.skip_multi_line_comment();
                    } else {
                        break;
                    }
                }
                _ => break,
            }
        }
    }

    fn skip_single_line_comment(&mut self) {
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
    }

    fn skip_multi_line_comment(&mut self) {
        // The initial '/*' has already been partially consumed.
        // self.ch is currently '/' and peek_char() is '*'.
        self.read_char(); // Advance past '/' to '*'
        self.read_char(); // Advance past '*'

        loop {
            if self.ch == b'*' && self.peek_char() == b'/' {
                self.read_char();
                self.read_char();
                break;
            }
            if self.ch == 0 { // EOF
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

    fn read_string(&mut self) -> Token {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == b'"' || self.ch == 0 {
                break;
            }
        }
        Token::String(self.input[position..self.position].to_string())
    }
}

fn is_letter(ch: u8) -> bool {
    b'a' <= ch && ch <= b'z' || b'A' <= ch && ch <= b'Z' || ch == b'_'
}

fn is_digit(ch: u8) -> bool {
    b'0' <= ch && ch <= b'9'
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_skip_comments() {
        let input = r#"
            // this is a single line comment
            /*
               this is a
               multi-line comment
            */
        "#;

        let mut lexer = Lexer::new(input);
        let token = lexer.next_token();
        assert_eq!(token, Token::Eof);
    }

    #[test]
    fn test_next_token_core_syntax() {
        let input = "let five = 5; mut ten = 10;";
        let mut lexer = Lexer::new(input);

        let tokens = vec![
            Token::Let,
            Token::Identifier("five".to_string()),
            Token::Assign,
            Token::Int(5),
            Token::Semicolon,
            Token::Mut,
            Token::Identifier("ten".to_string()),
            Token::Assign,
            Token::Int(10),
            Token::Semicolon,
            Token::Eof,
        ];

        for expected_token in tokens {
            let token = lexer.next_token();
            assert_eq!(token, expected_token);
        }
    }

    #[test]
    fn test_integer_overflow() {
        let input = "99999999999999999999999999999999999999";
        let mut lexer = Lexer::new(input);
        let token = lexer.next_token();
        assert!(matches!(token, Token::Illegal(_)));
    }

    #[test]
    fn test_string_literal() {
        let input = r#""hello world""#;
        let mut lexer = Lexer::new(input);
        let token = lexer.next_token();
        assert_eq!(token, Token::String("hello world".to_string()));
    }

    #[test]
    fn test_logical_operators() {
        let input = "&& ||";
        let mut lexer = Lexer::new(input);
        let tokens = vec![Token::And, Token::Or];
        for expected_token in tokens {
            let token = lexer.next_token();
            assert_eq!(token, expected_token);
        }
    }

    #[test]
    fn test_brackets() {
        let input = "[]";
        let mut lexer = Lexer::new(input);
        let tokens = vec![Token::LBracket, Token::RBracket];
        for expected_token in tokens {
            let token = lexer.next_token();
            assert_eq!(token, expected_token);
        }
    }

    #[test]
    fn test_while_keyword() {
        let input = "while";
        let mut lexer = Lexer::new(input);
        let token = lexer.next_token();
        assert_eq!(token, Token::While);
    }

    #[test]
    fn test_for_keyword() {
        let input = "for";
        let mut lexer = Lexer::new(input);
        let token = lexer.next_token();
        assert_eq!(token, Token::For);
    }
}
