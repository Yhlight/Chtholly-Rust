//! Abstract Syntax Tree (AST) nodes for the Chtholly language.

/// Represents a full Chtholly program.
#[derive(Debug, PartialEq, Clone)]
pub struct Program {
    pub statements: Vec<Statement>,
}

/// Represents a statement in the Chtholly language.
#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    /// A variable declaration, e.g., `let x = 5;`.
    LetDeclaration {
        name: String,
        is_mutable: bool,
        initializer: Option<Expression>,
    },
    /// An expression statement, e.g., `x + 5;`.
    ExpressionStatement(Expression),
}


/// Represents an expression in the Chtholly language.
#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    /// A numeric literal, e.g., `10`, `30.4`.
    NumberLiteral(f64),

    /// A string literal, e.g., `"HelloWorld"`.
    StringLiteral(String),

    /// An identifier, e.g., a variable name.
    Identifier(String),

    /// An assignment expression, e.g., `x = 10`.
    Assignment {
        name: String,
        value: Box<Expression>,
    },

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
