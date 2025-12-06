use crate::token::Token;

#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Let(Identifier, Option<Type>, Option<Expression>),
    Mut(Identifier, Option<Type>, Option<Expression>),
    Return(Expression),
    Expression(Expression),
    Block(BlockStatement),
    Struct(StructStatement),
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(Identifier),
    IntLiteral(i64),
    FloatLiteral(f64),
    StringLiteral(String),
    CharLiteral(char),
    Boolean(bool),
    Prefix(Token, Box<Expression>),
    Infix(Box<Expression>, Token, Box<Expression>),
    If {
        condition: Box<Expression>,
        consequence: BlockStatement,
        alternative: Option<BlockStatement>,
    },
    FunctionLiteral {
        parameters: Vec<(Identifier, Type)>,
        body: BlockStatement,
    },
    Call {
        function: Box<Expression>,
        arguments: Vec<Expression>,
    },
    StructLiteral {
        name: Identifier,
        fields: Vec<(Identifier, Expression)>,
    },
}

#[derive(Debug, PartialEq, Clone)]
pub enum Type {
    Int,
    Double,
    Char,
    String,
    Bool,
    Void,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Identifier(pub String);

#[derive(Debug, PartialEq, Clone)]
pub struct BlockStatement {
    pub statements: Vec<Statement>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct StructStatement {
    pub name: Identifier,
    pub fields: Vec<(Identifier, Type)>,
}

pub struct Program {
    pub statements: Vec<Statement>,
}

impl Program {
    pub fn new() -> Self {
        Program {
            statements: Vec::new(),
        }
    }
}
