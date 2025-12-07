use crate::parser::ast::Program;

pub struct TypeChecker {}

impl TypeChecker {
    pub fn new() -> Self {
        TypeChecker {}
    }

    pub fn check_program(&self, program: &Program) {
        // For now, we'll just do nothing.
    }
}
