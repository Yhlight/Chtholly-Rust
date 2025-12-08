use chtholly::codegen::Compiler;
use chtholly::lexer::Lexer;
use chtholly::parser::Parser;
use inkwell::context::Context;
use regex::Regex;

#[test]
fn test_if_else_expression() {
    let input = r#"
        let x = 10;
        let result = if (x > 5) { 1 } else { 2 };
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program().unwrap();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile_program(program);

    let ir = compiler.module.print_to_string().to_string();

    // Check for the conditional branch using a regex
    let re = Regex::new(r"br i1 %\w+, label %then, label %else").unwrap();
    assert!(re.is_match(&ir), "IR did not contain a conditional branch. IR was:\n{}", ir);

    // Check for the 'then' block
    assert!(ir.contains("then:"));
    assert!(ir.contains("br label %ifcont"));

    // Check for the 'else' block
    assert!(ir.contains("else:"));
    assert!(ir.contains("br label %ifcont"));

    // Check for the merge block and PHI node
    assert!(ir.contains("ifcont:"));
    let phi_re = Regex::new(r"%\w+ = phi i32 \[ 1, %then \], \[ 2, %else \]").unwrap();
    assert!(phi_re.is_match(&ir));

    // Check that the result is stored
    let store_re = Regex::new(r"store i32 %\w+, ptr %result").unwrap();
    assert!(store_re.is_match(&ir));
}

#[test]
fn test_if_expression_without_else() {
    let input = "if (true) { 10 };";

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program().unwrap();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile_program(program);

    let ir = compiler.module.print_to_string().to_string();

    assert!(ir.contains("br i1 true, label %then, label %else"));
    assert!(ir.contains("then:"));
    assert!(ir.contains("else:"));
    assert!(ir.contains("ifcont:"));
}
