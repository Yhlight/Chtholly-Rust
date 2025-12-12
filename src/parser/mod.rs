//! Parser for the Chtholly language.

use crate::ast::{Expression, BinaryOperator};
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

    pub fn parse_expression(&mut self) -> Option<Expression> {
        self.parse_addition()
    }

    fn parse_addition(&mut self) -> Option<Expression> {
        let mut left = self.parse_multiplication()?;

        while self.current_token == Token::Plus || self.current_token == Token::Minus {
            let operator = match self.current_token {
                Token::Plus => BinaryOperator::Plus,
                Token::Minus => BinaryOperator::Minus,
                _ => return None, // Should not happen
            };
            self.next_token();
            let right = self.parse_multiplication()?;
            left = Expression::BinaryExpression {
                left: Box::new(left),
                operator,
                right: Box::new(right),
            };
        }

        Some(left)
    }

    fn parse_multiplication(&mut self) -> Option<Expression> {
        let mut left = self.parse_primary()?;

        while self.current_token == Token::Asterisk || self.current_token == Token::Slash {
            let operator = match self.current_token {
                Token::Asterisk => BinaryOperator::Multiply,
                Token::Slash => BinaryOperator::Divide,
                _ => return None, // Should not happen
            };
            self.next_token();
            let right = self.parse_primary()?;
            left = Expression::BinaryExpression {
                left: Box::new(left),
                operator,
                right: Box::new(right),
            };
        }

        Some(left)
    }


    fn parse_primary(&mut self) -> Option<Expression> {
        let expr = match &self.current_token {
            Token::Number(n) => Some(Expression::NumberLiteral(*n)),
            Token::String(s) => Some(Expression::StringLiteral(s.clone())),
            _ => None,
        };
        if expr.is_some() {
            self.next_token();
        }
        expr
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;

    #[test]
    fn test_parse_number_literal() {
        let input = "123.45";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let expression = parser.parse_expression().unwrap();
        assert_eq!(expression, Expression::NumberLiteral(123.45));
    }

    #[test]
    fn test_parse_string_literal() {
        let input = "\"hello\"";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let expression = parser.parse_expression().unwrap();
        assert_eq!(expression, Expression::StringLiteral("hello".to_string()));
    }

    #[test]
    fn test_parse_binary_expression() {
        let input = "1 + 2 * 3";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let expression = parser.parse_expression().unwrap();
        assert_eq!(
            expression,
            Expression::BinaryExpression {
                left: Box::new(Expression::NumberLiteral(1.0)),
                operator: BinaryOperator::Plus,
                right: Box::new(Expression::BinaryExpression {
                    left: Box::new(Expression::NumberLiteral(2.0)),
                    operator: BinaryOperator::Multiply,
                    right: Box::new(Expression::NumberLiteral(3.0)),
                }),
            }
        );
    }
}
