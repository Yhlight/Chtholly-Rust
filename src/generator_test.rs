use inkwell::context::Context;
use crate::lexer::Lexer;
use crate::parser::Parser;
use crate::generator::CodeGenerator;
use crate::semantic::SemanticAnalyzer;

#[test]
fn test_let_and_return() {
    let input = r#"
        let x = 5;
        return x;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut semantic_analyzer = SemanticAnalyzer::new();
    semantic_analyzer.analyze(&program);

    let context = Context::create();
    let mut generator = CodeGenerator::new(&context, &mut semantic_analyzer);
    generator.generate(&program).unwrap();

    let actual_ir = generator.print_to_string();

    assert!(actual_ir.contains("define i64 @main()"));
    assert!(actual_ir.contains("%x = alloca i64"));
    assert!(actual_ir.contains("store i64 5, ptr %x"));
    assert!(actual_ir.contains("ret i64"));
}

#[test]
fn test_float() {
    let input = "let x = 5.5;";

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut semantic_analyzer = SemanticAnalyzer::new();
    semantic_analyzer.analyze(&program);

    let context = Context::create();
    let mut generator = CodeGenerator::new(&context, &mut semantic_analyzer);
    generator.generate(&program).unwrap();

    let actual_ir = generator.print_to_string();

    assert!(actual_ir.contains("store double 5.5"));
}

#[test]
fn test_boolean() {
    let input = "let x = true;";

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut semantic_analyzer = SemanticAnalyzer::new();
    semantic_analyzer.analyze(&program);

    let context = Context::create();
    let mut generator = CodeGenerator::new(&context, &mut semantic_analyzer);
    generator.generate(&program).unwrap();

    let actual_ir = generator.print_to_string();

    assert!(actual_ir.contains("store i1 true"));
}

#[test]
fn test_char() {
    let input = "let x = 'a';";

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut semantic_analyzer = SemanticAnalyzer::new();
    semantic_analyzer.analyze(&program);

    let context = Context::create();
    let mut generator = CodeGenerator::new(&context, &mut semantic_analyzer);
    generator.generate(&program).unwrap();

    let actual_ir = generator.print_to_string();

    assert!(actual_ir.contains("store i8 97"));
}

#[test]
fn test_infix_expression() {
    let input = r#"
        let x = 5;
        let y = 10;
        let z = x + y;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut semantic_analyzer = SemanticAnalyzer::new();
    semantic_analyzer.analyze(&program);

    let context = Context::create();
    let mut generator = CodeGenerator::new(&context, &mut semantic_analyzer);
    generator.generate(&program).unwrap();

    let actual_ir = generator.print_to_string();

    assert!(actual_ir.contains("load i64, ptr %x"));
    assert!(actual_ir.contains("load i64, ptr %y"));
    assert!(actual_ir.contains("add i64"));
    assert!(actual_ir.contains("store i64 %addtmp, ptr %z"));
}

#[test]
fn test_if_statement() {
    let input = "if (true) { let x = 5; }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    let mut semantic_analyzer = SemanticAnalyzer::new();
    semantic_analyzer.analyze(&program);
    let context = Context::create();
    let mut generator = CodeGenerator::new(&context, &mut semantic_analyzer);
    generator.generate(&program).unwrap();
    let actual_ir = generator.print_to_string();

    assert!(actual_ir.contains("br i1 true, label %then, label %else"));
    assert!(actual_ir.contains("then:"));
    assert!(actual_ir.contains("else:"));
    assert!(actual_ir.contains("merge:"));
}

#[test]
fn test_while_statement() {
    let input = "while (true) { }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    let mut semantic_analyzer = SemanticAnalyzer::new();
    semantic_analyzer.analyze(&program);
    let context = Context::create();
    let mut generator = CodeGenerator::new(&context, &mut semantic_analyzer);
    generator.generate(&program).unwrap();
    let actual_ir = generator.print_to_string();

    assert!(actual_ir.contains("loop_cond:"));
    assert!(actual_ir.contains("br i1 true, label %loop_body, label %loop_end"));
    assert!(actual_ir.contains("loop_body:"));
    assert!(actual_ir.contains("br label %loop_cond"));
    assert!(actual_ir.contains("loop_end:"));
}
