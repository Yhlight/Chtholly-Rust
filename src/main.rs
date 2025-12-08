use chtholly::parser::Parser;
use chtholly::compiler::Compiler;
use inkwell::context::Context;

fn main() {
    let code = r#"
        fn main() {
            let a: i32 = 10;
            let mut b = 20;
        }
    "#;

    let parser = Parser::new(code);
    let ast = parser.parse();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile(&ast).unwrap();

    compiler.print_ir();
}
