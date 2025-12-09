# Chtholly Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development is structured in phases, with a primary focus on ensuring memory safety through ownership, borrowing, and lifetime management.

## Phase 1: Core Language Features & Compiler Infrastructure

### Milestone 1.1: Project Bootstrap & Lexical Analysis
- [x] Setup CMake build system.
- [x] Implement the Lexer to tokenize Chtholly source code (`.cns` files).
- [x] Support for basic data types: `i32`, `f64`, `bool`, `char`, `string`.
- [x] Support for comments (`//` and `/* */`).
- [x] Recognize the main function signature: `fn main(...)`.

### Milestone 1.2: Parser & Abstract Syntax Tree (AST)
- [x] Implement the Parser to construct an AST from the token stream.
- [x] Support for variable declarations (`let` and `let mut`).
- [x] Support for basic arithmetic and logical expressions.
- [ ] Initial implementation of function definitions (`fn`).

### Milestone 1.3: Semantic Analysis & Type System
- [ ] Implement a semantic analyzer to traverse the AST and perform type checking.
- [ ] Create a symbol table to store information about variables and functions.
- [ ] Enforce type compatibility rules for assignments and expressions.

### Milestone 1.4: Code Generation (LLVM IR)
- [ ] Integrate the LLVM libraries into the build process.
- [ ] Traverse the AST to generate LLVM Intermediate Representation (IR).
- [ ] Compile a basic "Hello, World!" program into an executable binary.

## Phase 2: Ownership, Borrowing, and Lifetimes

This phase is the cornerstone of Chtholly's memory safety guarantees.

### Milestone 2.1: Ownership System
- [ ] Implement move semantics as the default for variable assignments and function arguments.
- [ ] Track the ownership of resources within the semantic analyzer.
- [ ] Automatically insert code to deallocate (drop) resources when their owner goes out of scope.
- [ ] Implement a `Copy` trait for primitive types to enable copy semantics where appropriate.

### Milestone 2.2: Borrowing & References
- [ ] Implement immutable references (`&T`) to allow shared, read-only access to resources.
- [ ] Implement mutable references (`&mut T`) to allow exclusive, read-write access.
- [ ] At compile time, enforce the core borrowing rules:
    - Any number of immutable references (`&T`) can exist simultaneously.
    - Only one mutable reference (`&mut T`) can exist at any given time.
    - A mutable reference cannot coexist with any other references (mutable or immutable).

### Milestone 2.3: Lifetime Management
- [ ] Implement lifetime analysis to prevent dangling references.
- [ ] Ensure that no reference can outlive the data it points to.
- [ ] Implement lifetime elision rules to automatically infer lifetimes in common scenarios, minimizing the need for explicit annotations.

## Phase 3: Advanced Language Features

### Milestone 3.1: Control Flow
- [ ] `if-else` expressions.
- [ ] `while` and `do-while` loops.
- [ ] C-style `for` loops.
- [ ] `foreach` loops based on the iterator pattern.
- [ ] `switch-case` statements with improved semantics (e.g., no fall-through by default).

### Milestone 3.2: Compound Data Types
- [ ] `struct`: User-defined data structures.
- [ ] `class`: Structs with methods, constructors, destructors, and access modifiers.
- [ ] `enum`: Algebraic data types, including enums with associated data.
- [ ] `array`: Both static (`T[N]`) and dynamic (`T[]`) arrays.

### Milestone 3.3: Generics & Contracts
- [ ] Generic functions (`fn add<T>(a: T, b: T) -> T`).
- [ ] Generic data structures (`class Point<T>`).
- [ ] Implement the `contract` system for defining interfaces (similar to Rust's traits).
- [ ] Support for associated types within contracts.

## Phase 4: Standard Library & Tooling

### Milestone 4.1: Core Standard Library
- [ ] `iostream` module for console input and output.
- [ ] `optional<T>` type for representing optional values.
- [ ] `Result<T, E>` type for robust error handling.
- [ ] A comprehensive `string` manipulation library.
- [ ] Basic container types, such as `Vector` and `HashMap`.

### Milestone 4.2: Build & Package Management
- [ ] Develop a command-line interface (CLI) for the Chtholly compiler.
- [ ] Design and implement a package manager for handling dependencies.
- [ ] Establish a standard project structure and module system.

## Phase 5: Concurrency
- [ ] Define a memory model for concurrent programming in Chtholly.
- [ ] Implement safe concurrency primitives (e.g., channels, mutexes) that are integrated with the ownership and borrowing system to prevent data races at compile time.
