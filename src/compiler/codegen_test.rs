use crate::compiler::codegen::CodeGenerator;
use crate::compiler::lexer::Lexer;
use crate::compiler::parser::Parser;
use inkwell::context::Context;

#[test]
fn test_integer_expression() {
    let input = "let x = 5; let y = 10; let z = y + x * 2;";
    let mut lexer = Lexer::new(input);
    let mut parser = Parser::new(&mut lexer);
    let program = parser.parse_program();

    let context = Context::create();
    let mut codegen = CodeGenerator::new(&context, "test");
    codegen.compile_program(program).unwrap();

    let module = codegen.get_module();
    let ir = module.print_to_string().to_string();

    println!("Generated IR:\n{}", ir);

    // Assert that the variables are allocated and stored correctly
    assert!(ir.contains("%x = alloca i64"));
    assert!(ir.contains("store i64 5, ptr %x"));
    assert!(ir.contains("%y = alloca i64"));
    assert!(ir.contains("store i64 10, ptr %y"));
    assert!(ir.contains("%z = alloca i64"));

    // Assert that the arithmetic operations are correct
    assert!(ir.contains("mul i64 %x2, 2"));
    assert!(ir.contains("add i64 %y1, %mul"));
    assert!(ir.contains("store i64 %add, ptr %z"));
}
