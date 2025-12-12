//! Symbol table for semantic analysis.

use inkwell::values::PointerValue;
use std::collections::HashMap;

#[derive(Debug, Clone)]
pub struct Variable<'ctx> {
    pub name: String,
    pub is_mutable: bool,
    pub pointer: PointerValue<'ctx>,
}

pub struct SymbolTable<'ctx> {
    variables: HashMap<String, Variable<'ctx>>,
}

impl<'ctx> SymbolTable<'ctx> {
    pub fn new() -> Self {
        SymbolTable {
            variables: HashMap::new(),
        }
    }

    pub fn define(&mut self, name: &str, is_mutable: bool, pointer: PointerValue<'ctx>) {
        let var = Variable {
            name: name.to_string(),
            is_mutable,
            pointer,
        };
        self.variables.insert(name.to_string(), var);
    }

    pub fn get(&self, name: &str) -> Option<&Variable<'ctx>> {
        self.variables.get(name)
    }
}
