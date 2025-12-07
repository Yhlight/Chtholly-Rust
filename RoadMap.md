# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development is divided into several milestones, each focusing on a specific set of features.

## Milestone 1: Core Language and Infrastructure

- [ ] **Parser:** Implement a robust parser for the Chtholly language syntax.
- [ ] **AST (Abstract Syntax Tree):** Define and build the AST to represent the code's structure.
- [ ] **Type System:** Implement a basic type system with type checking and inference.
- [ ] **Code Generation (LLVM):** Set up the LLVM backend for generating machine code.
- [ ] **Standard Library:** Create a minimal standard library with basic I/O and data structures.

## Milestone 2: Advanced Language Features

- [ ] **Ownership and Borrowing:** Implement the ownership and borrowing system for memory safety.
- [ ] **Classes and Structs:** Add support for classes and structs, including member variables and methods.
- [ ] **Enums:** Implement enums with support for associated data.
- [ ] **Generics:** Add support for generic functions and types.
- [ ] **Error Handling:** Implement the `Result<T, E>` type and the `?` operator for error handling.

## Milestone 3: Concurrency and Tooling

- [ ] **Concurrency:** Add support for concurrent programming with threads and channels.
- [ ] **Package Manager:** Create a package manager for managing dependencies and building projects.
- [ ] **Debugger:** Implement a debugger for debugging Chtholly programs.
- [ ] **LSP (Language Server Protocol):** Implement a language server for providing IDE features like code completion and diagnostics.

## Milestone 4: Optimization and Performance

- [ ] **Compiler Optimizations:** Implement various compiler optimizations to improve performance.
- [ ] **Garbage Collection:** Investigate and potentially implement a garbage collector for opt-in use cases.
- [ ] **FFI (Foreign Function Interface):** Add support for calling functions from other languages.
- [ ] **JIT (Just-In-Time) Compilation:** Implement a JIT compiler for dynamic code execution.
