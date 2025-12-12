//! Lexer for the Chtholly language.

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
            b'+' => Token::Plus,
            b'=' => Token::Assign,
            b'-' => Token::Minus,
            b'*' => Token::Asterisk,
            b'/' => Token::Slash,
            b'(' => Token::LParen,
            b')' => Token::RParen,
            b'{' => Token::LBrace,
            b'}' => Token::RBrace,
            b';' => Token::Semicolon,
            b',' => Token::Comma,
            b'0'..=b'9' => return self.read_number(),
            b'"' => return self.read_string(),
            b'a'..=b'z' | b'A'..=b'Z' | b'_' => return self.read_identifier(),
            0 => Token::Eof,
            _ => unimplemented!(),
        };

        self.read_char();
        tok
    }

    fn skip_whitespace(&mut self) {
        loop {
            if self.ch.is_ascii_whitespace() {
                self.read_char();
            } else if self.ch == b'/' && self.peek_char() == b'/' {
                self.skip_single_line_comment();
            } else if self.ch == b'/' && self.peek_char() == b'*' {
                self.skip_multi_line_comment();
            } else {
                break;
            }
        }
    }

    fn skip_single_line_comment(&mut self) {
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
    }

    fn skip_multi_line_comment(&mut self) {
        // Consume the '/*'
        self.read_char();
        self.read_char();

        while !(self.ch == b'*' && self.peek_char() == b'/') && self.ch != 0 {
            self.read_char();
        }

        // Consume the '*/'
        if self.ch != 0 {
            self.read_char();
            self.read_char();
        }
    }


    fn read_number(&mut self) -> Token {
        let start = self.position;
        while self.ch.is_ascii_digit() || self.ch == b'.' {
            self.read_char();
        }
        let number_str = &self.input[start..self.position];
        Token::Number(number_str.parse().unwrap())
    }

    fn read_string(&mut self) -> Token {
        let start = self.position + 1;
        self.read_char();
        while self.ch != b'"' && self.ch != 0 {
            self.read_char();
        }
        let string_val = &self.input[start..self.position];
        self.read_char(); // Skip the closing quote
        Token::String(string_val.to_string())
    }

    fn read_identifier(&mut self) -> Token {
        let start = self.position;
        while self.ch.is_ascii_alphanumeric() || self.ch == b'_' {
            self.read_char();
        }
        let ident = &self.input[start..self.position];
        match ident {
            "let" => Token::Let,
            "mut" => Token::Mut,
            "fn" => Token::Fn,
            _ => Token::Identifier(ident.to_string()),
        }
    }
}


#[cfg(test)]
mod tests {
    use super::*;
    use token::Token;

    #[test]
    fn test_skip_single_line_comment() {
        let input = "1 + // this is a comment\n 2";
        let mut lexer = Lexer::new(input);

        assert_eq!(lexer.next_token(), Token::Number(1.0));
        assert_eq!(lexer.next_token(), Token::Plus);
        assert_eq!(lexer.next_token(), Token::Number(2.0));
        assert_eq!(lexer.next_token(), Token::Eof);
    }

    #[test]
    fn test_skip_multi_line_comment() {
        let input = "1 /* this is a \n multi-line comment */ + 2";
        let mut lexer = Lexer::new(input);

        assert_eq!(lexer.next_token(), Token::Number(1.0));
        assert_eq!(lexer.next_token(), Token::Plus);
        assert_eq!(lexer.next_token(), Token::Number(2.0));
        assert_eq!(lexer.next_token(), Token::Eof);
    }

    #[test]
    fn test_slash_is_not_a_comment() {
        let input = "1 / 2";
        let mut lexer = Lexer::new(input);

        assert_eq!(lexer.next_token(), Token::Number(1.0));
        assert_eq!(lexer.next_token(), Token::Slash);
        assert_eq!(lexer.next_token(), Token::Number(2.0));
        assert_eq!(lexer.next_token(), Token::Eof);
    }
}
