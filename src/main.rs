use chtholly::parser::Parser;
use chtholly::compiler::Compiler;
use inkwell::context::Context;

fn main() {
    let code = r#"
        fn main() {
            let a: i32 = 10;
            let b: f64 = 3.14;
            let c: bool = true;
            let d: string = "hello";
        }
    "#;

    let parser = Parser::new(code);
    let ast = parser.parse();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile(&ast).unwrap();

    compiler.print_ir();
}
