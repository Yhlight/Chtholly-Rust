# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Milestone 1: Core Language and Infrastructure

- [x] **Project Setup:**
  - [x] Initialize Rust project with Cargo.
  - [x] Add LLVM dependencies.
  - [x] Set up `.gitignore`.
- [x] **Basic Syntax Parsing:**
  - [x] Comments (single-line and multi-line).
  - [x] Main function definition.
- [x] **Abstract Syntax Tree (AST):**
  - [x] Define initial AST nodes.
- [x] **Testing:**
  - [x] Set up the testing framework.
  - [x] Implement initial parser tests.

## Milestone 2: Variables and Data Types

- [x] **Variable Declarations:**
  - [x] `let` (immutable) variables.
  - [x] `mut` (mutable) variables.
- [x] **Data Types:**
  - [x] Integer types (`int`, `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`).
  - [x] Floating-point types (`double`, `float`).
  - [x] `char` type.
  - [x] `bool` type.
  - [x] `string` type.
- [x] **Type Inference and Annotations:**
  - [x] Implement type inference for variable declarations.
  - [x] Support explicit type annotations.

## Milestone 3: Ownership, Borrowing, and Operators

- [ ] **Ownership System:**
  - [ ] Implement move semantics for variable assignments.
  - [ ] Implement `Copy` trait for basic types.
- [ ] **Borrowing and References:**
  - [ ] `&` (immutable) references.
  - [ ] `&mut` (mutable) references.
  - [ ] Implement the borrow checker rules.
- [x] **Operators:**
  - [x] Arithmetic operators.
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
