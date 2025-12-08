use chtholly::codegen::Compiler;
use chtholly::lexer::Lexer;
use chtholly::parser::Parser;
use inkwell::context::Context;

#[test]
fn test_integer_arithmetic() {
    let tests = vec![
        ("let z = 5 + 5;", "store i32 10, ptr %z"),
        ("let z = 10 - 5;", "store i32 5, ptr %z"),
        ("let z = 2 * 8;", "store i32 16, ptr %z"),
        ("let z = 10 / 2;", "store i32 5, ptr %z"),
    ];

    for (input, expected_ir) in tests {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        compiler.compile_program(program);

        let ir = compiler.module.print_to_string().to_string();
        assert!(
            ir.contains(expected_ir),
            "Failed on input '{}'. Expected to find '{}'. IR was:\n{}",
            input,
            expected_ir,
            ir
        );
    }
}

#[test]
fn test_arithmetic_with_variables() {
    let input = r#"
        let x = 10;
        let y = 20;
        let z = x + y;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program().unwrap();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile_program(program);

    let ir = compiler.module.print_to_string().to_string();

    assert!(ir.contains("%x = alloca i32"));
    assert!(ir.contains("store i32 10, ptr %x"));
    assert!(ir.contains("%y = alloca i32"));
    assert!(ir.contains("store i32 20, ptr %y"));
    assert!(ir.contains("%z = alloca i32"));
    assert!(ir.contains("load i32, ptr %x"));
    assert!(ir.contains("load i32, ptr %y"));
    assert!(ir.contains("add i32"));
}

#[test]
fn test_comparison_operators() {
    let tests = vec![
        ("let a = 5 == 5;", "store i1 true, ptr %a"),
        ("let a = 5 != 10;", "store i1 true, ptr %a"),
        ("let a = 2 < 8;", "store i1 true, ptr %a"),
        ("let a = 10 > 5;", "store i1 true, ptr %a"),
        ("let a = 5 == 6;", "store i1 false, ptr %a"),
        ("let a = 5 != 5;", "store i1 false, ptr %a"),
        ("let a = 8 < 2;", "store i1 false, ptr %a"),
        ("let a = 5 > 10;", "store i1 false, ptr %a"),
    ];

    for (input, expected_ir) in tests {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        compiler.compile_program(program);

        let ir = compiler.module.print_to_string().to_string();
        assert!(
            ir.contains(expected_ir),
            "Failed on input '{}'. Expected to find '{}'. IR was:\n{}",
            input,
            expected_ir,
            ir
        );
    }
}
