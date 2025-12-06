
use chtholly::compiler::Compiler;
use chtholly::lexer::Lexer;
use chtholly::parser::Parser;
use inkwell::context::Context;
use inkwell::values::AnyValue;

#[test]
fn test_main_function() {
    let input = "fn main() {}";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let context = Context::create();
    let module = context.create_module("main");
    let builder = context.create_builder();
    let compiler = Compiler::new(&context, &builder, &module);
    let result = compiler.compile(program);

    assert!(result.is_ok());
    let function = result.unwrap();
    assert!(function.print_to_string().to_string().contains("define i32 @main()"));
}
