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

pub struct SemanticAnalyzer {
    symbols: HashMap<String, Type>,
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
            Statement::Let(name, expr) => {
                let expr_type = self.analyze_expression(expr);
                self.symbols.insert(name.clone(), expr_type);
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
                if let Some(t) = self.symbols.get(name) {
                    t.clone()
                } else {
                    self.errors.push(format!("undeclared variable: {}", name));
                    Type::Void
                }
            }
            Expression::Integer(_) => Type::Integer,
            Expression::Float(_) => Type::Float,
            Expression::String(_) => Type::String,
            Expression::Char(_) => Type::Char,
            Expression::Prefix(_, expr) => self.analyze_expression(expr),
            Expression::Infix(left, _, right) => {
                let left_type = self.analyze_expression(left);
                let right_type = self.analyze_expression(right);
                if left_type != Type::Void && right_type != Type::Void && left_type != right_type {
                    self.errors.push(format!(
                        "type mismatch: {:?} and {:?}",
                        left_type, right_type
                    ));
                }
                left_type
            }
            _ => Type::Void,
        }
    }
}
