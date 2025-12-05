# Chtholly Language Development Roadmap

This document outlines the development plan and progress for the Chtholly programming language.

## Phase 1: Foundations

- [x] **Project Setup**: Initialize the Rust project, configure `Cargo.toml`, and set up the basic directory structure.
- [x] **Lexer (Tokenizer)**: Implement the lexer to scan the source code and convert it into a stream of tokens.
    - [x] Support for comments (single-line and multi-line).
    - [x] Recognize keywords, identifiers, operators, and literals (integers, strings, chars).
- [x] **Parser & AST**: Implement the parser to consume the token stream and build an Abstract Syntax Tree (AST).
    - [x] Define AST node structures.
    - [x] Implement parsing for basic expressions and variable declarations.
- [x] **Operators**: Implement all standard arithmetic, comparison, logical, and bitwise operators.

## Phase 2: Core Language Features

- [ ] **Basic Data Types**: Implement support for primitive data types (`int`, `double`, `char`, `bool`, `string`, etc.).
- [ ] **Variables**: Implement variable declarations (`let`, `mut`) and assignments.
    - [ ] Type inference and explicit type annotations.
- [ ] **Basic Control Flow**:
    - [ ] `if-else` expressions.
    - [ ] `switch-case` statements.

## Phase 3: Functions, Scope, and Memory Management

- [ ] **Functions**:
    - [ ] Function definitions and calls.
    - [ ] Parameter passing and return values.
    - [ ] Support for `void` return type.
- [ ] **Scope Management**: Implement lexical scoping for variables and functions.
- [ ] **Ownership and Borrowing**:
    - [ ] Implement move semantics for variable assignments and function calls.
    - [ ] Implement borrowing with immutable (`&`) and mutable (`&mut`) references.
    - [ ] Enforce borrowing rules at compile time.

## Phase 4: Compound Data Types

- [ ] **Structs**: Implement struct definitions and instantiations.
    - [ ] Member access.
    - [ ] Member functions (methods).
- [ ] **Enums**: Implement enum definitions, including enums with associated data.
    - [ ] Pattern matching/destructuring in `if` and `switch`.
- [ ] **Arrays**:
    - [ ] Static arrays (`int[4]`).
    - [ ] Dynamic arrays (`int[]`).
    - [ ] Indexing and element access.

## Phase 5: Advanced Features

- [ ] **Classes**:
    - [ ] Class definitions with member variables and methods.
    - [ ] Constructors and destructors.
    - [ ] `public`/`private` access modifiers.
    - [ ] `Self` keyword implementation.
- [ ] **Generics**:
    - [ ] Generic functions.
    - [ ] Generic classes and structs.
    - [ ] Specialization.
- [ ] **Modules and Imports**:
    - [ ] Implement `import` for file-based modules.
    - [ ] Implement `package` and `use` keywords.
    - [ ] Namespace management.
- [ ] **Error Handling**:
    - [ ] Implement the `Result<T, E>` type.
    - [ ] Implement the `?` operator for error propagation.

## Phase 6: Standard Library and Tooling

- [ ] **Standard Library**:
    - [ ] `iostream` module for basic I/O.
    - [ ] `optional` module.
    - [ ] `iterator` module and related constraints.
- [ ] **Build System Integration**: Integrate the compiler with Cargo or a custom build script.
- [ ] **Documentation**: Write comprehensive documentation for the language and its standard library.
