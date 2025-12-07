//! Abstract Syntax Tree representation for the Chtholly language.

use crate::lexer::token::Token;
use std::fmt;

/// A node in the Abstract Syntax Tree.
pub trait Node {
    /// Returns the literal value of the token associated with this node.
    fn token_literal(&self) -> String;
    /// Returns a string representation of the node for debugging.
    fn string(&self) -> String;
}

/// Represents a statement in Chtholly.
#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Let(LetStatement),
    Return(ReturnStatement),
    Expression(ExpressionStatement),
}

impl Node for Statement {
    fn token_literal(&self) -> String {
        match self {
            Statement::Let(s) => s.token_literal(),
            Statement::Return(s) => s.token_literal(),
            Statement::Expression(s) => s.token_literal(),
        }
    }
    fn string(&self) -> String {
        match self {
            Statement::Let(s) => s.string(),
            Statement::Return(s) => s.string(),
            Statement::Expression(s) => s.string(),
        }
    }
}

/// Represents an expression in Chtholly.
#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(Identifier),
    IntegerLiteral(IntegerLiteral),
    FloatLiteral(FloatLiteral),
    StringLiteral(StringLiteral),
    Boolean(Boolean),
    Prefix(PrefixExpression),
    Infix(InfixExpression),
    If(IfExpression),
    FunctionLiteral(FunctionLiteral),
    Call(CallExpression),
}

impl Node for Expression {
    fn token_literal(&self) -> String {
        match self {
            Expression::Identifier(i) => i.token_literal(),
            Expression::IntegerLiteral(i) => i.token_literal(),
            Expression::FloatLiteral(f) => f.token_literal(),
            Expression::StringLiteral(s) => s.token_literal(),
            Expression::Boolean(b) => b.token_literal(),
            Expression::Prefix(p) => p.token_literal(),
            Expression::Infix(i) => i.token_literal(),
            Expression::If(i) => i.token_literal(),
            Expression::FunctionLiteral(f) => f.token_literal(),
            Expression::Call(c) => c.token_literal(),
        }
    }
    fn string(&self) -> String {
        match self {
            Expression::Identifier(i) => i.string(),
            Expression::IntegerLiteral(i) => i.string(),
            Expression::FloatLiteral(f) => f.string(),
            Expression::StringLiteral(s) => s.string(),
            Expression::Boolean(b) => b.string(),
            Expression::Prefix(p) => p.string(),
            Expression::Infix(i) => i.string(),
            Expression::If(i) => i.string(),
            Expression::FunctionLiteral(f) => f.string(),
            Expression::Call(c) => c.string(),
        }
    }
}

/// The root of a Chtholly program's AST.
pub struct Program {
    pub statements: Vec<Statement>,
}

impl Node for Program {
    fn token_literal(&self) -> String {
        if let Some(first_statement) = self.statements.first() {
            first_statement.token_literal()
        } else {
            "".to_string()
        }
    }

    fn string(&self) -> String {
        self.statements
            .iter()
            .map(|s| s.string())
            .collect::<String>()
    }
}

/// Represents a `let` statement: `let <name> = <value>;`
#[derive(Debug, PartialEq, Clone)]
pub struct LetStatement {
    pub token: Token,
    pub name: Identifier,
    pub value: Expression,
}

impl Node for LetStatement {
    fn token_literal(&self) -> String {
        "let".to_string()
    }
    fn string(&self) -> String {
        format!(
            "{} {} = {};",
            self.token_literal(),
            self.name.string(),
            self.value.string()
        )
    }
}

/// Represents a `return` statement: `return <value>;`
#[derive(Debug, PartialEq, Clone)]
pub struct ReturnStatement {
    pub token: Token,
    pub return_value: Expression,
}

impl Node for ReturnStatement {
    fn token_literal(&self) -> String {
        "return".to_string()
    }
    fn string(&self) -> String {
        format!("{} {};", self.token_literal(), self.return_value.string())
    }
}

/// Represents a statement that consists of a single expression.
#[derive(Debug, PartialEq, Clone)]
pub struct ExpressionStatement {
    pub token: Token,
    pub expression: Expression,
}

impl Node for ExpressionStatement {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
    fn string(&self) -> String {
        self.expression.string()
    }
}

/// Represents a block of statements.
#[derive(Debug, PartialEq, Clone)]
pub struct BlockStatement {
    pub token: Token, // the { token
    pub statements: Vec<Statement>,
}

impl Node for BlockStatement {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
    fn string(&self) -> String {
        self.statements
            .iter()
            .map(|s| s.string())
            .collect::<String>()
    }
}

/// Represents an identifier.
#[derive(Debug, PartialEq, Clone)]
pub struct Identifier {
    pub token: Token,
    pub value: String,
}

impl Node for Identifier {
    fn token_literal(&self) -> String {
        self.value.clone()
    }
    fn string(&self) -> String {
        self.value.clone()
    }
}

/// Represents an integer literal expression.
#[derive(Debug, PartialEq, Clone)]
pub struct IntegerLiteral {
    pub token: Token,
    pub value: i64,
}

impl Node for IntegerLiteral {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
    fn string(&self) -> String {
        self.value.to_string()
    }
}

/// Represents a float literal expression.
#[derive(Debug, PartialEq, Clone)]
pub struct FloatLiteral {
    pub token: Token,
    pub value: f64,
}

impl Node for FloatLiteral {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
    fn string(&self) -> String {
        self.value.to_string()
    }
}

/// Represents a string literal expression.
#[derive(Debug, PartialEq, Clone)]
pub struct StringLiteral {
    pub token: Token,
    pub value: String,
}

impl Node for StringLiteral {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
    fn string(&self) -> String {
        self.value.clone()
    }
}

/// Represents a boolean literal expression.
#[derive(Debug, PartialEq, Clone)]
pub struct Boolean {
    pub token: Token,
    pub value: bool,
}

impl Node for Boolean {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
    fn string(&self) -> String {
        self.value.to_string()
    }
}

/// Represents a prefix expression, like `!5` or `-15`.
#[derive(Debug, PartialEq, Clone)]
pub struct PrefixExpression {
    pub token: Token, // The prefix token, e.g., `!` or `-`
    pub operator: String,
    pub right: Box<Expression>,
}

impl Node for PrefixExpression {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
    fn string(&self) -> String {
        format!("({}{})", self.operator, self.right.string())
    }
}

/// Represents an infix expression, like `5 + 5`.
#[derive(Debug, PartialEq, Clone)]
pub struct InfixExpression {
    pub token: Token, // The operator token, e.g., `+`
    pub left: Box<Expression>,
    pub operator: String,
    pub right: Box<Expression>,
}

impl Node for InfixExpression {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
    fn string(&self) -> String {
        format!(
            "({} {} {})",
            self.left.string(),
            self.operator,
            self.right.string()
        )
    }
}

/// Represents an if-else expression.
#[derive(Debug, PartialEq, Clone)]
pub struct IfExpression {
    pub token: Token, // The 'if' token
    pub condition: Box<Expression>,
    pub consequence: BlockStatement,
    pub alternative: Option<BlockStatement>,
}

impl Node for IfExpression {
    fn token_literal(&self) -> String {
        "if".to_string()
    }
    fn string(&self) -> String {
        let mut s = format!(
            "if {} {}",
            self.condition.string(),
            self.consequence.string()
        );
        if let Some(alt) = &self.alternative {
            s.push_str(" else ");
            s.push_str(&alt.string());
        }
        s
    }
}

/// Represents a function literal, like `fn(x, y) { x + y; }`
#[derive(Debug, PartialEq, Clone)]
pub struct FunctionLiteral {
    pub token: Token, // The 'fn' token
    pub parameters: Vec<Identifier>,
    pub body: BlockStatement,
}

impl Node for FunctionLiteral {
    fn token_literal(&self) -> String {
        "fn".to_string()
    }
    fn string(&self) -> String {
        let params = self
            .parameters
            .iter()
            .map(|p| p.string())
            .collect::<Vec<String>>()
            .join(", ");
        format!("fn({}) {}", params, self.body.string())
    }
}

/// Represents a call expression, like `myFunction(1, 2)`
#[derive(Debug, PartialEq, Clone)]
pub struct CallExpression {
    pub token: Token, // The '(' token
    pub function: Box<Expression>, // Identifier or FunctionLiteral
    pub arguments: Vec<Expression>,
}

impl Node for CallExpression {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
    fn string(&self) -> String {
        let args = self
            .arguments
            .iter()
            .map(|a| a.string())
            .collect::<Vec<String>>()
            .join(", ");
        format!("{}({})", self.function.string(), args)
    }
}
