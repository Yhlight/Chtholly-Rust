//! Abstract Syntax Tree representation for the Chtholly language.

use crate::lexer::token::Token;

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
    // Other statements like Return, ExpressionStatement, etc., will be added here.
}

impl Node for Statement {
    fn token_literal(&self) -> String {
        match self {
            Statement::Let(s) => s.token_literal(),
        }
    }
    fn string(&self) -> String {
        match self {
            Statement::Let(s) => s.string(),
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
}

impl Node for Expression {
    fn token_literal(&self) -> String {
        match self {
            Expression::Identifier(i) => i.token_literal(),
            Expression::IntegerLiteral(i) => i.token_literal(),
            Expression::FloatLiteral(f) => f.token_literal(),
            Expression::StringLiteral(s) => s.token_literal(),
            Expression::Boolean(b) => b.token_literal(),
        }
    }
    fn string(&self) -> String {
        match self {
            Expression::Identifier(i) => i.string(),
            Expression::IntegerLiteral(i) => i.string(),
            Expression::FloatLiteral(f) => f.string(),
            Expression::StringLiteral(s) => s.string(),
            Expression::Boolean(b) => b.string(),
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
    /// The `let` token.
    pub token: Token,
    /// The identifier being bound.
    pub name: Identifier,
    /// The expression producing the value.
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

/// Represents an identifier.
#[derive(Debug, PartialEq, Clone)]
pub struct Identifier {
    /// The `Ident` token.
    pub token: Token,
    /// The name of the identifier.
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
