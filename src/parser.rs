use crate::lexer::{Lexer, Token};
use crate::ast::*;

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

fn token_to_precedence(token: &Token) -> Precedence {
    match token {
        Token::Equal | Token::NotEqual => Precedence::Equals,
        Token::LessThan | Token::GreaterThan => Precedence::LessGreater,
        Token::Plus | Token::Minus => Precedence::Sum,
        Token::Multiply | Token::Divide => Precedence::Product,
        _ => Precedence::Lowest,
    }
}

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::Eof,
            peek_token: Token::Eof,
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

    pub fn parse_program(&mut self) -> Result<Program, Vec<String>> {
        let mut program = Program::new();

        while self.current_token != Token::Eof {
            if let Some(statement) = self.parse_statement() {
                program.statements.push(statement);
            }
            self.next_token();
        }

        if self.errors.is_empty() {
            Ok(program)
        } else {
            Err(self.errors.clone())
        }
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            Token::Return => self.parse_return_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        let mut is_mut = false;
        if self.peek_token == Token::Mut {
            is_mut = true;
            self.next_token();
        }

        if !self.expect_peek(&Token::Identifier("".to_string())) {
            return None;
        }

        let name = match self.current_token.clone() {
            Token::Identifier(name) => Identifier(name),
            _ => return None,
        };

        if !self.expect_peek(&Token::Assign) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Let(name, is_mut, value))
    }

    fn parse_return_statement(&mut self) -> Option<Statement> {
        self.next_token();

        let return_value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Return(return_value))
    }

    fn parse_expression_statement(&mut self) -> Option<Statement> {
        let expression = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Expression(expression))
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let mut left_exp = self.parse_prefix()?;

        while self.peek_token != Token::Semicolon && precedence < token_to_precedence(&self.peek_token) {
            self.next_token();
            left_exp = self.parse_infix(left_exp)?;
        }

        Some(left_exp)
    }

    fn parse_prefix(&mut self) -> Option<Expression> {
        match self.current_token.clone() {
            Token::Identifier(name) => Some(Expression::Identifier(Identifier(name))),
            Token::Integer(value) => Some(Expression::Literal(Literal::Integer(value))),
            Token::Float(value) => Some(Expression::Literal(Literal::Float(value))),
            Token::String(value) => Some(Expression::Literal(Literal::String(value))),
            Token::True => Some(Expression::Literal(Literal::Boolean(true))),
            Token::False => Some(Expression::Literal(Literal::Boolean(false))),
            Token::Minus => self.parse_prefix_expression(),
            _ => None,
        }
    }

    fn parse_infix(&mut self, left: Expression) -> Option<Expression> {
        let operator = match self.current_token {
            Token::Plus => InfixOperator::Plus,
            Token::Minus => InfixOperator::Minus,
            Token::Multiply => InfixOperator::Multiply,
            Token::Divide => InfixOperator::Divide,
            Token::Equal => InfixOperator::Equal,
            Token::NotEqual => InfixOperator::NotEqual,
            Token::LessThan => InfixOperator::LessThan,
            Token::GreaterThan => InfixOperator::GreaterThan,
            _ => return None,
        };

        let precedence = token_to_precedence(&self.current_token);
        self.next_token();
        let right = self.parse_expression(precedence)?;

        Some(Expression::Infix(Box::new(left), operator, Box::new(right)))
    }

    fn parse_prefix_expression(&mut self) -> Option<Expression> {
        let operator = match self.current_token {
            Token::Minus => PrefixOperator::Minus,
            _ => return None,
        };

        self.next_token();

        let right = self.parse_expression(Precedence::Prefix)?;

        Some(Expression::Prefix(operator, Box::new(right)))
    }

    fn expect_peek(&mut self, token: &Token) -> bool {
        if std::mem::discriminant(&self.peek_token) == std::mem::discriminant(token) {
            self.next_token();
            true
        } else {
            self.peek_error(token);
            false
        }
    }

    fn peek_error(&mut self, token: &Token) {
        let msg = format!("expected next token to be {:?}, got {:?} instead", token, self.peek_token);
        self.errors.push(msg);
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

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program().unwrap();

        assert_eq!(program.statements.len(), 3);

        let tests = vec!["x", "y", "foobar"];

        for (i, test) in tests.iter().enumerate() {
            let stmt = &program.statements[i];
            if let Statement::Let(Identifier(name), _, _) = stmt {
                assert_eq!(name, test);
            } else {
                panic!("statement not a let statement");
            }
        }
    }

    #[test]
    fn test_prefix_expressions() {
        let prefix_tests = vec![
            ("-15;", PrefixOperator::Minus, 15),
        ];

        for (input, operator, value) in prefix_tests {
            let lexer = Lexer::new(input);
            let mut parser = Parser::new(lexer);
            let program = parser.parse_program().unwrap();

            assert_eq!(program.statements.len(), 1);

            if let Statement::Expression(Expression::Prefix(op, exp)) = &program.statements[0] {
                assert_eq!(*op, operator);
                if let Expression::Literal(Literal::Integer(val)) = **exp {
                    assert_eq!(val, value);
                } else {
                    panic!("expression not an integer literal");
                }
            } else {
                panic!("statement not a prefix expression");
            }
        }
    }

    #[test]
    fn test_infix_expressions() {
        let infix_tests = vec![
            ("5 + 5;", 5, InfixOperator::Plus, 5),
            ("5 - 5;", 5, InfixOperator::Minus, 5),
            ("5 * 5;", 5, InfixOperator::Multiply, 5),
            ("5 / 5;", 5, InfixOperator::Divide, 5),
        ];

        for (input, left_value, operator, right_value) in infix_tests {
            let lexer = Lexer::new(input);
            let mut parser = Parser::new(lexer);
            let program = parser.parse_program().unwrap();

            assert_eq!(program.statements.len(), 1);

            if let Statement::Expression(Expression::Infix(left, op, right)) = &program.statements[0] {
                if let Expression::Literal(Literal::Integer(val)) = **left {
                    assert_eq!(val, left_value);
                } else {
                    panic!("left expression not an integer literal");
                }
                assert_eq!(*op, operator);
                if let Expression::Literal(Literal::Integer(val)) = **right {
                    assert_eq!(val, right_value);
                } else {
                    panic!("right expression not an integer literal");
                }
            } else {
                panic!("statement not an infix expression");
            }
        }
    }

    #[test]
    fn test_return_statements() {
        let input = r#"
            return 5;
            return 10;
            return 993322;
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program().unwrap();

        assert_eq!(program.statements.len(), 3);

        for stmt in program.statements {
            if let Statement::Return(_) = stmt {
                // ok
            } else {
                panic!("statement not a return statement");
            }
        }
    }

    #[test]
    fn test_let_mut_statements() {
        let input = r#"
            let mut x = 5;
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program().unwrap();

        assert_eq!(program.statements.len(), 1);

        let stmt = &program.statements[0];
        if let Statement::Let(Identifier(name), is_mut, _) = stmt {
            assert_eq!(name, "x");
            assert_eq!(*is_mut, true);
        } else {
            panic!("statement not a let statement");
        }
    }

    #[test]
    fn test_parser_errors() {
        let input = r#"
            let x 5;
            let = 10;
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let result = parser.parse_program();

        assert!(result.is_err());

        let errors = result.unwrap_err();
        assert_eq!(errors.len(), 2);
    }
}
