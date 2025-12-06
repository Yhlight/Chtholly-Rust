use crate::token::Token;
use std::any::Any;
use std::fmt;

// A trait for displaying the AST for debugging
pub trait Node: fmt::Display {
    fn token_literal(&self) -> String;
}

pub trait Statement: Node {
    fn statement_node(&self);
    fn as_any(&self) -> &dyn Any;
}

pub trait Expression: Node {
    fn expression_node(&self);
    fn as_any(&self) -> &dyn Any;
}

pub struct Program {
    pub statements: Vec<Box<dyn Statement>>,
}

impl fmt::Display for Program {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        for s in &self.statements {
            write!(f, "{}", s)?;
        }
        Ok(())
    }
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

pub struct LetStatement {
    pub token: Token, // the 'let' token
    pub name: Identifier,
    pub value: Box<dyn Expression>,
}

impl fmt::Display for LetStatement {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{} {} = {};", self.token_literal(), self.name, self.value)
    }
}

impl Node for LetStatement {
    fn token_literal(&self) -> String {
        "let".to_string()
    }
}

impl Statement for LetStatement {
    fn statement_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

#[derive(Clone)]
pub struct Identifier {
    pub token: Token, // the 'Identifier' token
    pub value: String,
}

impl fmt::Display for Identifier {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.value)
    }
}

impl Node for Identifier {
    fn token_literal(&self) -> String {
        self.value.clone()
    }
}

impl Expression for Identifier {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct InfixExpression {
    pub token: Token, // The operator token, e.g. +
    pub left: Box<dyn Expression>,
    pub operator: String,
    pub right: Box<dyn Expression>,
}

impl fmt::Display for InfixExpression {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "({} {} {})", self.left, self.operator, self.right)
    }
}

impl Node for InfixExpression {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Expression for InfixExpression {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct ExpressionStatement {
    pub token: Token, // the first token of the expression
    pub expression: Box<dyn Expression>,
}

impl fmt::Display for ExpressionStatement {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.expression)
    }
}

impl Node for ExpressionStatement {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Statement for ExpressionStatement {
    fn statement_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct BooleanLiteral {
    pub token: Token,
    pub value: bool,
}

impl fmt::Display for BooleanLiteral {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.value)
    }
}

impl Node for BooleanLiteral {
    fn token_literal(&self) -> String {
        self.value.to_string()
    }
}

impl Expression for BooleanLiteral {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct PrefixExpression {
    pub token: Token, // The prefix token, e.g. !
    pub operator: String,
    pub right: Box<dyn Expression>,
}

impl fmt::Display for PrefixExpression {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "({}{})", self.operator, self.right)
    }
}

impl Node for PrefixExpression {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Expression for PrefixExpression {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct ReturnStatement {
    pub token: Token, // the 'return' token
    pub return_value: Box<dyn Expression>,
}

impl fmt::Display for ReturnStatement {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{} {};", self.token_literal(), self.return_value)
    }
}

impl Node for ReturnStatement {
    fn token_literal(&self) -> String {
        "return".to_string()
    }
}

impl Statement for ReturnStatement {
    fn statement_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}


pub struct IntegerLiteral {
    pub token: Token,
    pub value: i64,
}

impl fmt::Display for IntegerLiteral {
     fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.value)
    }
}

impl Node for IntegerLiteral {
    fn token_literal(&self) -> String {
        // Corrected from self.token.to_string()
        self.value.to_string()
    }
}

impl Expression for IntegerLiteral {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}
