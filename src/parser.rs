use crate::lexer::Lexer;
use crate::token::Token;
use crate::ast::{self, Program, Statement};
use std::mem;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer,
            current_token: Token::Eof,
            peek_token: Token::Eof,
            errors: Vec::new(),
        };
        p.next_token();
        p.next_token();
        p
    }

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }

    fn peek_error(&mut self, t: Token) {
        let msg = format!("expected next token to be {:?}, got {:?} instead", t, self.peek_token);
        self.errors.push(msg);
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program {
            statements: Vec::new(),
        };

        while !self.current_token_is(Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                program.statements.push(stmt);
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<Box<dyn Statement>> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            _ => None,
        }
    }

    fn parse_let_statement(&mut self) -> Option<Box<dyn Statement>> {
        let let_token = self.current_token.clone();

        if !self.expect_peek_variant(Token::Identifier("".to_string())) {
            return None;
        }

        let name = match self.current_token.clone() {
            Token::Identifier(s) => ast::Identifier {
                token: self.current_token.clone(),
                value: s,
            },
            _ => unreachable!(),
        };

        if !self.expect_peek_variant(Token::Assign) {
            return None;
        }

        // For TDD, skip the expression part for now as the test doesn't check it.
        while !self.current_token_is(Token::Semicolon) {
            self.next_token();
            if self.current_token_is(Token::Eof) {
                 self.errors.push("unexpected EOF while parsing let statement".to_string());
                 return None;
            }
        }

        // Create a dummy expression to satisfy the struct definition
        let dummy_value = Box::new(ast::Identifier {
            token: Token::Identifier("dummy".to_string()),
            value: "dummy".to_string(),
        });

        let stmt = ast::LetStatement {
            token: let_token,
            name,
            value: dummy_value,
        };

        Some(Box::new(stmt))
    }

    fn current_token_is(&self, t: Token) -> bool {
        mem::discriminant(&self.current_token) == mem::discriminant(&t)
    }

    // Asserts the variant of the peek_token.
    // If it matches, it advances the tokens and returns true.
    // Otherwise, it adds an error and returns false.
    fn expect_peek_variant(&mut self, t: Token) -> bool {
        if mem::discriminant(&self.peek_token) == mem::discriminant(&t) {
            self.next_token();
            true
        } else {
            self.peek_error(t);
            false
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::{LetStatement, Node, Statement};
    use crate::lexer::Lexer;


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

        check_parser_errors(&parser);

        assert_eq!(program.statements.len(), 3, "program.statements does not contain 3 statements. got={}", program.statements.len());

        let expected_identifiers = vec!["x", "y", "foobar"];

        for (i, expected_identifier) in expected_identifiers.iter().enumerate() {
            let stmt = &program.statements[i];
            test_let_statement(stmt, expected_identifier);
        }
    }

    fn test_let_statement(stmt: &Box<dyn Statement>, name: &str) {
        assert_eq!(stmt.token_literal(), "let", "stmt.token_literal not 'let'. got={}", stmt.token_literal());

        let let_stmt = stmt.as_any().downcast_ref::<LetStatement>().expect("statement not a LetStatement");

        assert_eq!(let_stmt.name.value, name, "let_stmt.name.value not '{}'. got={}", name, let_stmt.name.value);
        assert_eq!(let_stmt.name.token_literal(), name, "let_stmt.name.token_literal() not '{}'. got={}", name, let_stmt.name.token_literal());
    }

    fn check_parser_errors(parser: &Parser) {
        if parser.errors().is_empty() {
            return;
        }

        eprintln!("Parser has {} errors", parser.errors().len());
        for msg in parser.errors() {
            eprintln!("Parser error: {}", msg);
        }
        panic!("Parser has errors");
    }
}