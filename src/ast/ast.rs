// Represents a node in the Abstract Syntax Tree
pub trait Node {
    fn token_literal(&self) -> String;
}

// Represents a statement
#[derive(Debug)]
pub enum Statement {
    Let(LetStatement),
    // ... other statements
}

// Represents an expression
#[derive(Debug)]
pub enum Expression {
    Identifier(Identifier),
    IntegerLiteral(i64),
    BooleanLiteral(bool),
    PrefixExpression {
        operator: String,
        right: Box<Expression>,
    },
    InfixExpression {
        left: Box<Expression>,
        operator: String,
        right: Box<Expression>,
    },
    // ... other expressions
}

// Represents a let statement
#[derive(Debug)]
pub struct LetStatement {
    pub name: Identifier,
    pub value: Expression,
    pub is_mutable: bool,
}

// Represents an identifier
#[derive(Debug)]
pub struct Identifier {
    pub value: String,
}

// The root node of the AST
pub struct Program {
    pub statements: Vec<Statement>,
}

impl Node for Program {
    fn token_literal(&self) -> String {
        if self.statements.len() > 0 {
            // For now, just return the literal of the first statement
            // This will be improved later
            "program".to_string()
        } else {
            "".to_string()
        }
    }
}
