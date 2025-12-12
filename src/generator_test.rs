use inkwell::context::Context;
use crate::lexer::Lexer;
use crate::parser::Parser;
use crate::generator::CodeGenerator;

#[test]
fn test_let_and_return() {
    let input = r#"
        let x = 5;
        return x;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let context = Context::create();
    let mut generator = CodeGenerator::new(&context);
    generator.generate(&program).unwrap();

    let expected_ir = "; ModuleID = 'main'\nsource_filename = \"main\"\n\ndefine i64 @main() {\nentry:\n  %x = alloca i64, align 8\n  store i64 5, ptr %x, align 4\n  %x1 = load i64, ptr %x, align 4\n  ret i64 %x1\n}\n";

    let actual_ir = generator.print_to_string();

    assert_eq!(expected_ir, actual_ir.replace("\r\n", "\n"));
}
