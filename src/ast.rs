use crate::lexer::Token;

#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Let {
        name: String,
        mutable: bool,
        value: Expression,
    },
    Return(Expression),
    Expression(Expression),
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(String),
    Integer(i64),
    Float(f64),
    String(String),
    Char(char),
    Boolean(bool),
    Prefix(Token, Box<Expression>),
    Infix(Box<Expression>, Token, Box<Expression>),
    If {
        condition: Box<Expression>,
        consequence: Vec<Statement>,
        alternative: Option<Vec<Statement>>,
    },
    Function {
        parameters: Vec<String>,
        body: Vec<Statement>,
    },
    Call {
        function: Box<Expression>,
        arguments: Vec<Expression>,
    },
    Assign {
        name: String,
        value: Box<Expression>,
    },
}

pub type Program = Vec<Statement>;
