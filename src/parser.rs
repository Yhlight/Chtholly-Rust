use crate::lexer::Lexer;
use crate::token::Token;
use crate::ast::{self, Program, Statement, Expression};
use std::mem;

#[derive(PartialEq, PartialOrd)]
enum Precedence {
    Lowest,
    Equals,      // ==
    LessGreater, // > or <
    Sum,         // +
    Product,     // *
    Prefix,      // -X or !X
    Call,        // myFunction(X)
}

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

    fn no_prefix_parse_fn_error(&mut self, t: &Token) {
        let msg = format!("no prefix parse function for {:?} found", t);
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

        self.next_token();

        let value = match self.parse_expression(Precedence::Lowest) {
            Some(expr) => expr,
            None => {
                return None;
            }
        };

        if self.peek_token_is(Token::Semicolon) {
            self.next_token();
        }

        let stmt = ast::LetStatement {
            token: let_token,
            name,
            value,
        };

        Some(Box::new(stmt))
    }

    fn parse_expression(&mut self, _precedence: Precedence) -> Option<Box<dyn Expression>> {
        let token_clone = self.current_token.clone();
        let prefix = match token_clone {
            Token::Identifier(_) => Self::parse_identifier,
            Token::Int(_) => Self::parse_integer_literal,
            _ => {
                self.no_prefix_parse_fn_error(&token_clone);
                return None;
            }
        };

        prefix(self)
    }

    fn parse_identifier(&mut self) -> Option<Box<dyn Expression>> {
        if let Token::Identifier(s) = self.current_token.clone() {
            Some(Box::new(ast::Identifier {
                token: self.current_token.clone(),
                value: s,
            }))
        } else {
            None
        }
    }

    fn parse_integer_literal(&mut self) -> Option<Box<dyn Expression>> {
        if let Token::Int(val) = self.current_token {
            Some(Box::new(ast::IntegerLiteral {
                token: self.current_token.clone(),
                value: val,
            }))
        } else {
            None
        }
    }

    fn current_token_is(&self, t: Token) -> bool {
        mem::discriminant(&self.current_token) == mem::discriminant(&t)
    }

    fn peek_token_is(&self, t: Token) -> bool {
        mem::discriminant(&self.peek_token) == mem::discriminant(&t)
    }

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
    use crate::ast::{LetStatement, Node, Statement, Expression, IntegerLiteral};
    use crate::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let tests = vec![
            ("let x = 5;", "x", 5),
            ("let y = 10;", "y", 10),
            ("let foobar = 838383;", "foobar", 838383),
        ];

        for (input, expected_identifier, expected_value) in tests {
            let lexer = Lexer::new(input);
            let mut parser = Parser::new(lexer);
            let program = parser.parse_program();
            check_parser_errors(&parser);

            assert_eq!(program.statements.len(), 1);
            let stmt = &program.statements[0];
            test_let_statement(stmt, expected_identifier);
            let let_stmt = stmt.as_any().downcast_ref::<LetStatement>().unwrap();
            test_integer_literal(&let_stmt.value, expected_value);
        }
    }

    fn test_let_statement(stmt: &Box<dyn Statement>, name: &str) {
        assert_eq!(stmt.token_literal(), "let", "stmt.token_literal not 'let'. got={}", stmt.token_literal());

        let let_stmt = stmt.as_any().downcast_ref::<LetStatement>().expect("statement not a LetStatement");

        assert_eq!(let_stmt.name.value, name, "let_stmt.name.value not '{}'. got={}", name, let_stmt.name.value);
        assert_eq!(let_stmt.name.token_literal(), name, "let_stmt.name.token_literal() not '{}'. got={}", name, let_stmt.name.token_literal());
    }

    fn test_integer_literal(expr: &Box<dyn Expression>, value: i64) {
        let integ = expr.as_any().downcast_ref::<IntegerLiteral>().expect("expr not IntegerLiteral");

        assert_eq!(integ.value, value);
        assert_eq!(integ.token_literal(), value.to_string());
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

    #[test]
    fn test_let_statement_error_handling() {
        let input = "let x = ;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        parser.parse_program();

        assert_eq!(parser.errors().len(), 1, "Expected 1 error, but got {} {:?}", parser.errors().len(), parser.errors());
    }
}