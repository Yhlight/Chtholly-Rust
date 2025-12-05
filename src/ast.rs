#[derive(Debug, PartialEq)]
pub enum Statement {
    Let(String, bool, Expression), // bool represents mutability
    Function(String, Vec<String>, BlockStatement),
    ExpressionStatement(Expression),
    // Other statements will be added here
}

#[derive(Debug, PartialEq)]
pub struct BlockStatement {
    pub statements: Vec<Statement>,
}

#[derive(Debug, PartialEq)]
pub enum Expression {
    Identifier(String),
    Literal(Literal),
    Binary(Operator, Box<Expression>, Box<Expression>),
    // Other expressions will be added here
}

#[derive(Debug, PartialEq)]
pub enum Operator {
    Plus,
    Minus,
    Star,
    Slash,
}

#[derive(Debug, PartialEq)]
pub enum Literal {
    Int(i64),
    Double(f64),
    String(String),
}

#[derive(Debug)]
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
