#[derive(Debug, PartialEq)]
pub enum Statement {
    Let(String, Expression),
    // Other statements will be added here
}

#[derive(Debug, PartialEq)]
pub enum Expression {
    Identifier(String),
    Literal(Literal),
    // Other expressions will be added here
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
