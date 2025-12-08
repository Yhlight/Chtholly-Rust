
#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Let(Identifier, bool, Expression),
    Return(Expression),
    Expression(Expression),
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(Identifier),
    Literal(Literal),
    Prefix(PrefixOperator, Box<Expression>),
    Infix(Box<Expression>, InfixOperator, Box<Expression>),
}

#[derive(Debug, PartialEq, Clone)]
pub enum PrefixOperator {
    Not,
    Minus,
}

#[derive(Debug, PartialEq, Clone)]
pub enum InfixOperator {
    Plus,
    Minus,
    Multiply,
    Divide,
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Identifier(pub String);

#[derive(Debug, PartialEq, Clone)]
pub enum Literal {
    Integer(i64),
    Float(f64),
    String(String),
    Boolean(bool),
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
