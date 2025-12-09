use crate::ast::{ASTNode, Type, BinaryOperator};
use crate::lexer::{Lexer, Token, LexerError};
use std::iter::Peekable;
use thiserror::Error;
use std::slice::Iter;

#[derive(Debug, Error, PartialEq)]
pub enum ParserError {
    #[error("Lexer error: {0}")]
    LexerError(#[from] LexerError),
    #[error("Unexpected token")]
    UnexpectedToken,
    #[error("Unexpected end of file")]
    UnexpectedEOF,
}

type ParseResult<T> = Result<T, ParserError>;


pub struct Parser<'a> {
    tokens: Vec<Token>,
    _marker: std::marker::PhantomData<&'a ()>,
}

impl<'a> Parser<'a> {
    pub fn new(input: &'a str) -> Result<Self, LexerError> {
        let lexer = Lexer::new(input);
        let tokens: Result<Vec<Token>, LexerError> = lexer
            .filter(|t| !matches!(t, Ok(Token::Whitespace) | Ok(Token::Newline)))
            .collect();
        Ok(Parser {
            tokens: tokens?,
            _marker: std::marker::PhantomData,
        })
    }

    pub fn parse(&self) -> ParseResult<Vec<ASTNode>> {
        let mut ast = Vec::new();
        let mut tokens = self.tokens.iter().peekable();

        while tokens.peek().is_some() {
            let node = self.parse_statement(&mut tokens)?;
            ast.push(node);
        }
        Ok(ast)
    }

    fn parse_statement(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        let statement = match tokens.peek().ok_or(ParserError::UnexpectedEOF)? {
            Token::Comment(_) => self.parse_comment(tokens),
            &Token::Fn => self.parse_function(tokens),
            &Token::Let => self.parse_variable_declaration(tokens),
            &Token::If => self.parse_if_statement(tokens),
            &Token::While => self.parse_while_statement(tokens),
            &Token::For => self.parse_for_statement(tokens),
            _ => self.parse_assignment_or_expression(tokens),
        }?;

        if tokens.peek() == Some(&&Token::Semicolon) {
            tokens.next();
        }

        Ok(statement)
    }

    fn parse_assignment_or_expression(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        let left = self.parse_expression(tokens)?;
        if tokens.peek() == Some(&&Token::Assign) {
            tokens.next(); // Consume '='
            let right = self.parse_expression(tokens)?;
            Ok(ASTNode::AssignmentExpression {
                left: Box::new(left),
                right: Box::new(right),
            })
        } else {
            Ok(left)
        }
    }

    fn parse_comment(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        match tokens.next().ok_or(ParserError::UnexpectedEOF)? {
            Token::Comment(text) => Ok(ASTNode::Comment(text.clone())),
            _ => Err(ParserError::UnexpectedToken),
        }
    }

    fn parse_function(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        tokens.next(); // Consume 'fn'

        let name = match tokens.next().ok_or(ParserError::UnexpectedEOF)? {
            Token::Identifier(name) => name.clone(),
            _ => return Err(ParserError::UnexpectedToken),
        };

        if tokens.next() != Some(&Token::LParen) { return Err(ParserError::UnexpectedToken); }
        self.skip_until_matching(tokens, &Token::LParen, &Token::RParen);

        let body = self.parse_block(tokens)?;

        Ok(ASTNode::Function {
            name,
            args: Vec::new(),
            body,
        })
    }

    fn parse_variable_declaration(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        tokens.next(); // Consume 'let'

        let is_mutable = if tokens.peek() == Some(&&Token::Mut) {
            tokens.next();
            true
        } else {
            false
        };

        let name = match tokens.next().ok_or(ParserError::UnexpectedEOF)? {
            Token::Identifier(name) => name.clone(),
            _ => return Err(ParserError::UnexpectedToken),
        };

        let type_annotation = if tokens.peek() == Some(&&Token::Colon) {
            tokens.next();
            Some(self.parse_type(tokens)?)
        } else {
            None
        };

        let value = if tokens.peek() == Some(&&Token::Assign) {
            tokens.next();
            Some(self.parse_expression(tokens)?)
        } else {
            None
        };

        Ok(ASTNode::VariableDeclaration {
            is_mutable,
            name,
            type_annotation,
            value: value.map(Box::new),
        })
    }

    fn parse_type(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<Type> {
        match tokens.next().ok_or(ParserError::UnexpectedEOF)? {
            Token::Identifier(type_name) => match type_name.as_str() {
                "i32" => Ok(Type::I32),
                "f64" => Ok(Type::F64),
                "bool" => Ok(Type::Bool),
                "string" => Ok(Type::String),
                _ => Err(ParserError::UnexpectedToken),
            },
            _ => Err(ParserError::UnexpectedToken),
        }
    }

    fn parse_expression(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        self.parse_expression_with_precedence(tokens, 0)
    }

    fn parse_expression_with_precedence(&self, tokens: &mut Peekable<Iter<Token>>, min_precedence: u8) -> ParseResult<ASTNode> {
        let mut left = self.parse_primary_expression(tokens)?;

        while let Some(&next_token) = tokens.peek() {
            let precedence = self.get_precedence(next_token);
            if precedence < min_precedence {
                break;
            }

            if let Some(op) = self.parse_binary_operator(tokens) {
                let right = self.parse_expression_with_precedence(tokens, precedence)?;
                left = ASTNode::BinaryExpression {
                    op,
                    left: Box::new(left),
                    right: Box::new(right),
                };
            } else {
                break;
            }
        }

        Ok(left)
    }

    fn parse_primary_expression(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        match tokens.peek().ok_or(ParserError::UnexpectedEOF)? {
            Token::IntegerLiteral(_) => {
                if let Some(Token::IntegerLiteral(value)) = tokens.next() {
                    Ok(ASTNode::IntegerLiteral(*value))
                } else {
                    Err(ParserError::UnexpectedToken)
                }
            }
            Token::FloatLiteral(_) => {
                if let Some(Token::FloatLiteral(value)) = tokens.next() {
                    Ok(ASTNode::FloatLiteral(*value))
                } else {
                    Err(ParserError::UnexpectedToken)
                }
            }
            Token::StringLiteral(_) => {
                if let Some(Token::StringLiteral(value)) = tokens.next() {
                    Ok(ASTNode::StringLiteral(value.clone()))
                } else {
                    Err(ParserError::UnexpectedToken)
                }
            }
            Token::True => {
                tokens.next();
                Ok(ASTNode::BoolLiteral(true))
            }
            Token::False => {
                tokens.next();
                Ok(ASTNode::BoolLiteral(false))
            }
            Token::Identifier(_) => {
                if let Some(Token::Identifier(name)) = tokens.next() {
                    Ok(ASTNode::Identifier(name.clone()))
                } else {
                    Err(ParserError::UnexpectedToken)
                }
            }
            Token::Ampersand => {
                tokens.next(); // Consume '&'
                let is_mutable = if tokens.peek() == Some(&&Token::Mut) {
                    tokens.next(); // Consume 'mut'
                    true
                } else {
                    false
                };
                let expr = self.parse_primary_expression(tokens)?;
                Ok(ASTNode::Reference {
                    expression: Box::new(expr),
                    is_mutable,
                })
            }
            Token::Asterisk => {
                tokens.next(); // Consume '*'
                let expr = self.parse_primary_expression(tokens)?;
                Ok(ASTNode::Dereference(Box::new(expr)))
            }
            _ => Err(ParserError::UnexpectedToken),
        }
    }

    fn parse_binary_operator(&self, tokens: &mut Peekable<Iter<Token>>) -> Option<BinaryOperator> {
        let op = match tokens.peek() {
            Some(Token::Plus) => Some(BinaryOperator::Add),
            Some(Token::Minus) => Some(BinaryOperator::Subtract),
            Some(Token::Asterisk) => {
                // This is now only for binary multiplication
                Some(BinaryOperator::Multiply)
            }
            Some(Token::Slash) => Some(BinaryOperator::Divide),
            Some(Token::Equal) => Some(BinaryOperator::Equal),
            Some(Token::NotEqual) => Some(BinaryOperator::NotEqual),
            Some(Token::LessThan) => Some(BinaryOperator::LessThan),
            Some(Token::GreaterThan) => Some(BinaryOperator::GreaterThan),
            Some(Token::LessThanOrEqual) => Some(BinaryOperator::LessThanOrEqual),
            Some(Token::GreaterThanOrEqual) => Some(BinaryOperator::GreaterThanOrEqual),
            _ => None,
        };
        if op.is_some() {
            tokens.next();
        }
        op
    }

    fn get_precedence(&self, token: &Token) -> u8 {
        match token {
            Token::Equal | Token::NotEqual => 1,
            Token::LessThan | Token::GreaterThan | Token::LessThanOrEqual | Token::GreaterThanOrEqual => 2,
            Token::Plus | Token::Minus => 3,
            Token::Asterisk | Token::Slash => 4,
            _ => 0,
        }
    }

    fn parse_if_statement(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        tokens.next(); // Consume 'if'

        if tokens.next() != Some(&Token::LParen) { return Err(ParserError::UnexpectedToken); }
        let condition = self.parse_expression(tokens)?;
        if tokens.next() != Some(&Token::RParen) { return Err(ParserError::UnexpectedToken); }

        let then_block = self.parse_block(tokens)?;

        let else_block = if tokens.peek() == Some(&&Token::Else) {
            tokens.next(); // Consume 'else'
            Some(self.parse_block(tokens)?)
        } else {
            None
        };

        Ok(ASTNode::IfStatement {
            condition: Box::new(condition),
            then_block,
            else_block,
        })
    }

    fn parse_block(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<Vec<ASTNode>> {
        if tokens.next() != Some(&Token::LBrace) { return Err(ParserError::UnexpectedToken); }

        let mut body = Vec::new();
        while tokens.peek() != Some(&&Token::RBrace) && tokens.peek().is_some() {
            body.push(self.parse_statement(tokens)?);
        }
        tokens.next(); // Consume '}'

        Ok(body)
    }

    fn parse_while_statement(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        tokens.next(); // Consume 'while'

        if tokens.next() != Some(&Token::LParen) { return Err(ParserError::UnexpectedToken); }
        let condition = self.parse_expression(tokens)?;
        if tokens.next() != Some(&Token::RParen) { return Err(ParserError::UnexpectedToken); }

        let body = self.parse_block(tokens)?;

        Ok(ASTNode::WhileStatement {
            condition: Box::new(condition),
            body,
        })
    }


    fn skip_until_matching(&self, tokens: &mut Peekable<Iter<Token>>, open: &Token, close: &Token) {
        let mut depth = 1;
        while let Some(token) = tokens.next() {
            if token == open {
                depth += 1;
            } else if token == close {
                depth -= 1;
                if depth == 0 {
                    return;
                }
            }
        }
    }

    fn parse_for_statement(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        tokens.next();

        if tokens.next() != Some(&Token::LParen) {
            return Err(ParserError::UnexpectedToken);
        }

        let init = if tokens.peek() != Some(&&Token::Semicolon) {
            Some(Box::new(self.parse_statement(tokens)?))
        } else {
            None
        };

        if tokens.peek() == Some(&&Token::Semicolon) {
            tokens.next();
        }

        let condition = if tokens.peek() != Some(&&Token::Semicolon) {
            Some(Box::new(self.parse_expression(tokens)?))
        } else {
            None
        };

        if tokens.peek() == Some(&&Token::Semicolon) {
            tokens.next();
        }

        let increment = if tokens.peek() != Some(&&Token::RParen) {
            Some(Box::new(self.parse_expression_or_assignment(tokens)?))
        } else {
            None
        };

        if tokens.next() != Some(&Token::RParen) {
            return Err(ParserError::UnexpectedToken);
        }

        let body = self.parse_block(tokens)?;

        Ok(ASTNode::ForStatement {
            init,
            condition,
            increment,
            body,
        })
    }

    fn parse_expression_or_assignment(&self, tokens: &mut Peekable<Iter<Token>>) -> ParseResult<ASTNode> {
        let left = self.parse_expression(tokens)?;
        if tokens.peek() == Some(&&Token::Assign) {
            tokens.next(); // Consume '='
            let right = self.parse_expression(tokens)?;
            Ok(ASTNode::AssignmentExpression {
                left: Box::new(left),
                right: Box::new(right),
            })
        } else {
            Ok(left)
        }
    }
}
