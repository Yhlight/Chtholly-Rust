use chtholly::codegen::Compiler;
use chtholly::lexer::Lexer;
use chtholly::parser::Parser;
use inkwell::context::Context;

#[test]
fn test_integer_arithmetic() {
    let tests = vec![
        ("let z = 5 + 5; return z;", 10),
        ("let z = 10 - 5; return z;", 5),
        ("let z = 2 * 8; return z;", 16),
        ("let z = 10 / 2; return z;", 5),
    ];

    for (input, expected_val) in tests {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        compiler.compile_program(program);

        let execution_engine = compiler.module.create_jit_execution_engine(inkwell::OptimizationLevel::None).unwrap();
        let main = unsafe { execution_engine.get_function::<unsafe extern "C" fn() -> i32>("main").unwrap() };

        unsafe {
            assert_eq!(main.call(), expected_val);
        }
    }
}

#[test]
fn test_logical_operators() {
    let tests = vec![
        ("return true && false;", 0),
        ("return true || false;", 1),
        ("return false && true;", 0),
        ("return false || false;", 0),
    ];

    for (input, expected_val) in tests {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        compiler.compile_program(program);

        let execution_engine = compiler.module.create_jit_execution_engine(inkwell::OptimizationLevel::None).unwrap();
        let main = unsafe { execution_engine.get_function::<unsafe extern "C" fn() -> i32>("main").unwrap() };

        unsafe {
            assert_eq!(main.call(), expected_val);
        }
    }
}

#[test]
fn test_arithmetic_with_variables() {
    let input = r#"
        let x = 10;
        let y = 20;
        return x + y;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program().unwrap();

    let context = Context::create();
    let mut compiler = Compiler::new(&context);
    compiler.compile_program(program);

    let execution_engine = compiler.module.create_jit_execution_engine(inkwell::OptimizationLevel::None).unwrap();
    let main = unsafe { execution_engine.get_function::<unsafe extern "C" fn() -> i32>("main").unwrap() };

    unsafe {
        assert_eq!(main.call(), 30);
    }
}

#[test]
fn test_comparison_operators() {
    let tests = vec![
        ("return 5 == 5;", 1),
        ("return 5 != 10;", 1),
        ("return 2 < 8;", 1),
        ("return 10 > 5;", 1),
        ("return 5 == 6;", 0),
        ("return 5 != 5;", 0),
        ("return 8 < 2;", 0),
        ("return 5 > 10;", 0),
    ];

    for (input, expected_val) in tests {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        compiler.compile_program(program);

        let execution_engine = compiler.module.create_jit_execution_engine(inkwell::OptimizationLevel::None).unwrap();
        let main = unsafe { execution_engine.get_function::<unsafe extern "C" fn() -> i32>("main").unwrap() };

        unsafe {
            assert_eq!(main.call(), expected_val);
        }
    }
}
