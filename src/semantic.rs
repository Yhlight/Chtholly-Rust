use std::collections::HashMap;
use crate::ast::{Program, Statement, Expression, Type as AstType};

#[derive(Debug, PartialEq, Clone)]
pub enum Type {
    I32,
    I64,
    F64,
    Bool,
    Char,
    String,
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
    in_loop: bool,
}

impl SemanticAnalyzer {
    pub fn new() -> Self {
        let mut scopes = Vec::new();
        scopes.push(HashMap::new()); // Global scope
        SemanticAnalyzer {
            scopes,
            errors: Vec::new(),
            in_loop: false,
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
            Statement::Let {
                name,
                mutable,
                type_annotation,
                value,
            } => {
                let expr_type = self.analyze_expression(value);
                let var_type = match type_annotation {
                    Some(t) => {
                        let annotated_type = type_from_ast(t);
                        if annotated_type != expr_type {
                            self.errors.push(format!(
                                "type mismatch: annotated type {:?}, but got {:?}",
                                annotated_type, expr_type
                            ));
                        }
                        annotated_type
                    }
                    None => expr_type,
                };
                self.scopes.last_mut().unwrap().insert(
                    name.clone(),
                    Symbol {
                        type_: var_type,
                        mutable: *mutable,
                    },
                );
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
                if cond_type != Type::Bool {
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
            Statement::While { condition, body } => {
                let cond_type = self.analyze_expression(condition);
                if cond_type != Type::Bool {
                    self.errors.push(format!(
                        "while condition must be a boolean, but got {:?}",
                        cond_type
                    ));
                }
                let old_in_loop = self.in_loop;
                self.in_loop = true;
                self.analyze_statement(body);
                self.in_loop = old_in_loop;
            }
            Statement::Break => {
                if !self.in_loop {
                    self.errors.push("break statement outside of a loop".to_string());
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
            Expression::Integer(_) => Type::I64,
            Expression::Float(_) => Type::F64,
            Expression::String(_) => Type::String,
            Expression::Char(_) => Type::Char,
            Expression::Boolean(_) => Type::Bool,
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
                    | crate::lexer::Token::GreaterThanOrEqual => Type::Bool,
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
            Expression::Integer(_) => Some(Type::I64),
            Expression::Float(_) => Some(Type::F64),
            Expression::String(_) => Some(Type::String),
            Expression::Char(_) => Some(Type::Char),
            Expression::Boolean(_) => Some(Type::Bool),
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
                    | crate::lexer::Token::GreaterThanOrEqual => Some(Type::Bool),
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

fn type_from_ast(ast_type: &AstType) -> Type {
    match ast_type {
        AstType::I32 => Type::I32,
        AstType::I64 => Type::I64,
        AstType::F64 => Type::F64,
        AstType::Bool => Type::Bool,
        AstType::Char => Type::Char,
        AstType::String => Type::String,
    }
}
