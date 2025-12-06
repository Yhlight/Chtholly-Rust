//! Abstract Syntax Tree (AST) nodes for the Chtholly language.

#[derive(Debug, PartialEq)]
pub struct Program {
    pub functions: Vec<FunctionDefinition>,
}

#[derive(Debug, PartialEq)]
pub struct FunctionDefinition {
    pub name: String,
    pub params: Vec<Parameter>,
    pub return_type: Type,
    pub body: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Parameter {
    pub name: String,
    pub param_type: Type,
}

#[derive(Debug, PartialEq, Clone)]
pub enum Type {
    Simple(String),
    Array(Box<Type>),
    Generic(String, Vec<Type>),
}

#[derive(Debug, PartialEq)]
pub enum Statement {
    Expression(Expression),
    // Future statements: Let, Return, If, etc.
}

#[derive(Debug, PartialEq)]
pub enum Expression {
    FunctionCall {
        name: String,
        args: Vec<Expression>,
    },
    StringLiteral(String),
    IntegerLiteral(i64),
    // Future expressions: Variable, BinaryOp, etc.
}
