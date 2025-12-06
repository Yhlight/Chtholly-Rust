## 环境脚本
```
#!/bin/bash

# 遇到错误立即停止
set -e

echo "--- 开始配置 LLVM 17 环境 (适用于 Rust 开发) ---"

# 1. 安装基础依赖
echo "[1/4] 安装基础依赖工具..."
sudo apt-get update -y
# 确保安装 wget 和 lsb-release，这是 llvm.sh 脚本需要的
sudo apt-get install -y wget lsb-release software-properties-common gnupg

# 2. 使用官方脚本安装 LLVM 17
echo "[2/4] 下载并安装 LLVM 17..."

# 下载官方安装脚本 (如果不存在)
if [ ! -f "llvm.sh" ]; then
    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh
fi

# 执行安装脚本，参数 17 代表 LLVM 17
# 注意：官方脚本会自动添加 apt 源并更新
sudo ./llvm.sh 17

# 安装额外的开发包 (Rust bindgen/llvm-sys 需要这些头文件)
echo "正在安装 LLVM 17 开发库..."
sudo apt-get install -y libllvm17 llvm-17-dev llvm-17-runtime clang-17 libclang-17-dev

# 清理安装脚本
rm -f llvm.sh

# 3. 配置环境变量
echo "[3/4] 配置环境变量..."

# 定义 LLVM 17 的安装路径
LLVM_PATH="/usr/lib/llvm-17"
LLVM_BIN="$LLVM_PATH/bin"

# 准备要写入的环境变量内容
# 注意：这里变量名变成了 LLVM_SYS_170_PREFIX
ENV_CONFIG="
# --- LLVM 17 for Rust Config Start ---
export PATH=\"$LLVM_BIN:\$PATH\"
export LLVM_SYS_170_PREFIX=\"$LLVM_PATH\"
# 防止 bindgen 找不到 clang
export LIBCLANG_PATH=\"$LLVM_PATH/lib\"
# --- LLVM 17 for Rust Config End ---
"

# 智能检测 Shell 配置文件 (包含之前的修复)
SHELL_CONFIG_FILE="$HOME/.bashrc"

if [ -n "${ZSH_VERSION:-}" ]; then
    SHELL_CONFIG_FILE="$HOME/.zshrc"
elif [ -n "${BASH_VERSION:-}" ]; then
    SHELL_CONFIG_FILE="$HOME/.bashrc"
else
    SHELL_CONFIG_FILE="$HOME/.profile"
fi

echo "检测到配置文件: $SHELL_CONFIG_FILE"

# 检查是否已经配置过，避免重复写入
if grep -q "LLVM_SYS_170_PREFIX" "$SHELL_CONFIG_FILE"; then
    echo "警告: LLVM 17 的配置似乎已经存在于 $SHELL_CONFIG_FILE 中，跳过写入。"
else
    touch "$SHELL_CONFIG_FILE"
    echo "$ENV_CONFIG" >> "$SHELL_CONFIG_FILE"
    echo "已将环境变量写入 $SHELL_CONFIG_FILE"
fi

# 4. 验证与提示
echo "--- 配置完成! ---"
echo "请执行以下命令使环境变量立即生效："
echo "source $SHELL_CONFIG_FILE"
echo ""
echo "验证方式:"
echo "1. 输入 llvm-config-17 --version 确认版本"
echo "2. 检查 echo \$LLVM_SYS_170_PREFIX (应输出 /usr/lib/llvm-17)"
```

## 项目要求

1. Chtholly是一门基于Rust编写的编程语言，使用LLVM17作为后端，语法规范文档是Chtholly.md(已经提供)。

2. 项目使用.gitignore进行版本控制，不要提交构建产物与二进制文件。

3. Scrum + TDD驱动，以稳定项目为主，增量实现，避免回归。

4. 每一个计划的推进前，都需要重新阅读Chtholly.md中相关语法规范。

5. 需要创建一个RoadMap.md(尽可能详细)记录项目的进度，每一个功能的实现都需要通过此文档进行反馈。
