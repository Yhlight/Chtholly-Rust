use chtholly::parser::Parser;
use chtholly::compiler::Compiler;
use inkwell::context::Context;

fn main() {
    let code = r#"
        fn main() {
            while (true) {
                let a = 10;
            }
        }
    "#;

    let parser = Parser::new(code).unwrap();
    let ast = parser.parse().unwrap();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile(&ast).unwrap();

    compiler.print_ir();
}
