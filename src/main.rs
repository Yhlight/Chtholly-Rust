use inkwell::context::Context;
use lexer::Lexer;
use parser::Parser;
use codegen::CodeGen;

mod lexer;
mod parser;
mod codegen;

fn main() {
    let input = "let x = 5;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let context = Context::create();
    let codegen = CodeGen::new(&context);
    let result = codegen.compile(program);

    match result {
        Ok(ir) => println!("{}", ir),
        Err(e) => eprintln!("Error: {}", e),
    }
}
