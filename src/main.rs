// 注意：本项目在 Ubuntu 上构建时可能会遇到 LLVM 链接错误
// （特别是关于 "Polly" 库）。这似乎是由于 llvm-sys crate
// 和 Ubuntu 的 LLVM 软件包之间的不兼容性造成的。
//
// 要解决此问题，请在构建时设置以下环境变量：
// export LLVM_SYS_140_PREFIX=/usr/lib/llvm-14
//
// 如果问题仍然存在，请考虑使用 llvmenv 或从源代码构建 LLVM。

use clap::Parser;

/// Chtholly 语言的编译器
#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// 要编译的源文件
    #[arg(short, long)]
    file: String,
}

fn main() {
    let args = Args::parse();

    println!("正在编译文件: {}", args.file);

    // 在未来的 Sprint 中，我们将在这里添加词法分析、解析、语义分析和代码生成。
    // 例如：
    // let source_code = std::fs::read_to_string(args.file).expect("无法读取源文件");
    // let tokens = lexer::lex(&source_code);
    // let ast = parser::parse(tokens);
    // ...
}
