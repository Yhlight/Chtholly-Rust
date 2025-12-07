# Chtholly Development Roadmap

This document outlines the development plan for the Chtholly programming language. Each feature will be implemented in a separate phase to ensure stability and allow for incremental progress.

## Phase 1: Project Setup and Basic Syntax

- [ ] **Project Initialization:** Set up the Rust project using Cargo.
- [ ] **Roadmap:** Create this `RoadMap.md` file.
- [ ] **.gitignore:** Create a `.gitignore` file to exclude build artifacts.
- [ ] **Parser and AST:** Implement the basic parser and Abstract Syntax Tree (AST) structure.
- [ ] **Comments:** Implement support for single-line and multi-line comments.
- [ ] **Main Function:** Implement the `main` function as the entry point of the program.

## Phase 2: Variables and Data Types

- [ ] **Variable Declaration:** Implement `let` and `mut` for immutable and mutable variables.
- [ ] **Data Types:** Implement support for basic data types (`i32`, `f64`, `char`, `string`, `bool`).
- [ ] **Type Inference:** Implement type inference for variable declarations.
- [ ] **Type Annotations:** Implement support for explicit type annotations.

## Phase 3: Ownership and Borrowing

- [ ] **Ownership:** Implement the ownership system for resource management.
- [ ] **Move Semantics:** Implement move semantics for variable assignments and function calls.
- [ ] **Borrowing:** Implement borrowing with `&` and `&mut` for immutable and mutable references.
- [ ] **Lifetime Management:** Implement lifetime management to prevent dangling references.

## Phase 4: Operators and Control Flow

- [ ] **Operators:** Implement standard arithmetic, comparison, logical, and bitwise operators.
- [ ] **If-Else:** Implement `if-else` conditional statements.
- [ ] **Switch-Case:** Implement `switch-case` statements.
- [ ] **Loops:** Implement `while`, `for`, `foreach`, and `do-while` loops.

## Phase 5: Functions and Classes

- [ ] **Functions:** Implement function definitions and calls.
- [ ] **Lambda Expressions:** Implement lambda expressions.
- [ ] **Classes:** Implement class definitions, including member variables, constructors, and destructors.
- [ ] **Member Functions:** Implement member functions with `self`, `&self`, and `&mut self`.
- [ ] **Access Modifiers:** Implement `public` and `private` access modifiers.

## Phase 6: Structs, Enums, and Generics

- [ ] **Structs:** Implement struct definitions and usage.
- [ ] **Enums:** Implement enums with associated data.
- [ ] **Generics:** Implement generic functions and classes.

## Phase 7: Modules and Error Handling

- [ ] **Modules:** Implement the module system with `import` and `use`.
- [ ] **Error Handling:** Implement `Result<T, E>` for error handling.
- [ ] **`?` Operator:** Implement the `?` operator for error propagation.

## Phase 8: Standard Library

- [ ] **IO Stream:** Implement a basic I/O stream library.
- [ ] **Optional:** Implement the `optional` type.
- [ ] **Iterator:** Implement the `iterator` trait and related features.

## Phase 9: LLVM Backend

- [ ] **LLVM Integration:** Integrate LLVM as the backend for code generation.
- [ ] **Code Generation:** Implement code generation for all implemented language features.
- [ ] **Compilation:** Implement the compilation process from Chtholly source code to executable.
