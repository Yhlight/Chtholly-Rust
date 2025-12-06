## 环境脚本
```
#!/bin/bash

echo "--- 1. 安装 Rust 工具链 (rustup) ---"
# 安装 curl，用于下载 rustup 脚本
sudo apt update
sudo apt install -y curl build-essential

# 安装 Rust
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y

# 激活 Rust 环境变量
source $HOME/.cargo/env
echo "Rust 安装完成。"

echo "--- 2. 添加 LLVM 18 官方存储库 ---"
# LLVM 官方推荐的添加方式，保证版本正确

# 1. 导入 LLVM 官方 GPG 密钥
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo gpg --dearmor -o /etc/apt/keyrings/llvm-archive-keyring.gpg

# 2. 将密钥文件移动到正确位置
sudo chmod 644 /etc/apt/keyrings/llvm-archive-keyring.gpg

# 3. 添加 LLVM 18 存储库 (假设您使用的是 Ubuntu 22.04 LTS 或类似版本)
RELEASE=$(lsb_release -sc)
echo "deb [signed-by=/etc/apt/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/$RELEASE/ llvm-toolchain-$RELEASE-18 main" | sudo tee /etc/apt/sources.list.d/llvm.list > /dev/null

echo "--- 3. 安装 LLVM 18 核心开发库 ---"
sudo apt update

# 安装 clang-18, libllvm-18-dev (核心开发库)
sudo apt install -y clang-18 libllvm-18-dev

echo "--- 4. 配置 LLVM 环境变量 (关键步骤) ---"

# 定义 LLVM 18 的安装路径。在 Debian/Ubuntu 系统上，它通常位于 /usr/lib/llvm-18
LLVM_18_PATH="/usr/lib/llvm-18"

# 导出环境变量：告诉 Rust 的 llvm-sys 库在哪里找到 LLVM 18
# 181 是 llvm-sys 库期望的版本前缀
export LLVM_SYS_181_PREFIX=$LLVM_18_PATH

# 为了方便，将这个环境变量写入用户的 ~/.bashrc 文件，使其永久生效
echo "export LLVM_SYS_181_PREFIX=$LLVM_18_PATH" >> ~/.bashrc
echo "export PATH=\$PATH:$LLVM_18_PATH/bin" >> ~/.bashrc

echo "--- 环境配置完成! ---"
echo "Rust 版本: $(rustc --version)"
echo "LLVM 18 开发环境已安装并配置。"
echo "!!! 请重启终端会话或运行 'source ~/.bashrc' 使环境变量永久生效 !!!"
```

## 项目要求

1. Chtholly是一门基于Rust编写的编程语言，使用LLVM作为后端，语法规范文档是Chtholly.md

2. 项目使用.gitignore进行版本控制，不要提交构建产物与二进制文件

3. TDD驱动，以稳定项目为主，增量实现，避免回归

4. 每一个计划的推进前，都需要重新阅读Chtholly.md中相关语法规范

5. 需要创建一个RoadMap.md(尽可能详细)记录项目的进度，每一个功能的实现都需要通过此文档进行反馈
