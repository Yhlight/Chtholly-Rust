use std::fs;
use inkwell::context::Context;
use app::lexer::Lexer;
use app::parser::Parser;
use app::codegen::Compiler;

fn main() {
    let input = fs::read_to_string("input.cns").expect("Something went wrong reading the file");
    let lexer = Lexer::new(&input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    match program {
        Ok(program) => {
            let context = Context::create();
            let mut compiler = Compiler::new(&context);
            compiler.compile_program(program);
            compiler.module.print_to_stderr();
        },
        Err(errors) => {
            for error in errors {
                println!("{}", error);
            }
        }
    }
}
