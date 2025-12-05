# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Milestone 1: Core Language and Infrastructure

- [ ] **Project Setup:**
  - [ ] Initialize Rust project with Cargo.
  - [ ] Add LLVM dependencies.
  - [ ] Set up `.gitignore`.
- [ ] **Basic Syntax Parsing:**
  - [ ] Comments (single-line and multi-line).
  - [ ] Main function definition.
- [ ] **Abstract Syntax Tree (AST):**
  - [ ] Define initial AST nodes.
- [ ] **Testing:**
  - [ ] Set up the testing framework.
  - [ ] Implement initial parser tests.

## Milestone 2: Variables and Data Types

- [ ] **Variable Declarations:**
  - [ ] `let` (immutable) variables.
  - [ ] `mut` (mutable) variables.
- [ ] **Data Types:**
  - [ ] Integer types (`int`, `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`).
  - [ ] Floating-point types (`double`, `float`).
  - [ ] `char` type.
  - [ ] `bool` type.
  - [ ] `string` type.
- [ ] **Type Inference and Annotations:**
  - [ ] Implement type inference for variable declarations.
  - [ ] Support explicit type annotations.

## Milestone 3: Ownership, Borrowing, and Operators

- [ ] **Ownership System:**
  - [ ] Implement move semantics for variable assignments.
  - [ ] Implement `Copy` trait for basic types.
- [ ] **Borrowing and References:**
  - [ ] `&` (immutable) references.
  - [ ] `&mut` (mutable) references.
  - [ ] Implement the borrow checker rules.
- [ ] **Operators:**
  - [ ] Arithmetic operators.
  - [ ] Comparison operators.
  - [ ] Logical operators.
  - [ ] Bitwise operators.
  - [ ] Assignment operators.

## Milestone 4: Control Flow

- [ ] **Conditional Statements:**
  - [ ] `if-else` statements.
  - [ ] `switch-case` statements.
- [ ] **Looping Constructs:**
  - [ ] `while` loops.
  - [ ] `for` loops.
  - [ ] `foreach` loops.
  - [ ] `do-while` loops.

## Milestone 5: Functions and Classes

- [ ] **Functions:**
  - [ ] Function definitions and calls.
  - [ ] Lambda expressions.
- [ ] **Classes:**
  - [ ] Class definitions.
  - [ ] Member variables (`let` and `mut`).
  - [ ] Constructors.
  - [ ] Destructors.
  - [ ] Member functions (including `self`, `&self`, `&mut self`).
  - [ ] Access modifiers (`public`, `private`).
- [ ] **Structs:**
  - [ ] Struct definitions.
  - [ ] Struct initialization.

## Milestone 6: Advanced Features

- [ ] **Generics:**
  - [ ] Generic functions.
  - [ ] Generic classes.
- [ ] **Modules and Imports:**
  - [ ] `import` statements for files and standard library modules.
  - [ ] `use` statements for importing specific members.
- [ ] **Error Handling:**
  - [ ] `Result<T, E>` type.
  - [ ] `?` operator for error propagation.

## Milestone 7: LLVM Backend and Code Generation

- [ ] **LLVM Integration:**
  - [ ] Set up LLVM context and module.
- [ ] **Code Generation:**
  - [ ] Generate LLVM IR for all implemented language features.
- [ ] **Compiler Driver:**
  - [ ] Create a command-line interface for the compiler.
  - [ ] Compile Chtholly source code to an executable.
