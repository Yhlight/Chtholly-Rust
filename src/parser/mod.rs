//! The Parser for the Chtholly language.

use crate::ast::{self, Node, Program, Statement, LetStatement, Identifier, Expression, IntegerLiteral, FloatLiteral, StringLiteral, Boolean};
use crate::lexer::token::Token;
use crate::lexer::Lexer;

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

/// The Parser struct holds the state of the parser.
pub struct Parser<'a> {
    lexer: Lexer<'a>,
    cur_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl<'a> Parser<'a> {
    /// Creates a new Parser.
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer,
            cur_token: Token::Illegal,
            peek_token: Token::Illegal,
            errors: Vec::new(),
        };
        p.next_token();
        p.next_token();
        p
    }

    /// Advances the tokens in the stream.
    fn next_token(&mut self) {
        self.cur_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    /// Returns the list of parsing errors.
    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program {
            statements: Vec::new(),
        };
        while self.cur_token != Token::Eof {
            if let Some(stmt) = self.parse_statement() {
                program.statements.push(stmt);
            }
            self.next_token();
        }
        program
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.cur_token {
            Token::Let => self.parse_let_statement().map(Statement::Let),
            _ => None,
        }
    }

    fn parse_let_statement(&mut self) -> Option<LetStatement> {
        let token = self.cur_token.clone();

        if !self.expect_peek(Token::Ident("".to_string())) {
            return None;
        }

        let name = Identifier {
            token: self.cur_token.clone(),
            value: match &self.cur_token {
                Token::Ident(s) => s.clone(),
                _ => return None,
            },
        };

        if !self.expect_peek(Token::Assign) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(LetStatement { token, name, value })
    }

    fn parse_expression(&mut self, _precedence: Precedence) -> Option<Expression> {
        match &self.cur_token {
            Token::Ident(_) => self.parse_identifier(),
            Token::Int(_) => self.parse_integer_literal(),
            // Add other expression types here
            _ => None,
        }
    }

    fn parse_identifier(&mut self) -> Option<Expression> {
        match &self.cur_token {
            Token::Ident(value) => Some(Expression::Identifier(Identifier {
                token: self.cur_token.clone(),
                value: value.clone(),
            })),
            _ => None,
        }
    }

    fn parse_integer_literal(&mut self) -> Option<Expression> {
        match self.cur_token {
            Token::Int(value) => Some(Expression::IntegerLiteral(IntegerLiteral {
                token: self.cur_token.clone(),
                value,
            })),
            _ => None,
        }
    }


    fn peek_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.peek_token) == std::mem::discriminant(t)
    }

    fn expect_peek(&mut self, t: Token) -> bool {
        if self.peek_token_is(&t) {
            self.next_token();
            true
        } else {
            self.peek_error(&t);
            false
        }
    }

    fn peek_error(&mut self, t: &Token) {
        let msg = format!(
            "expected next token to be {:?}, got {:?} instead",
            t, self.peek_token
        );
        self.errors.push(msg);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::Node;
    use crate::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let input = "
let x = 5;
let y = 10;
let foobar = 838383;
";

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        check_parser_errors(&parser);

        assert_eq!(
            program.statements.len(),
            3,
            "program.statements does not contain 3 statements. got={}",
            program.statements.len()
        );

        let tests = vec![
            ("x", 5),
            ("y", 10),
            ("foobar", 838383),
        ];

        for (i, (expected_ident, expected_val)) in tests.iter().enumerate() {
            let stmt = &program.statements[i];
            assert_let_statement(stmt, expected_ident);

            if let Statement::Let(let_stmt) = stmt {
                assert_literal_expression(&let_stmt.value, expected_val);
            }
        }
    }

    fn assert_let_statement(s: &Statement, name: &str) {
        assert_eq!(s.token_literal(), "let");

        if let Statement::Let(let_stmt) = s {
            assert_eq!(let_stmt.name.value, name);
            assert_eq!(let_stmt.name.token_literal(), name);
        } else {
            panic!("s not Statement::Let. got={:?}", s);
        }
    }

    fn assert_literal_expression<T>(exp: &Expression, expected: T)
    where
        T: std::fmt::Display + PartialEq,
    {
        match exp {
            Expression::IntegerLiteral(il) => {
                assert_eq!(il.value.to_string(), expected.to_string());
            }
            // Add other literal types here
            _ => panic!("Expression not a literal. got={:?}", exp),
        }
    }

    fn check_parser_errors(parser: &Parser) {
        let errors = parser.errors();
        if errors.is_empty() {
            return;
        }

        eprintln!("parser has {} errors", errors.len());
        for msg in errors {
            eprintln!("parser error: {}", msg);
        }
        panic!("parser has errors");
    }
}
