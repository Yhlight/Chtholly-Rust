# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development will be test-driven and incremental, focusing on stability and correctness.

## Phase 1: Core Language Features

This phase focuses on implementing the fundamental building blocks of the language.

1.  **Project Setup**
    *   [x] Initialize CMake project structure.
    *   [x] Set up basic build configurations.
    *   [x] Integrate LLVM as the backend.
    *   [x] Create a simple "Hello, World!" entry point.

2.  **Lexer and Parser**
    *   [x] Implement a lexer to tokenize the source code.
    *   [ ] Implement a parser to build an Abstract Syntax Tree (AST).
    *   [x] Support for basic syntax: comments, identifiers, literals.

3.  **Variables and Data Types**
    *   [ ] Implement `let` and `let mut` for variable declarations.
    *   [ ] Support for primitive data types (`i32`, `f64`, `bool`, `char`).
    *   [ ] Type inference and explicit type annotations.
    *   [ ] Support for basic operators (`+`, `-`, `*`, `/`, `=`, `==`, etc.).

4.  **Control Flow**
    *   [ ] Implement `if-else` expressions.
    *   [ ] Implement `while` and `for` loops.
    *   [ ] Implement `switch-case` statements.

5.  **Functions**
    *   [ ] Implement function definitions with `fn`.
    *   [ ] Support for function calls.
    *   [ ] Implement return types.

## Phase 2: Ownership, Borrowing, and Lifetimes

This is the core of Chtholly's safety guarantees.

1.  **Ownership System**
    *   [ ] Implement move semantics for variable assignments and function calls.
    *   [ ] Implement the `Copy` trait for primitive types.
    *   [ ] Implement destructors (`~ClassName`) to be called when an object goes out of scope.

2.  **Borrowing and References**
    *   [ ] Implement immutable borrows (`&`).
    *   [ ] Implement mutable borrows (`&mut`).
    *   [ ] Enforce the borrowing rules (one mutable borrow or multiple immutable borrows).

3.  **Lifetime Management**
    *   [ ] Implement lifetime analysis to prevent dangling references.
    *   [ ] Implement smart lifetime omission for simple cases.

## Phase 3: Composite Data Types

1.  **Structs**
    *   [ ] Implement `struct` definitions.
    *   [ ] Support for member access.
    *   [ ] Support for struct instantiation.

2.  **Classes**
    *   [ ] Implement `class` definitions.
    *   [ ] Implement constructors and destructors.
    *   [ ] Implement member functions with `self`, `&self`, and `&mut self`.
    *   [ ] Implement access modifiers (`public`, `private`).

3.  **Enums**
    *   [ ] Implement `enum` definitions.
    *   [ ] Support for enums with associated data.
    *   [ ] Implement pattern matching for enums (e.g., in `switch` statements).

## Phase 4: Modules and Generics

1.  **Module System**
    *   [ ] Implement `import` and `use` statements.
    *   [ ] Implement the `package` system for organizing modules.

2.  **Generics**
    *   [ ] Implement generic functions.
    *   [ ] Implement generic structs and classes.

## Phase 5: Compile-Time System

This phase focuses on the `const` system for zero-cost abstractions.

1.  **Const Evaluation**
    *   [ ] Implement `const` variables.
    *   [ ] Implement `const fn` for compile-time functions.
    *   [ ] Implement `const struct` for compile-time data structures.
    *   [ ] Enforce the rules of the `const` context (no side effects, etc.).

2.  **Compile-Time Error Handling**
    *   [ ] Support for `Result<T, E>` in `const fn`.
    *   [ ] Enforce compile-time error handling.

## Phase 6: Error Handling and Standard Library

1.  **Error Handling**
    *   [ ] Implement the `Result<T, E>` type.
    *   [ ] Implement the `?` operator for error propagation.

2.  **Standard Library**
    *   [ ] Implement a basic `iostream` module with `println`.
    *   [ ] Implement a `string` module with basic string manipulation functions.
    *   [ ] Implement the `optional` type.
