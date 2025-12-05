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
    Expression(Expression),
}

/// Represents a let declaration.
#[derive(Debug, PartialEq, Clone)]
pub struct LetDeclaration {
    pub name: String,
    pub mutable: bool,
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
    Binary(Box<Expression>, BinaryOp, Box<Expression>),
    If(If),
    Block(Vec<Statement>),
}

/// Represents an if-else expression.
#[derive(Debug, PartialEq, Clone)]
pub struct If {
    pub condition: Box<Expression>,
    pub then_block: Box<Expression>,
    pub else_block: Option<Box<Expression>>,
}

/// Represents a literal value.
#[derive(Debug, PartialEq, Clone)]
pub enum Literal {
    Int(i64),
    Float(f64),
    Char(char),
    Bool(bool),
    String(String),
}

/// Represents a binary operator.
#[derive(Debug, PartialEq, Clone)]
pub enum BinaryOp {
    Add,
    Sub,
    Mul,
    Div,

    Eq,
    NotEq,
    Gt,
    GtEq,
    Lt,
    LtEq,

    And,
    Or,
}
