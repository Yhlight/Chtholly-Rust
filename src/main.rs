
use chumsky::Parser;
use inkwell::context::Context;
use std::env;
use std::fs;
use chumsky::Stream;

mod compiler;
use compiler::codegen::Compiler;
use compiler::lexer::lexer;
use compiler::parser::parser;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: chtholly <file>");
        return;
    }
    let filename = &args[1];

    let source_code = match fs::read_to_string(filename) {
        Ok(code) => code,
        Err(e) => {
            eprintln!("Failed to read file {}: {}", filename, e);
            return;
        }
    };

    let (tokens, lex_errs) = lexer().parse_recovery(source_code.as_str());
    if let Some(tokens) = tokens {
        if !lex_errs.is_empty() {
            eprintln!("Lexer errors: {:?}", lex_errs);
            return;
        }

        let (ast, parse_errs) = parser().parse_recovery(Stream::from_iter(
            0..tokens.len(),
            tokens.into_iter(),
        ));

        if let Some(ast) = ast {
            if !parse_errs.is_empty() {
                eprintln!("Parser errors: {:?}", parse_errs);
                return;
            }

            let context = Context::create();
            let compiler = Compiler::new(&context);

            // Create a main function
            let i64_type = context.i64_type();
            let fn_type = i64_type.fn_type(&[], false);
            let function = compiler.module.add_function("main", fn_type, None);
            let basic_block = context.append_basic_block(function, "entry");
            compiler.builder.position_at_end(basic_block);

            match compiler.compile_expr(&ast) {
                Ok(result) => {
                    if result.is_int_value() {
                        let int_value = result.into_int_value();
                        if int_value.get_type() == context.bool_type() {
                            if let Ok(extended_value) = compiler.builder.build_int_z_extend(int_value, i64_type, "zext") {
                                let _ = compiler.builder.build_return(Some(&extended_value));
                            } else {
                                eprintln!("Failed to build zext");
                            }
                        } else {
                            let _ = compiler.builder.build_return(Some(&int_value));
                        }
                    } else if result.is_float_value() {
                        // For now, we just return 0 for float results
                        let _ = compiler.builder.build_return(Some(&i64_type.const_int(0, false)));
                    } else {
                        eprintln!("Result is not an integer or float, cannot return from main");
                    }
                }
                Err(e) => {
                    eprintln!("Compiler error: {}", e);
                    return;
                }
            }

            println!("{}", compiler.module.print_to_string().to_string());
        } else {
            eprintln!("Failed to parse AST");
        }
    } else {
        eprintln!("Failed to lex tokens");
    }
}
