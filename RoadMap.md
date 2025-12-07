# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development is divided into several phases, each focusing on a specific set of features. This roadmap will be updated as the project progresses.

## Phase 1: Core Language Features and Compiler Infrastructure

- [x] **Lexer (Tokenizer):** Implement a lexer to convert Chtholly source code into a stream of tokens.
- [x] **Parser:** Build a parser that takes the token stream and generates an Abstract Syntax Tree (AST).
- [x] **Basic AST:** Define AST nodes for fundamental language constructs like variables, literals, and expressions.
- [x] **LLVM Integration:** Set up the LLVM backend for code generation.
- [ ] **Variable Declarations:** Implement `let` and `mut` for variable declarations.
- [x] **Primitive Data Types:** Support for `i32`, `f64`, `char`, `bool`, and `string`.
- [x] **Basic Operators:** Implement arithmetic, comparison, and logical operators.
- [x] **Main Function:** The entry point of a Chtholly program.

## Phase 2: Control Flow and Functions

- [ ] **`if-else` Statements:** Implement conditional branching.
- [ ] **`switch-case` Statements:** Implement switch-case control flow.
- [ ] **Looping Constructs:**
    - [ ] `while` loops
    - [ ] `for` loops
    - [ ] `do-while` loops
- [ ] **Function Definitions and Calls:** Implement function declarations, calls, and returns.
- [ ] **Comments:** Support for single-line and multi-line comments.

## Phase 3: Ownership and Memory Management

- [ ] **Ownership System:** Implement the core ownership model.
- [ ] **Move Semantics:** Enforce move semantics for resource transfer.
- [ ] **Borrowing and References:** Implement shared (`&`) and mutable (`&mut`) borrows.
- [ ] **Lifetime Management:** Introduce lifetime analysis to prevent dangling references.

## Phase 4: Composite Data Types

- [ ] **Structs:** Implement user-defined struct types.
- [ ] **Classes:** Implement class definitions with member variables and methods.
- [ ] **Constructors and Destructors:** Support for constructors and destructors.
- [ ] **Access Modifiers:** Implement `public` and `private` access control.
- [ ] **Arrays:** Support for static and dynamic arrays.

## Phase 5: Advanced Features

- [ ] **Generics:**
    - [ ] Generic functions
    - [ ] Generic classes and structs
- [ ] **Enums:** Implement enums with associated data.
- [ ] **Error Handling:** Implement `Result<T, E>` and the `?` operator.
- [ ] **`optional` Type:** Implement an `optional` type for handling nullable values.
- [ ] **Type Casting:** Implement the `type_cast<T>()` function.
- [ ] **Lambdas:** Implement lambda expressions.
- [ ] **`foreach` loop:** Implement `foreach` loop with iterators.

## Phase 6: Module System and Standard Library

- [ ] **Module System:** Implement `import` and `use` for code organization.
- [ ] **Package Management:** Define a package structure for larger projects.
- [ ] **Standard Library:**
    - [ ] **`iostream`:** For input and output operations (`print`, `println`).
    - [ ] **`string`:** String manipulation functions.
    - [ ] **`math`:** Basic mathematical functions.
    - [ ] **`iterator`:** Support for iterators.

## Phase 7: Testing and Tooling

- [ ] **Unit Testing Framework:** Create a framework for writing and running tests.
- [ ] **Integration Tests:** Develop a suite of integration tests for the language.
- [ ] **Build System:** Refine the build process for Chtholly projects.
