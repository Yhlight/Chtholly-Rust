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
    pub scopes: Vec<HashMap<String, Symbol>>,
    pub errors: Vec<String>,
}

impl SemanticAnalyzer {
    pub fn new() -> Self {
        let mut scopes = Vec::new();
        scopes.push(HashMap::new()); // Global scope
        SemanticAnalyzer {
            scopes,
            errors: Vec::new(),
        }
    }

    pub fn enter_scope(&mut self) {
        self.scopes.push(HashMap::new());
    }

    pub fn leave_scope(&mut self) {
        self.scopes.pop();
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
                self.scopes.last_mut().unwrap().insert(name.clone(), Symbol { type_: expr_type, mutable: *mutable });
            }
            Statement::Return(expr) => {
                self.analyze_expression(expr);
            }
            Statement::Expression(expr) => {
                self.analyze_expression(expr);
            }
            Statement::Block(statements) => {
                self.analyze_block_statement(statements);
            }
            Statement::If { condition, consequence, alternative } => {
                let cond_type = self.analyze_expression(condition);
                if cond_type != Type::Boolean {
                    self.errors.push(format!(
                        "if condition must be a boolean, but got {:?}",
                        cond_type
                    ));
                }
                self.analyze_statement(consequence);
                if let Some(alt) = alternative {
                    self.analyze_statement(alt);
                }
            }
        }
    }

    fn analyze_block_statement(&mut self, statements: &[Statement]) {
        self.enter_scope();
        for statement in statements {
            self.analyze_statement(statement);
        }
        self.leave_scope();
    }

    fn analyze_expression(&mut self, expression: &Expression) -> Type {
        match expression {
            Expression::Identifier(name) => {
                for scope in self.scopes.iter().rev() {
                    if let Some(symbol) = scope.get(name) {
                        return symbol.type_.clone();
                    }
                }
                self.errors.push(format!("undeclared variable: {}", name));
                Type::Void
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
                for scope in self.scopes.iter().rev() {
                    if let Some(symbol) = scope.get(name) {
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
                        return symbol.type_.clone();
                    }
                }
                self.errors.push(format!("undeclared variable: {}", name));
                Type::Void
            }
            _ => Type::Void,
        }
    }

    pub fn type_of(&self, expression: &Expression) -> Option<Type> {
        match expression {
            Expression::Identifier(name) => {
                for scope in self.scopes.iter().rev() {
                    if let Some(symbol) = scope.get(name) {
                        return Some(symbol.type_.clone());
                    }
                }
                None
            }
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
            Expression::Assign { name, .. } => {
                for scope in self.scopes.iter().rev() {
                    if let Some(symbol) = scope.get(name) {
                        return Some(symbol.type_.clone());
                    }
                }
                None
            }
            _ => None,
        }
    }
}
