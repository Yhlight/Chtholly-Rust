cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM 18.1 REQUIRED CONFIG)

# Add LLVM include directories and definitions
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add the executable
add_executable(chtholly src/main.cpp src/Lexer.cpp src/Parser.cpp src/SymbolTable.cpp src/SemanticAnalyzer.cpp src/TokenTypeToString.cpp src/TypeResolver.cpp src/CodeGenerator.cpp src/LifetimeManager.cpp)

# Link against LLVM libraries
target_link_libraries(chtholly PRIVATE
    LLVMCore
    LLVMSupport
    LLVMAnalysis
)
