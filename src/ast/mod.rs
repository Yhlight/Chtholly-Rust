#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Let(String, Expression),
    Mut(String, Expression),
    Return(Expression),
    Expression(Expression),
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(String),
    Int(i64),
    Double(f64),
    Char(char),
    String(String),
    Bool(bool),
    Array(Vec<Expression>),
    Index(Box<Expression>, Box<Expression>),
    Prefix(Prefix, Box<Expression>),
    Infix(Infix, Box<Expression>, Box<Expression>),
    If {
        condition: Box<Expression>,
        consequence: BlockStatement,
        alternative: Option<BlockStatement>,
    },
    Function {
        parameters: Vec<String>,
        body: BlockStatement,
    },
    Call {
        function: Box<Expression>,
        arguments: Vec<Expression>,
    },
    While {
        condition: Box<Expression>,
        body: BlockStatement,
    },
}

pub type BlockStatement = Vec<Statement>;

#[derive(Debug, PartialEq, Clone)]
pub enum Prefix {
    Not,
    Minus,
}

#[derive(Debug, PartialEq, Clone)]
pub enum Infix {
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Equal,
    NotEqual,
    GreaterThan,
    GreaterThanOrEqual,
    LessThan,
    LessThanOrEqual,
}
