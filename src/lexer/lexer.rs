use crate::lexer::token::Token;

pub struct Lexer {
    input: Vec<char>,
    position: usize,
    read_position: usize,
    ch: char,
}

impl Lexer {
    pub fn new(input: &str) -> Self {
        let mut lexer = Lexer {
            input: input.chars().collect(),
            position: 0,
            read_position: 0,
            ch: '\0',
        };
        lexer.read_char();
        lexer
    }

    fn read_char(&mut self) {
        if self.read_position >= self.input.len() {
            self.ch = '\0';
        } else {
            self.ch = self.input[self.read_position];
        }
        self.position = self.read_position;
        self.read_position += 1;
    }

    fn peek_char(&self) -> char {
        if self.read_position >= self.input.len() {
            '\0'
        } else {
            self.input[self.read_position]
        }
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_whitespace() {
            self.read_char();
        }
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        let token = match self.ch {
            '=' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::EqualEqual
                } else {
                    Token::Equal
                }
            }
            '!' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::BangEqual
                } else {
                    Token::Bang
                }
            }
            '+' => Token::Plus,
            '-' => Token::Minus,
            '*' => Token::Asterisk,
            '/' => Token::Slash,
            '%' => Token::Percent,
            '<' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::LessThanEqual
                } else {
                    Token::LessThan
                }
            }
            '>' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::GreaterThanEqual
                } else {
                    Token::GreaterThan
                }
            }
            '&' => {
                if self.peek_char() == '&' {
                    self.read_char();
                    Token::AndAnd
                } else {
                    Token::And
                }
            }
            '|' => {
                if self.peek_char() == '|' {
                    self.read_char();
                    Token::OrOr
                } else {
                    Token::Or
                }
            }
            '^' => Token::Caret,
            '~' => Token::Tilde,
            '(' => Token::LParen,
            ')' => Token::RParen,
            '{' => Token::LBrace,
            '}' => Token::RBrace,
            '[' => Token::LBracket,
            ']' => Token::RBracket,
            ',' => Token::Comma,
            ':' => Token::Colon,
            ';' => Token::Semicolon,
            '.' => Token::Dot,
            '\0' => Token::Eof,
            _ => {
                if self.ch.is_alphabetic() || self.ch == '_' {
                    let identifier = self.read_identifier();
                    return match identifier.as_str() {
                        "fn" => Token::Fn,
                        "let" => Token::Let,
                        "mut" => Token::Mut,
                        "if" => Token::If,
                        "else" => Token::Else,
                        "switch" => Token::Switch,
                        "case" => Token::Case,
                        "while" => Token::While,
                        "for" => Token::For,
                        "do" => Token::Do,
                        "return" => Token::Return,
                        "class" => Token::Class,
                        "struct" => Token::Struct,
                        "enum" => Token::Enum,
                        "public" => Token::Public,
                        "private" => Token::Private,
                        "const" => Token::Const,
                        "true" => Token::True,
                        "false" => Token::False,
                        _ => Token::Identifier(identifier),
                    };
                } else if self.ch.is_digit(10) {
                    return self.read_number();
                } else {
                    Token::Illegal
                }
            }
        };

        self.read_char();
        token
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while self.ch.is_alphanumeric() || self.ch == '_' {
            self.read_char();
        }
        self.input[position..self.position].iter().collect()
    }

    fn read_number(&mut self) -> Token {
        let position = self.position;
        let mut is_float = false;
        while self.ch.is_digit(10) || self.ch == '.' {
            if self.ch == '.' {
                is_float = true;
            }
            self.read_char();
        }
        let number: String = self.input[position..self.position].iter().collect();
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
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::token::Token;

    #[test]
    fn test_next_token() {
        let input = r#"
            let five = 5;
            let ten = 10;
            fn add(x, y) {
                x + y;
            }
            let result = add(five, ten);
            !-/*5;
            5 < 10 > 5;
            if (5 < 10) {
                return true;
            } else {
                return false;
            }
            10 == 10;
            10 != 9;
        "#;

        let tests = vec![
            Token::Let,
            Token::Identifier("five".to_string()),
            Token::Equal,
            Token::Integer(5),
            Token::Semicolon,
            Token::Let,
            Token::Identifier("ten".to_string()),
            Token::Equal,
            Token::Integer(10),
            Token::Semicolon,
            Token::Fn,
            Token::Identifier("add".to_string()),
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
            Token::Let,
            Token::Identifier("result".to_string()),
            Token::Equal,
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
            Token::Integer(5),
            Token::Semicolon,
            Token::Integer(5),
            Token::LessThan,
            Token::Integer(10),
            Token::GreaterThan,
            Token::Integer(5),
            Token::Semicolon,
            Token::If,
            Token::LParen,
            Token::Integer(5),
            Token::LessThan,
            Token::Integer(10),
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
            Token::Integer(10),
            Token::EqualEqual,
            Token::Integer(10),
            Token::Semicolon,
            Token::Integer(10),
            Token::BangEqual,
            Token::Integer(9),
            Token::Semicolon,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input);

        for (i, tt) in tests.iter().enumerate() {
            let tok = lexer.next_token();
            assert_eq!(*tt, tok, "tests[{}] - tokentype wrong.", i);
        }
    }
}
