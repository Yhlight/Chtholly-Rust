mod ast;
mod ast_builder;
mod codegen;
mod parser;

use inkwell::context::Context;
use parser::{ChthollyParser, Rule};
use pest::Parser;
use std::env;
use std::fs;
use std::process::{self, Command, Stdio};

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() != 2 {
        eprintln!("用法: {} <file.cns>", args[0]);
        process::exit(1);
    }

    let file_path = &args[1];

    if !file_path.ends_with(".cns") {
        eprintln!("错误：输入文件必须具有 .cns 扩展名。");
        process::exit(1);
    }

    println!("正在编译文件：{}", file_path);
    let source_code = match fs::read_to_string(file_path) {
        Ok(code) => code,
        Err(e) => {
            eprintln!("读取文件 '{}' 时出错：{}", file_path, e);
            process::exit(1);
        }
    };

    println!("--- 源代码 ---");
    println!("{}", source_code);
    println!("-------------------");

    let cst = match ChthollyParser::parse(Rule::program, &source_code) {
        Ok(cst) => cst,
        Err(e) => {
            eprintln!("解析错误：{}", e);
            process::exit(1);
        }
    };

    let ast = ast_builder::build_ast_from_cst(cst);

    println!("--- 抽象语法树 ---");
    println!("{:#?}", ast);
    println!("-------------------");

    let context = Context::create();
    let mut codegen = codegen::CodeGen::new(&context, "chtholly_module");

    match codegen.generate_ir(&ast) {
        Ok(ir) => {
            println!("--- LLVM IR ---");
            println!("{}", ir);
            println!("-----------------");

            // Save the IR to a file
            let ir_file_path = "output.ll";
            fs::write(ir_file_path, ir).expect("无法写入 LLVM IR 文件");
            println!("LLVM IR 已保存到 {}", ir_file_path);

            // Compile the LLVM IR with clang
            let output_executable = "output";
            let clang_output = Command::new("clang")
                .arg(ir_file_path)
                .arg("-o")
                .arg(output_executable)
                .output()
                .expect("无法执行 clang");

            if !clang_output.status.success() {
                eprintln!("Clang 编译失败：");
                eprintln!("{}", String::from_utf8_lossy(&clang_output.stderr));
                process::exit(1);
            }

            println!("Clang 编译成功！");

            // Execute the compiled program
            println!("--- 程序输出 ---");
            let program_output = Command::new(format!("./{}", output_executable))
                .stdout(Stdio::piped())
                .stderr(Stdio::piped())
                .output()
                .expect("无法执行程序");

            if program_output.status.success() {
                println!("{}", String::from_utf8_lossy(&program_output.stdout));
            } else {
                eprintln!("程序执行失败：");
                eprintln!("{}", String::from_utf8_lossy(&program_output.stderr));
            }
            println!("--------------------");

        }
        Err(e) => {
            eprintln!("代码生成错误：{}", e);
            process::exit(1);
        }
    }
}
