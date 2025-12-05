use crate::lexer::token::Token;

#[derive(Debug, PartialEq)]
pub enum Statement {
    LetStatement {
        token: Token,
        name: Identifier,
        // value: Expression,
    },
}

#[derive(Debug, PartialEq)]
pub struct Identifier {
    pub token: Token,
    pub value: String,
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
