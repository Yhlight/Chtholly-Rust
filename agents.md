## 环境脚本
```
#!/bin/bash

# 遇到错误立即停止
set -e

echo "--- 开始配置 LLVM 18 环境 (适用于 Rust 开发) ---"

# 1. 安装基础依赖
echo "[1/4] 安装基础依赖工具..."
sudo apt-get update -y
sudo apt-get install -y wget lsb-release software-properties-common gnupg

# 2. 使用官方脚本安装 LLVM 18
echo "[2/4] 下载并安装 LLVM 18 (这可能需要几分钟)..."
# 只有当 llvm.sh 不存在时才下载，避免重复下载
if [ ! -f "llvm.sh" ]; then
    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh
fi

# 参数 18 表示安装 LLVM 18 版本
sudo ./llvm.sh 18

# 安装额外的开发包（这对 Rust bindgen/llvm-sys 至关重要）
echo "正在安装 LLVM 18 开发库..."
sudo apt-get install -y libllvm18 llvm-18-dev llvm-18-runtime clang-18 libclang-18-dev

# 清理下载的脚本
rm -f llvm.sh

# 3. 配置环境变量
echo "[3/4] 配置环境变量..."

# 定义 LLVM 18 的安装路径
LLVM_PATH="/usr/lib/llvm-18"
LLVM_BIN="$LLVM_PATH/bin"

# 准备要写入的环境变量内容
ENV_CONFIG="
# --- LLVM 18 for Rust Config Start ---
export PATH=\"$LLVM_BIN:\$PATH\"
export LLVM_SYS_180_PREFIX=\"$LLVM_PATH\"
# 防止 bindgen 找不到 clang
export LIBCLANG_PATH=\"$LLVM_PATH/lib\"
# --- LLVM 18 for Rust Config End ---
"

# --- 修复的部分开始 ---
# 智能检测 Shell 配置文件
# 使用 :- 语法，如果变量未定义则返回空，避免 unbound variable 错误
SHELL_CONFIG_FILE="$HOME/.bashrc"

if [ -n "${ZSH_VERSION:-}" ]; then
    SHELL_CONFIG_FILE="$HOME/.zshrc"
elif [ -n "${BASH_VERSION:-}" ]; then
    SHELL_CONFIG_FILE="$HOME/.bashrc"
else
    # 默认回退
    SHELL_CONFIG_FILE="$HOME/.profile"
fi
# --- 修复的部分结束 ---

echo "检测到配置文件: $SHELL_CONFIG_FILE"

# 检查是否已经配置过，避免重复写入
if grep -q "LLVM_SYS_180_PREFIX" "$SHELL_CONFIG_FILE"; then
    echo "警告: 环境变量似乎已经存在于 $SHELL_CONFIG_FILE 中，跳过写入。"
else
    # 确保文件存在
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
echo "1. 输入 llvm-config-18 --version 确认版本"
echo "2. 检查 echo \$LLVM_SYS_180_PREFIX 是否输出 $LLVM_PATH"
```

## 项目要求

1. Chtholly是一门基于Rust编写的编程语言，使用LLVM作为后端，语法规范文档是Chtholly.md(已经提供)。

2. 项目使用.gitignore进行版本控制，不要提交构建产物与二进制文件。

3. TDD驱动，以稳定项目为主，增量实现，避免回归。

4. 每一个计划的推进前，都需要重新阅读Chtholly.md中相关语法规范。

5. 需要创建一个RoadMap.md(尽可能详细)记录项目的进度，每一个功能的实现都需要通过此文档进行反馈。
