pub mod token;

use token::Token;

pub struct Lexer {
    input: Vec<char>,
    position: usize,
    read_position: usize,
    ch: char,
}

impl Lexer {
    pub fn new(input: String) -> Self {
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

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        let token = match self.ch {
            '=' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::Equal
                } else {
                    Token::Assign
                }
            }
            '+' => {
                if self.peek_char() == '+' {
                    self.read_char();
                    Token::Increment
                } else if self.peek_char() == '=' {
                    self.read_char();
                    Token::PlusAssign
                } else {
                    Token::Plus
                }
            }
            '-' => {
                if self.peek_char() == '-' {
                    self.read_char();
                    Token::Decrement
                } else if self.peek_char() == '=' {
                    self.read_char();
                    Token::MinusAssign
                } else {
                    Token::Minus
                }
            }
            '!' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::NotEqual
                } else {
                    Token::Bang
                }
            }
            '/' => {
                if self.peek_char() == '/' {
                    // Comment, skip to end of line
                    while self.ch != '\n' && self.ch != '\0' {
                        self.read_char();
                    }
                    return self.next_token();
                } else if self.peek_char() == '*' {
                    // Block comment, skip to */
                    self.read_char();
                    self.read_char();
                    while !(self.ch == '*' && self.peek_char() == '/') && self.ch != '\0' {
                        self.read_char();
                    }
                    self.read_char();
                    self.read_char();
                    return self.next_token();

                } else if self.peek_char() == '=' {
                    self.read_char();
                    Token::SlashAssign
                } else {
                    Token::Slash
                }
            }
            '*' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::AsteriskAssign
                } else {
                    Token::Asterisk
                }
            }
            '%' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::PercentAssign
                } else {
                    Token::Percent
                }
            }
            '<' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::LessThanOrEqual
                } else if self.peek_char() == '<' {
                    self.read_char();
                    Token::ShiftLeft
                } else {
                    Token::LessThan
                }
            }
            '>' => {
                if self.peek_char() == '=' {
                    self.read_char();
                    Token::GreaterThanOrEqual
                } else if self.peek_char() == '>' {
                    self.read_char();
                    Token::ShiftRight
                } else {
                    Token::GreaterThan
                }
            }
            '&' => {
                if self.peek_char() == '&' {
                    self.read_char();
                    Token::And
                } else {
                    Token::BitwiseAnd
                }
            }
            '|' => {
                if self.peek_char() == '|' {
                    self.read_char();
                    Token::Or
                } else {
                    Token::BitwiseOr
                }
            }
            '^' => Token::BitwiseXor,
            '~' => Token::BitwiseNot,
            ';' => Token::Semicolon,
            ':' => Token::Colon,
            ',' => Token::Comma,
            '(' => Token::LParen,
            ')' => Token::RParen,
            '{' => Token::LBrace,
            '}' => Token::RBrace,
            '[' => Token::LBracket,
            ']' => Token::RBracket,
            '"' => self.read_string(),
            '\'' => self.read_char_literal(),
            '\0' => Token::Eof,
            _ => {
                if self.ch.is_alphabetic() {
                    return self.read_identifier();
                } else if self.ch.is_digit(10) {
                    return self.read_number();
                } else {
                    Token::Illegal(self.ch.to_string())
                }
            }
        };
        self.read_char();
        token
    }

    fn peek_char(&self) -> char {
        if self.read_position >= self.input.len() {
            '\0'
        } else {
            self.input[self.read_position]
        }
    }

    fn read_identifier(&mut self) -> Token {
        let position = self.position;
        while self.ch.is_alphanumeric() || self.ch == '_' {
            self.read_char();
        }
        let ident: String = self.input[position..self.position].iter().collect();
        match ident.as_str() {
            "fn" => Token::Function,
            "let" => Token::Let,
            "mut" => Token::Mut,
            "true" => Token::True,
            "false" => Token::False,
            "if" => Token::If,
            "else" => Token::Else,
            "return" => Token::Return,
            "class" => Token::Class,
            "struct" => Token::Struct,
            "enum" => Token::Enum,
            "public" => Token::Public,
            "private" => Token::Private,
            "switch" => Token::Switch,
            "case" => Token::Case,
            "for" => Token::For,
            "while" => Token::While,
            "do" => Token::Do,
            "fallthrough" => Token::Fallthrough,
            "Self" => Token::SelfKeyword,
            "i8" | "i16" | "i32" | "i64" | "u8" | "u16" | "u32" | "u64" | "f32" | "f64" | "char" | "void" | "bool" | "string" => Token::Type(ident),
            _ => Token::Identifier(ident),
        }
    }

    fn read_number(&mut self) -> Token {
        let position = self.position;
        let mut is_float = false;
        while self.ch.is_digit(10) {
            self.read_char();
        }

        if self.ch == '.' && self.peek_char().is_digit(10) {
            is_float = true;
            self.read_char();
            while self.ch.is_digit(10) {
                self.read_char();
            }
        }

        let number: String = self.input[position..self.position].iter().collect();
        if is_float {
            Token::FloatLiteral(number)
        } else {
            Token::IntLiteral(number)
        }
    }

    fn read_string(&mut self) -> Token {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == '"' || self.ch == '\0' {
                break;
            }
        }
        Token::StringLiteral(self.input[position..self.position].iter().collect())
    }

    fn read_char_literal(&mut self) -> Token {
        self.read_char(); // Consume opening '
        let ch = self.ch;
        self.read_char(); // Consume the character
        if self.ch != '\'' {
            // If the next character is not a closing quote, it's a malformed literal.
            return Token::Illegal(ch.to_string());
        }
        Token::CharLiteral(ch)
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_whitespace() {
            self.read_char();
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use super::token::Token;

    #[test]
    fn test_next_token() {
        let input = r#"
            let five = 5;
            let ten = 10.0;

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
            "foobar"
            "foo bar"
            'a'
            // this is a comment
            /* this is a
               block comment */
            mut mut_var = 1;
            mut_var += 1;
        "#;

        let tests = vec![
            Token::Let,
            Token::Identifier("five".to_string()),
            Token::Assign,
            Token::IntLiteral("5".to_string()),
            Token::Semicolon,
            Token::Let,
            Token::Identifier("ten".to_string()),
            Token::Assign,
            Token::FloatLiteral("10.0".to_string()),
            Token::Semicolon,
            Token::Let,
            Token::Identifier("add".to_string()),
            Token::Assign,
            Token::Function,
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
            Token::IntLiteral("5".to_string()),
            Token::Semicolon,
            Token::IntLiteral("5".to_string()),
            Token::LessThan,
            Token::IntLiteral("10".to_string()),
            Token::GreaterThan,
            Token::IntLiteral("5".to_string()),
            Token::Semicolon,
            Token::If,
            Token::LParen,
            Token::IntLiteral("5".to_string()),
            Token::LessThan,
            Token::IntLiteral("10".to_string()),
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
            Token::IntLiteral("10".to_string()),
            Token::Equal,
            Token::IntLiteral("10".to_string()),
            Token::Semicolon,
            Token::IntLiteral("10".to_string()),
            Token::NotEqual,
            Token::IntLiteral("9".to_string()),
            Token::Semicolon,
            Token::StringLiteral("foobar".to_string()),
            Token::StringLiteral("foo bar".to_string()),
            Token::CharLiteral('a'),
            Token::Mut,
            Token::Identifier("mut_var".to_string()),
            Token::Assign,
            Token::IntLiteral("1".to_string()),
            Token::Semicolon,
            Token::Identifier("mut_var".to_string()),
            Token::PlusAssign,
            Token::IntLiteral("1".to_string()),
            Token::Semicolon,
            Token::Eof,
        ];

        let mut lexer = Lexer::new(input.to_string());

        for (i, tt) in tests.iter().enumerate() {
            let tok = lexer.next_token();
            assert_eq!(*tt, tok, "tests[{}] - token literal wrong.", i);
        }
    }
}
