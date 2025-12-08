use chtholly::parser::Parser;
use chtholly::compiler::Compiler;
use inkwell::context::Context;

fn main() {
    let code = r#"
        fn main() {
            if (true) {
                let a = 20;
            } else {
                let b = 30;
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
