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

pub struct MutStatement {
    pub token: Token, // the 'mut' token
    pub name: Identifier,
    pub value: Box<dyn Expression>,
}

impl fmt::Display for MutStatement {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{} {} = {};", self.token_literal(), self.name, self.value)
    }
}

impl Node for MutStatement {
    fn token_literal(&self) -> String {
        "mut".to_string()
    }
}

impl Statement for MutStatement {
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

pub struct ArrayLiteral {
    pub token: Token, // the '[' token
    pub elements: Vec<Box<dyn Expression>>,
}

impl fmt::Display for ArrayLiteral {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let elements: Vec<String> = self.elements.iter().map(|e| e.to_string()).collect();
        write!(f, "[{}]", elements.join(", "))
    }
}

impl Node for ArrayLiteral {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Expression for ArrayLiteral {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct IndexExpression {
    pub token: Token, // the '[' token
    pub left: Box<dyn Expression>,
    pub index: Box<dyn Expression>,
}

impl fmt::Display for IndexExpression {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "({}[{}])", self.left, self.index)
    }
}

impl Node for IndexExpression {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Expression for IndexExpression {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct StringLiteral {
    pub token: Token,
    pub value: String,
}

impl fmt::Display for StringLiteral {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.value)
    }
}

impl Node for StringLiteral {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Expression for StringLiteral {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct FunctionLiteral {
    pub token: Token, // The 'fn' token
    pub parameters: Vec<Identifier>,
    pub body: BlockStatement,
}

impl fmt::Display for FunctionLiteral {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let params: Vec<String> = self.parameters.iter().map(|p| p.to_string()).collect();
        write!(f, "{}({}) {}", self.token_literal(), params.join(", "), self.body)
    }
}

impl Node for FunctionLiteral {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Expression for FunctionLiteral {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct CallExpression {
    pub token: Token, // The '(' token
    pub function: Box<dyn Expression>, // Identifier or FunctionLiteral
    pub arguments: Vec<Box<dyn Expression>>,
}

impl fmt::Display for CallExpression {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let args: Vec<String> = self.arguments.iter().map(|a| a.to_string()).collect();
        write!(f, "{}({})", self.function, args.join(", "))
    }
}

impl Node for CallExpression {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Expression for CallExpression {
    fn expression_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct BlockStatement {
    pub token: Token, // the { token
    pub statements: Vec<Box<dyn Statement>>,
}

impl fmt::Display for BlockStatement {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        for s in &self.statements {
            write!(f, "{}", s)?;
        }
        Ok(())
    }
}

impl Node for BlockStatement {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Statement for BlockStatement {
    fn statement_node(&self) {}
    fn as_any(&self) -> &dyn Any { self }
}

pub struct IfExpression {
    pub token: Token, // The 'if' token
    pub condition: Box<dyn Expression>,
    pub consequence: BlockStatement,
    pub alternative: Option<BlockStatement>,
}

impl fmt::Display for IfExpression {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "if {} {}", self.condition, self.consequence)?;
        if let Some(alt) = &self.alternative {
            write!(f, "else {}", alt)?;
        }
        Ok(())
    }
}

impl Node for IfExpression {
    fn token_literal(&self) -> String {
        self.token.to_string()
    }
}

impl Expression for IfExpression {
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
