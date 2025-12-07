## 环境设置

为了方便 Chtholly 语言的开发，请遵循以下步骤来设置您的开发环境。这些说明主要针对基于 Debian/Ubuntu 的系统。

### 1. 安装 Rust 和 Cargo

```bash
# 下载并运行官方的 rustup 安装脚本
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y

# 将 Cargo 的 bin 目录添加到当前 shell 的 PATH 中
source $HOME/.cargo/env
```

### 2. 安装 LLVM 18

标准的 Ubuntu 仓库可能没有最新版本的 LLVM，或者可能存在链接问题。推荐使用 LLVM 官方提供的 APT 仓库进行安装。

```bash
#!/bin/bash

# 获取发行版代号
CODENAME=$(lsb_release -cs)

# 添加 LLVM APT 仓库的 GPG 密钥
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo gpg --dearmor -o /etc/apt/trusted.gpg.d/llvm-snapshot.gpg

# 添加 LLVM 18 的 APT 仓库
sudo add-apt-repository "deb http://apt.llvm.org/${CODENAME}/ llvm-toolchain-${CODENAME}-18 main"

# 更新包列表并安装 LLVM 18
sudo apt-get update
sudo apt-get install -y llvm-18-dev libclang-18-dev clang-18
```

### 3. 配置环境变量

`inkwell` (via `llvm-sys`) 需要知道在哪里找到 LLVM。您可以通过设置以下环境变量来指定 LLVM 的安装路径：

```bash
export LLVM_SYS_180_PREFIX=/usr/lib/llvm-18
```

将此行添加到您的 `.bashrc` 或 `.zshrc` 文件中，以使其在新的 shell 会话中永久生效。

## 项目要求

1. Chtholly 是一门基于 Rust 编写的编程语言，使用 LLVM 作为后端，语法规范文档是 `Chtholly.md`（已经提供）。

2. 项目使用 `.gitignore` 进行版本控制，不要提交构建产物与二进制文件。

3. Scrum + TDD 驱动，以稳定项目为主，增量实现，避免回归。

4. 每一个计划的推进前，都需要重新阅读 `Chtholly.md` 中相关语法规范。

5. 需要创建一个 `RoadMap.md`（尽可能详细）记录项目的进度，每一个功能的实现都需要通过此文档进行反馈。
