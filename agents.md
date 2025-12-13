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
    libzstd-dev \
    lld-18 \
    lldb-18 \
    clang-tools-18 \
    clang-format-18 \
    libclang-rt-18-dev \
    libomp-18-dev
```

### Toolchain-LLVM18

为了得到一个合适的LLVM环境，您需要根据以下步骤配置。

#### 创建工具链文件 (Toolchain-LLVM18.cmake)

请在您的项目根目录下创建一个名为 `Toolchain-LLVM18.cmake` 的文件，并粘贴以下内容：

```cmake
# Toolchain-LLVM18.cmake

set(CMAKE_C_COMPILER clang-18)
set(CMAKE_CXX_COMPILER clang++-18)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

set(LINKER_FLAGS "-fuse-ld=lld-18 -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")

set(LLVM_DIR "/usr/lib/llvm-18/lib/cmake/llvm")

find_program(LLVM_AR NAMES llvm-ar-18 llvm-ar REQUIRED)
find_program(LLVM_RANLIB NAMES llvm-ranlib-18 llvm-ranlib REQUIRED)
set(CMAKE_AR ${LLVM_AR})
set(CMAKE_RANLIB ${LLVM_RANLIB})
```

#### 使用工具链文件配置 CMake

请在您的项目根目录执行：

```bash
cmake -S /app -B /app/build -DCMAKE_TOOLCHAIN_FILE=Toolchain-LLVM18.cmake
```

## 项目要求

1. Chtholly 是一门基于 C++17 编写的编程语言，使用 CMake 进行构建， Python脚本辅助构建，LLVM 作为后端，语法规范文档是 `Chtholly.md`（已经提供）。

2. 项目使用 `.gitignore` 进行版本控制，不要提交构建产物与二进制文件。

3. TDD 驱动，以稳定项目为主，增量实现，避免回归。

4. 每一个计划的推进前，都需要重新阅读 `Chtholly.md` 中相关语法规范。

5. 需要创建一个 `RoadMap.md`（尽可能详细）记录项目的进度，每一个功能的实现都需要通过此文档进行反馈。

6. Chtholly核心要点是所有权，借用检查和自动生命周期管理，编译期体系，为此RoadMap.md必须重点突出此。

7. 请务必根据agents.md进行环境的配置。
