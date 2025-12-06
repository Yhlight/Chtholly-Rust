pub mod token;
use token::Token;

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

        if self.ch == b'.' && self.peek_char().is_ascii_digit() {
            self.read_char();
            while self.ch.is_ascii_digit() {
                self.read_char();
            }
            let literal = &self.input[position..self.position];
            match literal.parse::<f64>() {
                Ok(f) => Token::Float(f),
                Err(_) => Token::Illegal,
            }
        } else {
            let literal = &self.input[position..self.position];
            match literal.parse::<i64>() {
                Ok(i) => Token::Int(i),
                Err(_) => Token::Illegal,
            }
        }
    }

    fn read_string(&mut self) -> String {
        let mut s = String::new();
        loop {
            self.read_char();
            match self.ch {
                b'"' | 0 => break,
                b'\\' => {
                    self.read_char();
                    s.push(self.ch as char);
                }
                _ => s.push(self.ch as char),
            }
        }
        s
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
        b'+' => {
            match self.peek_char() {
                b'=' => { self.read_char(); Token::PlusAssign }
                b'+' => { self.read_char(); Token::Increment }
                _ => Token::Plus
            }
        }
        b'-' => {
            match self.peek_char() {
                b'=' => { self.read_char(); Token::MinusAssign }
                b'-' => { self.read_char(); Token::Decrement }
                _ => Token::Minus
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
        b'/' => {
            if self.peek_char() == b'=' {
                self.read_char();
                Token::SlashAssign
            } else {
                Token::Slash
            }
        }
        b'*' => {
            if self.peek_char() == b'=' {
                self.read_char();
                Token::StarAssign
            } else {
                Token::Asterisk
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
        b'<' => {
            if self.peek_char() == b'=' {
                self.read_char();
                Token::LtEq
            } else {
                Token::Lt
            }
        }
        b'>' => {
            if self.peek_char() == b'=' {
                self.read_char();
                Token::GtEq
            } else {
                Token::Gt
            }
        }
        b';' => Token::Semicolon,
        b':' => Token::Colon,
        b',' => Token::Comma,
        b'{' => Token::LBrace,
        b'}' => Token::RBrace,
        b'(' => Token::LParen,
        b')' => Token::RParen,
        b'[' => Token::LBracket,
        b']' => Token::RBracket,
        b'"' => Token::String(self.read_string()),
        b'\'' => {
            self.read_char();
            let ch = self.ch as char;
            if self.peek_char() == b'\'' {
                self.read_char();
                Token::Char(ch)
            } else {
                Token::Illegal
            }
        }
        0 => Token::Eof,
        _ => {
            if self.ch.is_ascii_alphabetic() || self.ch == b'_' {
                let ident = self.read_identifier();
                let tok = match ident.as_str() {
                    "fn" => Token::Function,
                    "let" => Token::Let,
                    "mut" => Token::Mut,
                    "true" => Token::True,
                    "false" => Token::False,
                    "if" => Token::If,
                    "else" => Token::Else,
                    "return" => Token::Return,
                    "while" => Token::While,
                    "for" => Token::For,
                    "switch" => Token::Switch,
                    "case" => Token::Case,
                    "break" => Token::Break,
                    "fallthrough" => Token::Fallthrough,
                    "class" => Token::Class,
                    "struct" => Token::Struct,
                    "enum" => Token::Enum,
                    "public" => Token::Public,
                    "private" => Token::Private,
                    "self" => Token::SelfKw,
                    "type" => Token::Type,
                    "import" => Token::Import,
                    "use" => Token::Use,
                    "as" => Token::As,
                    "request" => Token::Request,
                    "require" => Token::Require,
                    _ => Token::Ident(ident),
                };
                return tok;
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
}


#[cfg(test)]
mod tests {
    use super::token::Token;
    use super::Lexer;

    fn assert_tokens(input: &str, expected_tokens: Vec<Token>) {
        let mut l = Lexer::new(input);
        for (i, tt) in expected_tokens.iter().enumerate() {
            let tok = l.next_token();
            assert_eq!(
                tok, *tt,
                "tests[{}] - token wrong. expected={:?}, got={:?}",
                i, tt, tok
            );
        }
    }

    #[test]
    fn test_operators() {
        let input = " = + - * / % ! < > ++ -- == != <= >= += -= *= /= %= ,;(){}[]";
        let expected = vec![
            Token::Assign,
            Token::Plus,
            Token::Minus,
            Token::Asterisk,
            Token::Slash,
            Token::Percent,
            Token::Bang,
            Token::Lt,
            Token::Gt,
            Token::Increment,
            Token::Decrement,
            Token::Eq,
            Token::NotEq,
            Token::LtEq,
            Token::GtEq,
            Token::PlusAssign,
            Token::MinusAssign,
            Token::StarAssign,
            Token::SlashAssign,
            Token::PercentAssign,
            Token::Comma,
            Token::Semicolon,
            Token::LParen,
            Token::RParen,
            Token::LBrace,
            Token::RBrace,
            Token::LBracket,
            Token::RBracket,
            Token::Eof,
        ];
        assert_tokens(input, expected);
    }

    #[test]
    fn test_keywords() {
        let input = "fn let mut true false if else return while for switch case break fallthrough class struct enum public private self type import use as request require";
        let expected = vec![
            Token::Function,
            Token::Let,
            Token::Mut,
            Token::True,
            Token::False,
            Token::If,
            Token::Else,
            Token::Return,
            Token::While,
            Token::For,
            Token::Switch,
            Token::Case,
            Token::Break,
            Token::Fallthrough,
            Token::Class,
            Token::Struct,
            Token::Enum,
            Token::Public,
            Token::Private,
            Token::SelfKw,
            Token::Type,
            Token::Import,
            Token::Use,
            Token::As,
            Token::Request,
            Token::Require,
            Token::Eof,
        ];
        assert_tokens(input, expected);
    }

    #[test]
    fn test_literals_and_identifiers() {
        let input = r#"
            let var1 = 5;
            let var_2 = 10.0;
        "#;

        let expected = vec![
            Token::Let,
            Token::Ident("var1".to_string()),
            Token::Assign,
            Token::Int(5),
            Token::Semicolon,
            Token::Let,
            Token::Ident("var_2".to_string()),
            Token::Assign,
            Token::Float(10.0),
            Token::Semicolon,
            Token::Eof,
        ];

        assert_tokens(input, expected);
    }

    #[test]
    fn test_invalid_number() {
        let input = "9223372036854775808";
        let expected = vec![Token::Illegal, Token::Eof];
        assert_tokens(input, expected);
    }

    #[test]
    fn test_escaped_string() {
        let input = r#""hello \"world\"""#;
        let expected = vec![
            Token::String("hello \"world\"".to_string()),
            Token::Eof,
        ];
        assert_tokens(input, expected);
    }

    #[test]
    fn test_comprehensive_code() {
        let input = r#"
            let add = fn(x, y) {
                x + y;
            };
            let result = add(5, 10);
        "#;
        let expected = vec![
            Token::Let,
            Token::Ident("add".to_string()),
            Token::Assign,
            Token::Function,
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
            Token::Semicolon,
            Token::Let,
            Token::Ident("result".to_string()),
            Token::Assign,
            Token::Ident("add".to_string()),
            Token::LParen,
            Token::Int(5),
            Token::Comma,
            Token::Int(10),
            Token::RParen,
            Token::Semicolon,
            Token::Eof,
        ];
        assert_tokens(input, expected);
    }
}
