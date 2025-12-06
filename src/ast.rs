use crate::token::Token;

pub trait Node {
    fn token_literal(&self) -> String;
}

#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Let {
        token: Token, // the 'let' token
        name: Identifier,
        value: Expression,
    },
    Mut {
        token: Token, // the 'mut' token
        name: Identifier,
        value: Expression,
    },
    ExpressionStatement {
        expression: Expression,
    },
    Block(Vec<Statement>),
    Continue(Token),
    Break(Token),
}

impl Node for Statement {
    fn token_literal(&self) -> String {
        match self {
            Statement::Let { token, .. } => token.literal.clone(),
            Statement::Mut { token, .. } => token.literal.clone(),
            Statement::ExpressionStatement { expression } => expression.token_literal(),
            Statement::Block(statements) => {
                if statements.is_empty() {
                    "".to_string()
                } else {
                    statements[0].token_literal()
                }
            }
            Statement::Continue(token) => token.literal.clone(),
            Statement::Break(token) => token.literal.clone(),
        }
    }
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(Identifier),
    IntegerLiteral {
        token: Token,
        value: i64,
    },
    FloatLiteral {
        token: Token,
        value: f64,
    },
    BooleanLiteral {
        token: Token,
        value: bool,
    },
    StringLiteral {
        token: Token,
        value: String,
    },
    CharLiteral {
        token: Token,
        value: char,
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
    IfExpression {
        token: Token, // The 'if' token
        condition: Box<Expression>,
        consequence: Box<Statement>,
        alternative: Option<Box<Statement>>,
    },
    FunctionLiteral {
        token: Token, // The 'fn' token
        name: Identifier,
        parameters: Vec<Identifier>,
        body: Box<Statement>, // BlockStatement
    },
    WhileExpression {
        token: Token, // The 'while' token
        condition: Box<Expression>,
        body: Box<Statement>, // BlockStatement
    },
}

impl Node for Expression {
    fn token_literal(&self) -> String {
        match self {
            Expression::Identifier(ident) => ident.token.literal.clone(),
            Expression::IntegerLiteral { token, .. } => token.literal.clone(),
            Expression::FloatLiteral { token, .. } => token.literal.clone(),
            Expression::BooleanLiteral { token, .. } => token.literal.clone(),
            Expression::StringLiteral { token, .. } => token.literal.clone(),
            Expression::CharLiteral { token, .. } => token.literal.clone(),
            Expression::PrefixExpression { token, .. } => token.literal.clone(),
            Expression::InfixExpression { token, .. } => token.literal.clone(),
            Expression::IfExpression { token, .. } => token.literal.clone(),
            Expression::FunctionLiteral { token, .. } => token.literal.clone(),
            Expression::WhileExpression { token, .. } => token.literal.clone(),
        }
    }
}

#[derive(Clone, Debug, PartialEq)]
pub struct Identifier {
    pub token: Token, // the 'ident' token
    pub value: String,
}

impl Node for Identifier {
    fn token_literal(&self) -> String {
        self.token.literal.clone()
    }
}

pub struct Program {
    pub statements: Vec<Statement>,
}

impl Node for Program {
    fn token_literal(&self) -> String {
        if self.statements.is_empty() {
            "".to_string()
        } else {
            self.statements[0].token_literal()
        }
    }
}
