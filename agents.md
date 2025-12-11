## 环境设置

为了方便 Chtholly 语言的开发，请遵循以下步骤来设置您的开发环境。这些说明主要针对基于 Debian/Ubuntu 的系统。

### 安装 LLVM 18

标准的 Ubuntu 仓库可能没有最新版本的 LLVM，或者可能存在链接问题。推荐使用 LLVM 官方提供的 APT 仓库进行安装。

```bash
#!/bin/bash

# 获取发行版代号
CODENAME=$(lsb_release -cs)

echo "正在添加 LLVM 18 官方 APT 仓库..."

# 添加 LLVM APT 仓库的 GPG 密钥
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo gpg --dearmor -o /etc/apt/trusted.gpg.d/llvm-snapshot.gpg

# 添加 LLVM 18 的 APT 仓库
sudo add-apt-repository "deb http://apt.llvm.org/${CODENAME}/ llvm-toolchain-${CODENAME}-18 main"

# 更新包列表
sudo apt-get update

# 安装 Clang C++ 编译器和 LLVM 18 核心开发依赖
# - clang-18: Clang C++ 编译器
# - libc++-18-dev: Clang 标准库 libc++
# - llvm-18-dev: 核心 LLVM 开发文件 (头文件、配置脚本等)
# - libpolly-18-dev 和 libzstd-dev: 保留，以确保所有必要的 LLVM 组件完整安装
sudo apt-get install -y \
    clang-18 \
    libc++-18-dev \
    llvm-18-dev \
    libpolly-18-dev \
    libzstd-dev
```

### 环境变量的配置

除此之外，你需要设置环境变量LLVM_DIR，尽管CMake能够自动查找LLVM所在的位置。

但我们推荐还是手动指定LLVM的CMake文件的位置。

## 项目要求

1. Chtholly 是一门基于 C++17 编写的编程语言，使用 CMake 进行构建， Python脚本辅助构建，LLVM 作为后端，语法规范文档是 `Chtholly.md`（已经提供）。

2. 项目使用 `.gitignore` 进行版本控制，不要提交构建产物与二进制文件。

3. TDD 驱动，以稳定项目为主，增量实现，避免回归。

4. 每一个计划的推进前，都需要重新阅读 `Chtholly.md` 中相关语法规范。

5. 需要创建一个 `RoadMap.md`（尽可能详细）记录项目的进度，每一个功能的实现都需要通过此文档进行反馈。

6. Chtholly核心要点是所有权，借用检查和自动生命周期管理，编译期体系，为此RoadMap.md必须重点突出此。
