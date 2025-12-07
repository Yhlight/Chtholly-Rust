## 环境脚本

为了方便 Chtholly 语言的开发，我们提供了一个自动化脚本来安装必要的依赖项。此脚本适用于基于 Debian/Ubuntu 的系统。对于其他操作系统，请参考官方文档进行安装。

```bash
#!/bin/bash

# 更新包列表
sudo apt-get update

# 安装 Rust 和 Cargo
# 这会下载并运行官方的 rustup 安装脚本
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y

# 将 Cargo 的 bin 目录添加到当前 shell 的 PATH 中
source $HOME/.cargo/env

# 安装 LLVM 14（或你需要的特定版本）和 Clang
sudo apt-get install -y llvm-14-dev libclang-14-dev clang-14

echo "开发环境已设置完毕！"
echo "请重新启动你的 shell 或运行 'source $HOME/.cargo/env' 以使 Rust 工具链生效。"
```

## 项目要求

1. Chtholly 是一门基于 Rust 编写的编程语言，使用 LLVM 作为后端，语法规范文档是 `Chtholly.md`（已经提供）。

2. 项目使用 `.gitignore` 进行版本控制，不要提交构建产物与二进制文件。

3. Scrum + TDD 驱动，以稳定项目为主，增量实现，避免回归。

4. 每一个计划的推进前，都需要重新阅读 `Chtholly.md` 中相关语法规范。

5. 需要创建一个 `RoadMap.md`（尽可能详细）记录项目的进度，每一个功能的实现都需要通过此文档进行反馈。
