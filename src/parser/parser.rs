use crate::ast::ast::{Expression, LetStatement, Program, Statement};
use crate::lexer::lexer::Lexer;
use crate::lexer::token::Token;
use std::collections::HashMap;

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

type PrefixParseFn = fn(&mut Parser) -> Option<Expression>;

pub struct Parser {
    lexer: Lexer,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
    prefix_parse_fns: HashMap<std::mem::Discriminant<Token>, PrefixParseFn>,
}

impl Parser {
    pub fn new(lexer: Lexer) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::Illegal,
            peek_token: Token::Illegal,
            errors: Vec::new(),
            prefix_parse_fns: HashMap::new(),
        };
        parser.register_prefix(Token::Integer(0), Parser::parse_integer_literal);
        parser.register_prefix(Token::True, Parser::parse_boolean_literal);
        parser.register_prefix(Token::False, Parser::parse_boolean_literal);
        parser.register_prefix(Token::Bang, Parser::parse_prefix_expression);
        parser.register_prefix(Token::Minus, Parser::parse_prefix_expression);

        parser.next_token();
        parser.next_token();
        parser
    }

    fn register_prefix(&mut self, token: Token, function: PrefixParseFn) {
        self.prefix_parse_fns
            .insert(std::mem::discriminant(&token), function);
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
        let is_mutable = if self.peek_token == Token::Mut {
            self.next_token();
            true
        } else {
            false
        };

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

        self.next_token();

        let value = match self.parse_expression(Precedence::Lowest) {
            Some(expr) => expr,
            None => {
                let msg = format!("expected expression, got nothing");
                self.errors.push(msg);
                return None;
            }
        };

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Let(LetStatement {
            name: crate::ast::ast::Identifier { value: name },
            value,
            is_mutable,
        }))
    }

    fn parse_expression(&mut self, _precedence: Precedence) -> Option<Expression> {
        let discriminant = std::mem::discriminant(&self.current_token);
        if let Some(prefix_fn) = self.prefix_parse_fns.get(&discriminant) {
            prefix_fn(self)
        } else {
            None
        }
    }

    fn parse_boolean_literal(&mut self) -> Option<Expression> {
        match self.current_token {
            Token::True => Some(Expression::BooleanLiteral(true)),
            Token::False => Some(Expression::BooleanLiteral(false)),
            _ => None,
        }
    }

    fn parse_prefix_expression(&mut self) -> Option<Expression> {
        let operator = match self.current_token {
            Token::Bang => "!".to_string(),
            Token::Minus => "-".to_string(),
            _ => return None,
        };

        self.next_token();

        let right = match self.parse_expression(Precedence::Prefix) {
            Some(expr) => expr,
            None => {
                let msg = format!("expected expression, got nothing");
                self.errors.push(msg);
                return None;
            }
        };

        Some(Expression::PrefixExpression {
            operator,
            right: Box::new(right),
        })
    }

    fn parse_integer_literal(&mut self) -> Option<Expression> {
        if let Token::Integer(value) = self.current_token {
            Some(Expression::IntegerLiteral(value))
        } else {
            None
        }
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
    use crate::ast::ast::Statement;
    use crate::lexer::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let input = r#"
            let x = 5;
            let mut y = 10;
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

        let tests = vec![
            ("x", 5, false),
            ("y", 10, true),
            ("foobar", 838383, false),
        ];

        for (i, (expected_identifier, expected_value, expected_mutable)) in tests.iter().enumerate()
        {
            let statement = &program.statements[i];
            if let Statement::Let(let_statement) = statement {
                assert_eq!(
                    let_statement.name.value, *expected_identifier,
                    "let_statement.name.value not '{}'. got={}",
                    expected_identifier, let_statement.name.value
                );

                assert_eq!(
                    let_statement.is_mutable, *expected_mutable,
                    "let_statement.is_mutable not {}. got={}",
                    expected_mutable, let_statement.is_mutable
                );

                if let Expression::IntegerLiteral(value) = let_statement.value {
                    assert_eq!(
                        value, *expected_value,
                        "let_statement.value not {}. got={}",
                        expected_value, value
                    );
                } else {
                    panic!(
                        "let_statement.value not an IntegerLiteral. got={:?}",
                        let_statement.value
                    );
                }
            } else {
                panic!("statement not a LetStatement. got={:?}", statement);
            }
        }
    }

    #[test]
    fn test_let_statement_errors() {
        let input = r#"
            let x = ;
            let = 10;
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        parser.parse_program();

        assert_eq!(
            parser.errors.len(),
            2,
            "parser has wrong number of errors. got={}",
            parser.errors.len()
        );
    }

    #[test]
    fn test_boolean_literal_expression() {
        let input = "let x = true;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert_eq!(program.statements.len(), 1);

        let stmt = &program.statements[0];
        if let Statement::Let(let_stmt) = stmt {
            if let Expression::BooleanLiteral(val) = let_stmt.value {
                assert_eq!(val, true);
            } else {
                panic!("not a boolean literal");
            }
        } else {
            panic!("not a let statement");
        }
    }

    #[test]
    fn test_prefix_expressions() {
        enum ExpectedRhs {
            Integer(i64),
            Boolean(bool),
        }

        let prefix_tests = vec![
            ("let x = !5;", "!", ExpectedRhs::Integer(5)),
            ("let x = -15;", "-", ExpectedRhs::Integer(15)),
            ("let x = !true;", "!", ExpectedRhs::Boolean(true)),
            ("let x = !false;", "!", ExpectedRhs::Boolean(false)),
        ];

        for (input, operator, expected_rhs) in prefix_tests {
            let lexer = Lexer::new(input);
            let mut parser = Parser::new(lexer);
            let program = parser.parse_program();
            assert_eq!(program.statements.len(), 1);

            let stmt = &program.statements[0];
            if let Statement::Let(let_stmt) = stmt {
                if let Expression::PrefixExpression {
                    operator: op,
                    right,
                } = &let_stmt.value
                {
                    assert_eq!(op, operator);
                    match expected_rhs {
                        ExpectedRhs::Integer(val) => {
                            if let Expression::IntegerLiteral(int_val) = **right {
                                assert_eq!(int_val, val);
                            } else {
                                panic!("expected integer literal");
                            }
                        }
                        ExpectedRhs::Boolean(val) => {
                            if let Expression::BooleanLiteral(bool_val) = **right {
                                assert_eq!(bool_val, val);
                            } else {
                                panic!("expected boolean literal");
                            }
                        }
                    }
                } else {
                    panic!("not a prefix expression");
                }
            } else {
                panic!("not a let statement");
            }
        }
    }
}
