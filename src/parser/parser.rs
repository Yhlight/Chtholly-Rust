use crate::ast::ast::{LetStatement, Program, Statement};
use crate::lexer::lexer::Lexer;
use crate::lexer::token::Token;

pub struct Parser {
    lexer: Lexer,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl Parser {
    pub fn new(lexer: Lexer) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::Illegal,
            peek_token: Token::Illegal,
            errors: Vec::new(),
        };
        parser.next_token();
        parser.next_token();
        parser
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
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

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            // ... other statements
            _ => None,
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        if !self.expect_peek(Token::Identifier("".to_string())) {
            return None;
        }

        let name = match &self.current_token {
            Token::Identifier(name) => name.clone(),
            _ => {
                let msg = format!("expected identifier, got {:?}", self.current_token);
                self.errors.push(msg);
                return None;
            }
        };

        if !self.expect_peek(Token::Equal) {
            return None;
        }

        // TODO: We're skipping the expression parsing for now
        while self.current_token != Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Let(LetStatement {
            name: crate::ast::ast::Identifier { value: name },
            value: crate::ast::ast::Expression::Identifier(
                crate::ast::ast::Identifier {
                    value: "dummy".to_string(),
                },
            ),
        }))
    }

    fn expect_peek(&mut self, token: Token) -> bool {
        if std::mem::discriminant(&self.peek_token) == std::mem::discriminant(&token) {
            self.next_token();
            true
        } else {
            self.peek_error(token);
            false
        }
    }

    fn peek_error(&mut self, token: Token) {
        let msg = format!(
            "expected next token to be {:?}, got {:?} instead",
            token, self.peek_token
        );
        self.errors.push(msg);
    }

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::ast::{Node, Statement};
    use crate::lexer::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let input = r#"
            let x = 5;
            let y = 10;
            let foobar = 838383;
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(
            program.statements.len(),
            3,
            "program.statements does not contain 3 statements. got={}",
            program.statements.len()
        );

        let expected_identifiers = vec!["x", "y", "foobar"];

        for (i, expected_identifier) in expected_identifiers.iter().enumerate() {
            let statement = &program.statements[i];
            if let Statement::Let(let_statement) = statement {
                assert_eq!(
                    let_statement.name.value, *expected_identifier,
                    "let_statement.name.value not '{}'. got={}",
                    expected_identifier, let_statement.name.value
                );
            } else {
                panic!("statement not a LetStatement. got={:?}", statement);
            }
        }
    }
}
