use std::error::Error;
use std::fs::File;
use std::io::Write;
use std::path::Path;
use std::process::{Command, Stdio};

use inkwell::context::Context;

pub mod ast;
pub mod codegen;
pub mod errors;
pub mod lexer;
pub mod parser;

fn main() -> Result<(), Box<dyn Error>> {
    // === 1. Setup ===
    let context = Context::create();
    let generator = codegen::CodeGenerator::new(&context);

    // === 2. Code Generation ===
    // This simulates compiling a hardcoded AST for `fn main(): i32 { return 0; }`
    println!("[1/4] Generating LLVM IR...");
    let module = generator.compile_hardcoded_main()?;
    let ir_string = module.print_to_string().to_string();

    // === 3. Write IR to file ===
    let output_dir = Path::new("output");
    if !output_dir.exists() {
        std::fs::create_dir(output_dir)?;
    }
    let ll_path = output_dir.join("main.ll");
    let mut ll_file = File::create(&ll_path)?;
    writeln!(ll_file, "{}", ir_string)?;
    println!("[2/4] LLVM IR written to {:?}", ll_path);

    // === 4. Compile LLVM IR to an executable using clang ===
    let exe_path = output_dir.join("main");
    println!("[3/4] Compiling IR to executable using clang...");
    let clang_output = Command::new("clang-17")
        .arg(ll_path)
        .arg("-o")
        .arg(&exe_path)
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .output()?;

    if !clang_output.status.success() {
        return Err(format!(
            "Clang failed with status: {}",
            clang_output.status
        ).into());
    }
    println!("[4/4] Executable created at {:?}", exe_path);

    // === 5. Execute and verify ===
    println!("\n--- Running compiled program ---");
    let program_output = Command::new(&exe_path).status()?;
    println!("--- Program finished ---\n");

    match program_output.code() {
        Some(0) => {
            println!("✅ Success! Program exited with code 0.");
            Ok(())
        }
        Some(code) => {
            Err(format!("❌ Failure! Program exited with code {}.", code).into())
        }
        None => Err("❌ Failure! Program was terminated by a signal.".into()),
    }
}
