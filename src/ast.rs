//! Abstract Syntax Tree (AST) for the Chtholly language.

/// Represents a node in the AST.
#[derive(Debug, PartialEq, Clone)]
pub enum Node {
    Function(Function),
    Statement(Statement),
    Expression(Expression),
}

/// Represents a function definition.
#[derive(Debug, PartialEq, Clone)]
pub struct Function {
    pub name: String,
    pub parameters: Vec<Parameter>,
    pub return_type: Type,
    pub body: Vec<Statement>,
}

/// Represents a statement.
#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Let(LetDeclaration),
}

/// Represents a let declaration.
#[derive(Debug, PartialEq, Clone)]
pub struct LetDeclaration {
    pub name: String,
    pub type_info: Option<Type>,
    pub value: Expression,
}

/// Represents a function parameter.
#[derive(Debug, PartialEq, Clone)]
pub struct Parameter {
    pub name: String,
    pub type_info: Type,
}

/// Represents a type.
#[derive(Debug, PartialEq, Clone)]
pub enum Type {
    Simple(String),
    Array(Box<Type>),
    Generic(String, Vec<Type>),
}

/// Represents an expression.
#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Literal(Literal),
}

/// Represents a literal value.
#[derive(Debug, PartialEq, Clone)]
pub enum Literal {
    Int(i64),
}
