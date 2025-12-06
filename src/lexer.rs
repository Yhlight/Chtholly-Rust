use crate::token::Token;

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

        match self.ch {
            0 => Token::Eof,
            _ => {
                // If it's not whitespace or a comment, we'll just return EOF for now
                // to make the test pass.
                if self.position >= self.input.len() {
                    Token::Eof
                } else {
                    // This path should not be hit by the current test
                    Token::Illegal(self.ch.to_string())
                }
            }
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
        // advance past the initial '/*'
        self.read_char();
        self.read_char();

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
}