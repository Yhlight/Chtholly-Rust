use crate::ast::{self, Program};
use crate::lexer::Lexer;
use crate::lexer::token::Token;

#[derive(PartialEq, PartialOrd)]
#[allow(dead_code)]
enum Precedence {
    LOWEST,
    EQUALS,      // ==
    LESSGREATER, // > or <
    SUM,         // +
    PRODUCT,     // *
    PREFIX,      // -X or !X
    CALL,        // myFunction(X)
}

pub struct Parser {
    lexer: Lexer,
    current_token: Token,
    peek_token: Token,
}

impl Parser {
    pub fn new(mut lexer: Lexer) -> Self {
        let current_token = lexer.next_token();
        let peek_token = lexer.next_token();
        Parser {
            lexer,
            current_token,
            peek_token,
        }
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program {
            statements: Vec::new(),
        };

        while self.current_token != Token::Eof {
            if let Some(statement) = self.parse_statement() {
                program.statements.push(statement);
            }
            self.next_token();
        }

        program
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    fn parse_statement(&mut self) -> Option<Box<dyn ast::Statement>> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            _ => None,
        }
    }

    fn parse_let_statement(&mut self) -> Option<Box<dyn ast::Statement>> {
        let let_token = self.current_token.clone();

        if !self.expect_peek(Token::Identifier("".to_string())) {
            return None;
        }

        let name = ast::Identifier {
            token: self.current_token.clone(),
            value: match &self.current_token {
                Token::Identifier(s) => s.clone(),
                _ => return None,
            },
        };

        if !self.expect_peek(Token::Assign) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::LOWEST).unwrap();

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Box::new(ast::LetStatement {
            token: let_token,
            name,
            value,
        }))
    }

    fn parse_expression(&mut self, _precedence: Precedence) -> Option<Box<dyn ast::Expression>> {
        match &self.current_token {
            Token::Identifier(_) => Some(Box::new(ast::Identifier {
                token: self.current_token.clone(),
                value: match &self.current_token {
                    Token::Identifier(s) => s.clone(),
                    _ => return None,
                },
            })),
            Token::IntLiteral(s) => {
                let value = s.parse::<i64>().unwrap();
                Some(Box::new(ast::IntegerLiteral {
                    token: self.current_token.clone(),
                    value,
                }))
            }
            _ => None,
        }
    }

    fn expect_peek(&mut self, token: Token) -> bool {
        if matches!(&self.peek_token, t if std::mem::discriminant(t) == std::mem::discriminant(&token)) {
            self.next_token();
            true
        } else {
            false
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let input = r#"
            let x = 5;
            let y = 10;
            let foobar = 838383;
        "#;

        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(
            program.statements.len(),
            3,
            "program.statements does not contain 3 statements. got={}",
            program.statements.len()
        );

        let tests = [
            ("x", "5"),
            ("y", "10"),
            ("foobar", "838383"),
        ];

        for (i, tt) in tests.iter().enumerate() {
            let stmt = &program.statements[i];
            let let_stmt = stmt
                .as_any()
                .downcast_ref::<ast::LetStatement>()
                .unwrap();
            assert_eq!(
                let_stmt.name.value,
                tt.0,
                "let_stmt.name.value not '{}'. got={}",
                tt.0,
                let_stmt.name.value
            );
            assert_eq!(
                let_stmt.value.token_literal(),
                tt.1,
                "let_stmt.value.token_literal() not '{}'. got={}",
                tt.1,
                let_stmt.value.token_literal()
            );
        }
    }
}
