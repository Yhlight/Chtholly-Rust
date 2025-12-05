use crate::lexer::token::Token;
use std::fmt::{self, Display};

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(Identifier),
    IntegerLiteral {
        token: Token,
        value: i64,
    },
    PrefixExpression {
        token: Token,
        operator: String,
        right: Box<Expression>,
    },
    InfixExpression {
        token: Token,
        left: Box<Expression>,
        operator: String,
        right: Box<Expression>,
    },
}

impl Display for Expression {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Expression::Identifier(ident) => write!(f, "{}", ident.value),
            Expression::IntegerLiteral { value, .. } => write!(f, "{}", value),
            Expression::PrefixExpression { operator, right, .. } => {
                write!(f, "({}{})", operator, right)
            }
            Expression::InfixExpression {
                left,
                operator,
                right,
                ..
            } => {
                write!(f, "({} {} {})", left, operator, right)
            }
        }
    }
}

#[derive(Debug, PartialEq)]
pub enum Statement {
    LetStatement {
        token: Token,
        name: Identifier,
        value: Expression,
    },
    ExpressionStatement {
        token: Token,
        expression: Expression,
    },
}

impl Display for Statement {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Statement::LetStatement { name, value, .. } => {
                write!(f, "let {} = {};", name, value)
            }
            Statement::ExpressionStatement { expression, .. } => write!(f, "{}", expression),
        }
    }
}

#[derive(Debug, PartialEq, Clone)]
pub struct Identifier {
    pub token: Token,
    pub value: String,
}

impl Display for Identifier {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.value)
    }
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
