use crate::ast::{ASTNode, Type};
use crate::lexer::{Lexer, Token};
use std::iter::Peekable;
use std::slice::Iter;

pub struct Parser<'a> {
    tokens: Vec<Token>,
    _marker: std::marker::PhantomData<&'a ()>,
}

impl<'a> Parser<'a> {
    pub fn new(input: &'a str) -> Self {
        let lexer = Lexer::new(input);
        Parser {
            tokens: lexer
                .filter(|t| !matches!(t, Token::Whitespace | Token::Newline))
                .collect(),
            _marker: std::marker::PhantomData,
        }
    }

    pub fn parse(&self) -> Vec<ASTNode> {
        let mut ast = Vec::new();
        let mut tokens = self.tokens.iter().peekable();

        while tokens.peek().is_some() {
            let node = self.parse_statement(&mut tokens);

            if let Some(n) = node {
                ast.push(n);
            }
        }
        ast
    }

    fn parse_statement(&self, tokens: &mut Peekable<Iter<Token>>) -> Option<ASTNode> {
        match tokens.peek() {
            Some(Token::Comment(_)) => self.parse_comment(tokens),
            Some(&Token::Fn) => self.parse_function(tokens),
            Some(&Token::Let) => self.parse_variable_declaration(tokens),
            _ => {
                tokens.next();
                None
            }
        }
    }

    fn parse_comment(&self, tokens: &mut Peekable<Iter<Token>>) -> Option<ASTNode> {
        if let Some(Token::Comment(text)) = tokens.next() {
            Some(ASTNode::Comment(text.clone()))
        } else {
            None
        }
    }

    fn parse_function(&self, tokens: &mut Peekable<Iter<Token>>) -> Option<ASTNode> {
        tokens.next(); // Consume 'fn'

        let name = if let Some(Token::Identifier(name)) = tokens.next() {
            name.clone()
        } else {
            return None;
        };

        if tokens.next() != Some(&Token::LParen) { return None; }
        self.skip_until_matching(tokens, &Token::LParen, &Token::RParen);

        let mut body = Vec::new();
        if tokens.peek() == Some(&&Token::LBrace) {
            tokens.next(); // Consume '{'
            while tokens.peek() != Some(&&Token::RBrace) && tokens.peek().is_some() {
                if let Some(statement) = self.parse_statement(tokens) {
                    body.push(statement);
                }
            }
            tokens.next(); // Consume '}'
        }

        Some(ASTNode::Function {
            name,
            args: Vec::new(),
            body,
        })
    }

    fn parse_variable_declaration(&self, tokens: &mut Peekable<Iter<Token>>) -> Option<ASTNode> {
        tokens.next(); // Consume 'let'

        let is_mutable = if tokens.peek() == Some(&&Token::Mut) {
            tokens.next();
            true
        } else {
            false
        };

        let name = if let Some(Token::Identifier(name)) = tokens.next() {
            name.clone()
        } else {
            return None;
        };

        let type_annotation = if tokens.peek() == Some(&&Token::Colon) {
            tokens.next();
            self.parse_type(tokens)
        } else {
            None
        };

        let value = if tokens.peek() == Some(&&Token::Assign) {
            tokens.next();
            self.parse_expression(tokens)
        } else {
            None
        };

        if tokens.peek() == Some(&&Token::Semicolon) {
            tokens.next();
        }

        Some(ASTNode::VariableDeclaration {
            is_mutable,
            name,
            type_annotation,
            value: value.map(Box::new),
        })
    }

    fn parse_type(&self, tokens: &mut Peekable<Iter<Token>>) -> Option<Type> {
        if let Some(Token::Identifier(type_name)) = tokens.next() {
            match type_name.as_str() {
                "i32" => Some(Type::I32),
                "f64" => Some(Type::F64),
                "bool" => Some(Type::Bool),
                "string" => Some(Type::String),
                _ => None,
            }
        } else {
            None
        }
    }

    fn parse_expression(&self, tokens: &mut Peekable<Iter<Token>>) -> Option<ASTNode> {
        match tokens.next() {
            Some(Token::IntegerLiteral(value)) => Some(ASTNode::IntegerLiteral(*value)),
            Some(Token::FloatLiteral(value)) => Some(ASTNode::FloatLiteral(*value)),
            Some(Token::StringLiteral(value)) => Some(ASTNode::StringLiteral(value.clone())),
            Some(Token::True) => Some(ASTNode::BoolLiteral(true)),
            Some(Token::False) => Some(ASTNode::BoolLiteral(false)),
            _ => None,
        }
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
}
