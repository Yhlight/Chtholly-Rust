//! Abstract Syntax Tree (AST) nodes for the Chtholly language.

/// Represents an expression in the Chtholly language.
#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    /// A numeric literal, e.g., `10`, `30.4`.
    NumberLiteral(f64),

    /// A string literal, e.g., `"HelloWorld"`.
    StringLiteral(String),

    /// A binary operation, e.g., `a + b`.
    BinaryExpression {
        left: Box<Expression>,
        operator: BinaryOperator,
        right: Box<Expression>,
    },
}

/// Represents a binary operator.
#[derive(Debug, PartialEq, Clone)]
pub enum BinaryOperator {
    Plus,
    Minus,
    Multiply,
    Divide,
}
