//! Parser for the Chtholly language.

use crate::ast::{Program, Statement, Expression, BinaryOperator};
use crate::lexer::Lexer;
use crate::lexer::token::Token;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::Eof,
            peek_token: Token::Eof,
        };
        // Read two tokens, so current_token and peek_token are both set
        parser.next_token();
        parser.next_token();
        parser
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Program {
        let mut statements = Vec::new();

        while self.current_token != Token::Eof {
            if let Some(statement) = self.parse_statement() {
                statements.push(statement);
            }
            self.next_token();
        }

        Program { statements }
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        let is_mutable = if self.peek_token == Token::Mut {
            self.next_token();
            true
        } else {
            false
        };

        self.next_token(); // consume 'let' or 'mut'

        let name = match &self.current_token {
            Token::Identifier(name) => name.clone(),
            _ => return None, // Expected identifier
        };

        self.next_token(); // consume identifier

        let initializer = if self.current_token == Token::Assign {
            self.next_token(); // consume '='
            self.parse_expression(0)
        } else {
            None
        };

        if self.current_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::LetDeclaration {
            name,
            is_mutable,
            initializer,
        })
    }

    fn parse_expression_statement(&mut self) -> Option<Statement> {
        let expression = self.parse_expression(0)?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::ExpressionStatement(expression))
    }


    pub fn parse_expression(&mut self, precedence: u8) -> Option<Expression> {
        let mut left = self.parse_prefix()?;

        while self.peek_token != Token::Semicolon && precedence < self.peek_precedence() {
            self.next_token();
            left = self.parse_infix(left)?;
        }

        Some(left)
    }

    fn parse_prefix(&mut self) -> Option<Expression> {
        match &self.current_token {
            Token::Number(n) => Some(Expression::NumberLiteral(*n)),
            Token::String(s) => Some(Expression::StringLiteral(s.clone())),
            Token::Identifier(name) => Some(Expression::Identifier(name.clone())),
            _ => None,
        }
    }

    fn parse_infix(&mut self, left: Expression) -> Option<Expression> {
        let operator = match self.current_token {
            Token::Plus => BinaryOperator::Plus,
            Token::Minus => BinaryOperator::Minus,
            Token::Asterisk => BinaryOperator::Multiply,
            Token::Slash => BinaryOperator::Divide,
            _ => return None,
        };

        let precedence = self.current_precedence();
        self.next_token();
        let right = self.parse_expression(precedence)?;

        Some(Expression::BinaryExpression {
            left: Box::new(left),
            operator,
            right: Box::new(right),
        })
    }

    fn peek_precedence(&self) -> u8 {
        match self.peek_token {
            Token::Plus | Token::Minus => 1,
            Token::Asterisk | Token::Slash => 2,
            _ => 0,
        }
    }

    fn current_precedence(&self) -> u8 {
        match self.current_token {
            Token::Plus | Token::Minus => 1,
            Token::Asterisk | Token::Slash => 2,
            _ => 0,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::{Program, Statement, Expression, BinaryOperator};
    use crate::lexer::Lexer;

    #[test]
    fn test_parse_number_literal() {
        let input = "123.45;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let expected = Program {
            statements: vec![Statement::ExpressionStatement(Expression::NumberLiteral(123.45))],
        };
        assert_eq!(program, expected);
    }

    #[test]
    fn test_parse_string_literal() {
        let input = "\"hello\";";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let expected = Program {
            statements: vec![Statement::ExpressionStatement(Expression::StringLiteral("hello".to_string()))],
        };
        assert_eq!(program, expected);
    }

    #[test]
    fn test_parse_binary_expression() {
        let input = "1 + 2 * 3;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let expected = Program {
            statements: vec![Statement::ExpressionStatement(Expression::BinaryExpression {
                left: Box::new(Expression::NumberLiteral(1.0)),
                operator: BinaryOperator::Plus,
                right: Box::new(Expression::BinaryExpression {
                    left: Box::new(Expression::NumberLiteral(2.0)),
                    operator: BinaryOperator::Multiply,
                    right: Box::new(Expression::NumberLiteral(3.0)),
                }),
            })],
        };
        assert_eq!(program, expected);
    }

    #[test]
    fn test_parse_let_statement() {
        let input = "let x = 5;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(
            program,
            Program {
                statements: vec![Statement::LetDeclaration {
                    name: "x".to_string(),
                    is_mutable: false,
                    initializer: Some(Expression::NumberLiteral(5.0)),
                }]
            }
        );
    }

    #[test]
    fn test_parse_mutable_let_statement() {
        let input = "let mut y = 10;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(
            program,
            Program {
                statements: vec![Statement::LetDeclaration {
                    name: "y".to_string(),
                    is_mutable: true,
                    initializer: Some(Expression::NumberLiteral(10.0)),
                }]
            }
        );
    }
}
