
use crate::compiler::lexer::Token;

#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Let(LetStatement),
    Mut(MutStatement),
}

#[derive(Debug, PartialEq, Clone)]
pub struct LetStatement {
    pub ident: String,
    pub data_type: Option<String>,
    pub value: Expression,
}

#[derive(Debug, PartialEq, Clone)]
pub struct MutStatement {
    pub ident: String,
    pub data_type: Option<String>,
    pub value: Expression,
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(String),
    IntLiteral(i64),
    FloatLiteral(f64),
    StringLiteral(String),
    Prefix(Token, Box<Expression>),
    Infix(Token, Box<Expression>, Box<Expression>),
}
