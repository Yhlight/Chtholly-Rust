
mod parser;

use parser::{ChthollyParser, Rule};
use pest::Parser;
use std::env;
use std::fs;
use std::process;

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

    let parse_result = ChthollyParser::parse(Rule::program, &source_code);

    match parse_result {
        Ok(pairs) => {
            println!("--- 解析树 ---");
            println!("{:#?}", pairs);
            println!("-------------------");
        }
        Err(e) => {
            eprintln!("解析错误：{}", e);
            process::exit(1);
        }
    }
}
