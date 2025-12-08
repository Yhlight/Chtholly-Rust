use chtholly::codegen::Compiler;
use chtholly::lexer::Lexer;
use chtholly::parser::Parser;
use inkwell::context::Context;

#[test]
fn test_let_statement() {
    let input = "let x = 5;";

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program().unwrap();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile_program(program);

    let ir = compiler.module.print_to_string().to_string();

    // Check for alloca instruction
    assert!(ir.contains("%x = alloca i32"), "IR should contain alloca for x");

    // Check for store instruction
    assert!(ir.contains("store i32 5, ptr %x"), "IR should contain store instruction for x");
}

#[test]
fn test_let_mut_statement() {
    let input = "let mut y = 10;";

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program().unwrap();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile_program(program);

    let ir = compiler.module.print_to_string().to_string();

    // Check for alloca instruction
    assert!(ir.contains("%y = alloca i32"), "IR should contain alloca for y");

    // Check for store instruction
    assert!(ir.contains("store i32 10, ptr %y"), "IR should contain store instruction for y");
}

#[test]
fn test_mutable_variable_reassignment() {
    let input = r#"
        let mut x = 5;
        x = 10;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program().unwrap();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile_program(program);

    let ir = compiler.module.print_to_string().to_string();

    assert!(ir.contains("store i32 5, ptr %x"));
    assert!(ir.contains("store i32 10, ptr %x"));
}

#[test]
#[should_panic(expected = "Cannot assign to immutable variable")]
fn test_immutable_variable_reassignment() {
    let input = r#"
        let x = 5;
        x = 10;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program().unwrap();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile_program(program);
}
