//! Defines the Abstract Syntax Tree (AST) for the Chtholly language.

/// Represents a statement in a Chtholly program.
#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    /// A `let` or `let mut` binding.
    Let {
        name: String,
        is_mutable: bool,
        type_annotation: Option<Type>,
        value: Expression,
    },
    /// A function declaration.
    FunctionDeclaration(FunctionDeclaration),
    /// An expression used as a statement.
    ExpressionStatement(Expression),
}

/// Represents a function declaration.
#[derive(Debug, PartialEq, Clone)]
pub struct FunctionDeclaration {
    pub name: String,
    pub parameters: Vec<Parameter>,
    pub return_type: Type,
    pub body: Vec<Statement>,
}

/// Represents a function parameter.
#[derive(Debug, PartialEq, Clone)]
pub struct Parameter {
    pub name: String,
    pub type_annotation: Type,
}

/// Represents an expression.
#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    /// A literal value.
    Literal(LiteralValue),
    /// An identifier used as a variable.
    Identifier(String),
    /// A binary operation (e.g., `a + b`).
    Binary {
        op: BinaryOp,
        left: Box<Expression>,
        right: Box<Expression>,
    },
    /// A function call.
    FunctionCall(FunctionCall),
}

/// Represents a function call.
#[derive(Debug, PartialEq, Clone)]
pub struct FunctionCall {
    pub name: String,
    pub arguments: Vec<Expression>,
}

/// Represents a binary operator.
#[derive(Debug, PartialEq, Clone, Copy)]
pub enum BinaryOp {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
}

/// Represents a literal value.
#[derive(Debug, PartialEq, Clone)]
pub enum LiteralValue {
    Integer(i64),
    Boolean(bool),
}

/// Represents a type in the Chtholly type system.
#[derive(Debug, PartialEq, Clone)]
pub enum Type {
    I32,
    Bool,
    Void,
}
