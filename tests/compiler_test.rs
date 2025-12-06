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
    assert!(function
        .print_to_string()
        .to_string()
        .contains("define i32 @main()"));
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
    assert!(function
        .print_to_string()
        .to_string()
        .contains("alloca i32"));
    assert!(function
        .print_to_string()
        .to_string()
        .contains("store i32 5, ptr %x"));
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
    assert!(function
        .print_to_string()
        .to_string()
        .contains("alloca i32"));
    assert!(function
        .print_to_string()
        .to_string()
        .contains("store i32 5, ptr %x"));
}

#[test]
fn test_integer_arithmetic() {
    let tests = vec![
        ("5 + 5", "ret i32 10"),
        ("5 - 5", "ret i32 0"),
        ("5 * 5", "ret i32 25"),
        ("5 / 5", "ret i32 1"),
    ];

    for (input, expected) in tests {
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
        assert!(function.print_to_string().to_string().contains(expected));
    }
}

#[test]
fn test_boolean_expressions_compiler() {
    let tests = vec![("true", "ret i1 true"), ("false", "ret i1 false")];

    for (input, expected) in tests {
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
        assert!(function.print_to_string().to_string().contains(expected));
    }
}

#[test]
fn test_comparison_expressions_compiler() {
    let tests = vec![
        ("1 < 2", "ret i1 true"),
        ("1 > 2", "ret i1 false"),
        ("1 == 2", "ret i1 false"),
        ("1 != 2", "ret i1 true"),
    ];

    for (input, expected) in tests {
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
        assert!(function.print_to_string().to_string().contains(expected));
    }
}

#[test]
fn test_bang_operator_compiler() {
    let tests = vec![("!true", "ret i1 false"), ("!false", "ret i1 true")];

    for (input, expected) in tests {
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
        assert!(function.print_to_string().to_string().contains(expected));
    }
}

#[test]
fn test_if_expression_compiler() {
    let input = "if (true) { 10 }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let context = Context::create();
    let module = context.create_module("main");
    let builder = context.create_builder();
    let mut compiler = Compiler::new(&context, &builder, &module);
    let result = compiler.compile(program);

    assert!(result.is_err());
}

#[test]
fn test_if_else_expression_compiler() {
    let input = "if (true) { 10 } else { 20 }";
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
    assert!(function
        .print_to_string()
        .to_string()
        .contains("phi i32 [ 10, %then ], [ 20, %else ]"));
}

#[test]
fn test_variable_use() {
    let input = "fn main() { let x = 5; x }";
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
    assert!(function
        .print_to_string()
        .to_string()
        .contains("load i32, ptr %x"));
}

#[test]
fn test_function_name() {
    let input = "fn myFunction() {}";
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
    assert!(function
        .print_to_string()
        .to_string()
        .contains("define i32 @myFunction()"));
}

#[test]
fn test_multiple_functions() {
    let input = r#"
        fn anotherFunction() {}
        fn main() {}
    "#;
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
    assert!(function
        .print_to_string()
        .to_string()
        .contains("define i32 @main()"));
    assert!(module.get_function("anotherFunction").is_some());
}

#[test]
fn test_while_loop_compiler() {
    let input = "fn main() { while (true) {} }";
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
    let output = function.print_to_string().to_string();

    assert!(output.contains("loop_cond:"));
    assert!(output.contains("loop_body:"));
    assert!(output.contains("loop_end:"));
    assert!(output.contains("br label %loop_cond"));
    assert!(output.contains("br i1 true, label %loop_body, label %loop_end"));
}

#[test]
fn test_while_loop_with_break_compiler() {
    let input = "fn main() { while (true) { break; } }";
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
    let output = function.print_to_string().to_string();

    assert!(output.contains("loop_cond:"));
    assert!(output.contains("loop_body:"));
    assert!(output.contains("loop_end:"));
    assert!(output.contains("br label %loop_end"));
}

#[test]
fn test_while_loop_with_continue_compiler() {
    let input = "fn main() { while (true) { continue; } }";
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
    let output = function.print_to_string().to_string();

    assert!(output.contains("loop_cond:"));
    assert!(output.contains("loop_body:"));
    assert!(output.contains("loop_end:"));
    assert!(output.contains("br label %loop_cond"));
}

#[test]
fn test_logical_and_comparison_operators_compiler() {
    let tests = vec![
        ("1 >= 2", "ret i1 false"),
        ("2 >= 1", "ret i1 true"),
        ("1 <= 2", "ret i1 true"),
        ("2 <= 1", "ret i1 false"),
        ("true && false", "ret i1 false"),
        ("true || false", "ret i1 true"),
    ];

    for (input, expected) in tests {
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
        assert!(function.print_to_string().to_string().contains(expected));
    }
}

#[test]
fn test_nested_while_loop_compiler() {
    let input = r#"
        fn main() {
            while (true) {
                while (true) {
                    break;
                }
                break;
            }
        }
    "#;
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
    let output = function.print_to_string().to_string();

    // Use regex to count the occurrences of the loop-related basic blocks.
    let re_cond = regex::Regex::new(r"loop_cond\d*:").unwrap();
    let re_body = regex::Regex::new(r"loop_body\d*:").unwrap();
    let re_end = regex::Regex::new(r"loop_end\d*:").unwrap();

    assert_eq!(re_cond.find_iter(&output).count(), 2);
    assert_eq!(re_body.find_iter(&output).count(), 2);
    assert_eq!(re_end.find_iter(&output).count(), 2);
}
