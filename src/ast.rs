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
    Block(Vec<Statement>),
    If {
        condition: Expression,
        consequence: Box<Statement>,
        alternative: Option<Box<Statement>>,
    },
    While {
        condition: Expression,
        body: Box<Statement>,
    },
    Break,
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
