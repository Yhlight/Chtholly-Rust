# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development is guided by Scrum and TDD principles to ensure stability and incremental progress.

## Core Philosophy

Chtholly aims to provide a programming language that combines the performance of C++ with the memory safety of Rust. The core features that enable this are:

- **Ownership:** Every value has a single owner. When the owner goes out of scope, the value is dropped.
- **Borrowing:** Ownership can be temporarily transferred through borrowing, which is checked at compile time.
- **Lifetimes:** The compiler ensures that all borrows are valid for the duration of their use.

## Development Phases

The project will be developed in the following phases, with each phase building upon the previous one.

### Phase 1: Foundational Language Features

This phase focuses on implementing the basic building blocks of the language.

- **[x] Lexer:** Implement a lexical analyzer to tokenize the Chtholly source code.
- **[ ] Parser:** Implement a parser to build an Abstract Syntax Tree (AST) from the token stream. (In Progress: `let` statements)
- **[x] AST:** Define the structure of the AST to represent the language constructs.
- **[ ] Basic Type System:**
    - [ ] Implement primitive types (`i32`, `f64`, `bool`, `char`, `string`).
    - [ ] Implement `let` bindings for immutable variables.
    - [ ] Implement `let mut` bindings for mutable variables.

### Phase 2: Ownership, Borrowing, and Lifetimes

This is the most critical phase of the project, as it implements the core memory safety features of Chtholly.

- **[ ] Ownership and Move Semantics:**
    - [ ] Implement move semantics for variable assignments and function calls.
    - [ ] Ensure that moved values cannot be used afterward.
- **[ ] Borrowing and References:**
    - [ ] Implement immutable borrows (`&`).
    - [ ] Implement mutable borrows (`&mut`).
    - [ ] Enforce the borrowing rules (multiple readers or one writer).
- **[ ] Lifetime Management:**
    - [ ] Implement lifetime analysis to prevent dangling references.
    - [ ] Implement lifetime elision rules to simplify common patterns.

### Phase 3: Control Flow and Functions

This phase adds more complex control flow structures and function handling.

- **[ ] Functions:**
    - [ ] Implement function definitions with `fn`.
    - [ ] Implement function calls.
- **[ ] Control Flow:**
    - [ ] `if-else` expressions.
    - [ ] `while` loops.
    - [ ] `for` loops (C-style).
- **[ ] `switch-case` statements:**
    - [ ] Implement `switch-case` with fallthrough control.

### Phase 4: Composite Data Types

This phase introduces user-defined data types.

- **[ ] `struct`:**
    - [ ] Implement `struct` definitions.
    - [ ] Implement struct instantiation and field access.
- **[ ] `class`:**
    - [ ] Implement `class` definitions.
    - [ ] Implement constructors and destructors.
    - [ ] Implement member functions with `self`, `&self`, and `&mut self`.
    - [ ] Implement access control (`public`/`private`).
- **[ ] Enums:**
    - [ ] Implement `enum` definitions.
    - [ ] Implement enums with associated data.
    - [ ] Implement pattern matching for enums.

### Phase 5: Advanced Features

This phase adds more advanced and expressive features to the language.

- **[ ] Generics:**
    - [ ] Implement generic functions.
    - [ ] Implement generic `struct`s and `class`es.
- **[ ] Error Handling:**
    - [ ] Implement the `Result<T, E>` type.
    - [ ] Implement the `?` operator for error propagation.
- **[ ] Modules:**
    - [ ] Implement the `import` system for file-based modules.
    - [ ] Implement a package and module resolution system.
- **[ ] Lambdas:**
    - [ ] Implement lambda expressions with captures.
- **[ ] Contracts (Traits/Interfaces):**
    - [ ] Implement `contract` definitions.
    - [ ] Implement `contract` enforcement for `class`es.

### Phase 6: LLVM Backend Integration

This phase focuses on compiling the Chtholly AST to machine code using LLVM.

- **[ ] LLVM IR Generation:**
    - [ ] Set up the LLVM toolchain.
    - [ ] Implement a code generator to translate the AST to LLVM IR.
- **[ ] JIT Compilation:**
    - [ ] Implement a JIT compiler for running Chtholly code directly.
- **[ ] AOT Compilation:**
    - [ ] Implement ahead-of-time compilation to produce executable binaries.

### Phase 7: Standard Library

This phase involves creating a standard library for Chtholly.

- **[ ] `iostream` module:**
    - [ ] Basic input/output functions (`print`, `println`).
- **[ ] `optional` module:**
    - [ ] `optional<T>` type for handling nullable values.
- **[ ] `iterator` module:**
    - [ ] Implement the `iterator` contract and related features for `foreach` loops.

This roadmap will be updated as the project progresses. Each feature will be developed in a separate branch and merged into the main branch after passing all tests.
