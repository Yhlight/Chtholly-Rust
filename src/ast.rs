use crate::token::Token;
use std::any::Any;

// Every node in our AST will implement this trait
pub trait Node {
    fn token_literal(&self) -> String;
}

// A statement is a block of code that does not produce a value (e.g., let x = 5;)
pub trait Statement: Node {
    fn statement_node(&self);
    fn as_any(&self) -> &dyn Any;
}

// An expression is a block of code that produces a value (e.g., 5, x + 10)
pub trait Expression: Node {
    fn expression_node(&self);
}

// The root node of every AST our parser produces
pub struct Program {
    pub statements: Vec<Box<dyn Statement>>,
}

impl Node for Program {
    fn token_literal(&self) -> String {
        if self.statements.is_empty() {
            "".to_string()
        } else {
            self.statements[0].token_literal()
        }
    }
}

// Represents a 'let' statement: let <Identifier> = <Expression>;
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
    fn as_any(&self) -> &dyn Any { self }
}

// Represents an identifier
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
