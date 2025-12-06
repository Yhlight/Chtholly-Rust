use crate::lexer::token::Token;

// The base Node trait
pub trait Node {
    fn token_literal(&self) -> String;
}

use std::any::Any;

// Statement Node trait
pub trait Statement: Node {
    fn statement_node(&self);
    fn as_any(&self) -> &dyn Any;
}

// Expression Node trait
pub trait Expression: Node {
    fn expression_node(&self);
}

// The root node of every AST
pub struct Program {
    pub statements: Vec<Box<dyn Statement>>,
}

impl Node for Program {
    fn token_literal(&self) -> String {
        if self.statements.len() > 0 {
            self.statements[0].token_literal()
        } else {
            "".to_string()
        }
    }
}

// Let Statement
pub struct LetStatement {
    pub token: Token, // the 'let' token
    pub name: Identifier,
    pub value: Box<dyn Expression>,
}

impl Node for LetStatement {
    fn token_literal(&self) -> String {
        "let".to_string()
    }
}

impl Statement for LetStatement {
    fn statement_node(&self) {}
    fn as_any(&self) -> &dyn Any {
        self
    }
}

// Identifier
pub struct Identifier {
    pub token: Token, // the 'Identifier' token
    pub value: String,
}

impl Node for Identifier {
    fn token_literal(&self) -> String {
        self.value.clone()
    }
}

impl Expression for Identifier {
    fn expression_node(&self) {}
}

// Integer Literal
pub struct IntegerLiteral {
    pub token: Token,
    pub value: i64,
}

impl Node for IntegerLiteral {
    fn token_literal(&self) -> String {
        self.value.to_string()
    }
}

impl Expression for IntegerLiteral {
    fn expression_node(&self) {}
}
