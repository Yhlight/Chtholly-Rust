mod parser;
mod codegen;
mod type_checker;

use codegen::codegen::CodeGen;
use inkwell::context::Context;
use parser::ast::Program;
use type_checker::TypeChecker;

fn main() {
    let context = Context::create();
    let codegen = CodeGen::new(&context);
    let type_checker = TypeChecker::new();
    let program = Program { nodes: vec![] };

    type_checker.check_program(&program);
    codegen.compile(program);
}
