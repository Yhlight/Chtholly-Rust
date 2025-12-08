use crate::ast::ASTNode;
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
            let node = match tokens.peek() {
                Some(Token::Comment(_)) => self.parse_comment(&mut tokens),
                Some(&Token::Fn) => self.parse_function(&mut tokens),
                _ => {
                    tokens.next();
                    None
                }
            };

            if let Some(n) = node {
                ast.push(n);
            }
        }
        ast
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

        if tokens.peek() == Some(&&Token::LBrace) {
            tokens.next(); // Consume '{'
            self.skip_until_matching(tokens, &Token::LBrace, &Token::RBrace);
        }

        Some(ASTNode::Function {
            name,
            args: Vec::new(),
            body: Vec::new(),
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
}
