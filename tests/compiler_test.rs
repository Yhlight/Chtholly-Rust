
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
    let mut compiler = Compiler::new(&context, &builder, &module);
    let result = compiler.compile(program);

    assert!(result.is_ok());
    let function = result.unwrap();
    assert!(function.print_to_string().to_string().contains("define i32 @main()"));
}

#[test]
fn test_let_statement() {
    let input = "fn main() { let x = 5; }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let context = Context::create();
    let module = context.create_module("main");
    let builder = context.create_builder();
    let mut compiler = Compiler::new(&context, &builder, &module);
    let result = compiler.compile(program);

    assert!(result.is_ok());
    let function = result.unwrap();
    assert!(function.print_to_string().to_string().contains("alloca i32"));
    assert!(function.print_to_string().to_string().contains("store i32 5, ptr %x"));
}

#[test]
fn test_mut_statement() {
    let input = "fn main() { mut x = 5; }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let context = Context::create();
    let module = context.create_module("main");
    let builder = context.create_builder();
    let mut compiler = Compiler::new(&context, &builder, &module);
    let result = compiler.compile(program);

    assert!(result.is_ok());
    let function = result.unwrap();
    assert!(function.print_to_string().to_string().contains("alloca i32"));
    assert!(function.print_to_string().to_string().contains("store i32 5, ptr %x"));
}
