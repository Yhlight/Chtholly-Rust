// Represents a node in the Abstract Syntax Tree.
#[derive(Debug, Clone, PartialEq)]
pub enum Node {
    Statement(Statement),
    Expression(Expression),
}

use crate::parser::types::Type;

// Represents a statement.
#[derive(Debug, Clone, PartialEq)]
pub enum Statement {
    // e.g., let x: i32 = 5;
    Let {
        name: String,
        type_annotation: Option<Type>,
        initializer: Box<Expression>,
    },
}

// Represents an expression.
#[derive(Debug, Clone, PartialEq)]
pub enum Expression {
    // e.g., 5
    IntegerLiteral(i64),
}

// The root of the AST.
pub struct Program {
    pub nodes: Vec<Node>,
}
