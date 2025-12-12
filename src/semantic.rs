use std::collections::HashMap;
use crate::ast::{Program, Statement, Expression};

#[derive(Debug, PartialEq, Clone)]
pub enum Type {
    Integer,
    Float,
    String,
    Char,
    Boolean,
    Void,
}

#[derive(Debug, Clone)]
pub struct Symbol {
    pub type_: Type,
    pub mutable: bool,
}

pub struct SemanticAnalyzer {
    pub symbols: HashMap<String, Symbol>,
    pub errors: Vec<String>,
}

impl SemanticAnalyzer {
    pub fn new() -> Self {
        SemanticAnalyzer {
            symbols: HashMap::new(),
            errors: Vec::new(),
        }
    }

    pub fn analyze(&mut self, program: &Program) {
        for statement in program {
            self.analyze_statement(statement);
        }
    }

    fn analyze_statement(&mut self, statement: &Statement) {
        match statement {
            Statement::Let { name, mutable, value } => {
                let expr_type = self.analyze_expression(value);
                self.symbols.insert(name.clone(), Symbol { type_: expr_type, mutable: *mutable });
            }
            Statement::Return(expr) => {
                self.analyze_expression(expr);
            }
            Statement::Expression(expr) => {
                self.analyze_expression(expr);
            }
        }
    }

    fn analyze_expression(&mut self, expression: &Expression) -> Type {
        match expression {
            Expression::Identifier(name) => {
                if let Some(symbol) = self.symbols.get(name) {
                    symbol.type_.clone()
                } else {
                    self.errors.push(format!("undeclared variable: {}", name));
                    Type::Void
                }
            }
            Expression::Integer(_) => Type::Integer,
            Expression::Float(_) => Type::Float,
            Expression::String(_) => Type::String,
            Expression::Char(_) => Type::Char,
            Expression::Boolean(_) => Type::Boolean,
            Expression::Prefix(_, expr) => self.analyze_expression(expr),
            Expression::Infix(left, op, right) => {
                let left_type = self.analyze_expression(left);
                let right_type = self.analyze_expression(right);

                if left_type != Type::Void && right_type != Type::Void && left_type != right_type {
                    self.errors.push(format!(
                        "type mismatch: {:?} and {:?}",
                        left_type, right_type
                    ));
                    return Type::Void;
                }

                match op {
                    crate::lexer::Token::Plus
                    | crate::lexer::Token::Minus
                    | crate::lexer::Token::Asterisk
                    | crate::lexer::Token::Slash => left_type,
                    crate::lexer::Token::EqualEqual
                    | crate::lexer::Token::NotEqual
                    | crate::lexer::Token::LessThan
                    | crate::lexer::Token::GreaterThan
                    | crate::lexer::Token::LessThanOrEqual
                    | crate::lexer::Token::GreaterThanOrEqual => Type::Boolean,
                    _ => Type::Void,
                }
            }
            Expression::Assign { name, value } => {
                let right_type = self.analyze_expression(value);
                if let Some(symbol) = self.symbols.get(name) {
                    if !symbol.mutable {
                        self.errors
                            .push(format!("cannot assign to immutable variable `{}`", name));
                    }
                    if symbol.type_ != right_type {
                        self.errors.push(format!(
                            "type mismatch: {:?} and {:?}",
                            symbol.type_, right_type
                        ));
                    }
                    symbol.type_.clone()
                } else {
                    self.errors.push(format!("undeclared variable: {}", name));
                    Type::Void
                }
            }
            _ => Type::Void,
        }
    }

    pub fn type_of(&self, expression: &Expression) -> Option<Type> {
        match expression {
            Expression::Identifier(name) => self.symbols.get(name).map(|s| s.type_.clone()),
            Expression::Integer(_) => Some(Type::Integer),
            Expression::Float(_) => Some(Type::Float),
            Expression::String(_) => Some(Type::String),
            Expression::Char(_) => Some(Type::Char),
            Expression::Boolean(_) => Some(Type::Boolean),
            Expression::Prefix(_, expr) => self.type_of(expr),
            Expression::Infix(left, op, _) => {
                let left_type = self.type_of(left)?;
                match op {
                    crate::lexer::Token::Plus
                    | crate::lexer::Token::Minus
                    | crate::lexer::Token::Asterisk
                    | crate::lexer::Token::Slash => Some(left_type),
                    crate::lexer::Token::EqualEqual
                    | crate::lexer::Token::NotEqual
                    | crate::lexer::Token::LessThan
                    | crate::lexer::Token::GreaterThan
                    | crate::lexer::Token::LessThanOrEqual
                    | crate::lexer::Token::GreaterThanOrEqual => Some(Type::Boolean),
                    _ => None,
                }
            }
            Expression::Assign { name, .. } => self.symbols.get(name).map(|s| s.type_.clone()),
            _ => None,
        }
    }
}
