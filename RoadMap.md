# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Core Language and Compiler Infrastructure

- [x] **Project Setup**
  - [x] Initialize Rust project with Cargo.
  - [x] Configure `.gitignore`.
  - [x] Set up LLVM 18 environment.
- [ ] **Compiler Infrastructure**
  - [ ] Lexer (Tokenizer)
  - [ ] Abstract Syntax Tree (AST)
  - [ ] Parser
  - [ ] LLVM Code Generator
- [ ] **Comments**
  - [ ] Single-line comments (`//`)
  - [ ] Multi-line comments (`/* */`)
- [ ] **Basic Language Features**
  - [ ] `main` function entry point
  - [ ] Integer and floating-point literals
  - [ ] String literals
  - [ ] Character literals

## Phase 2: Variables and Data Types

- [ ] **Variable Declarations**
  - [ ] Immutable variables (`let`)
  - [ ] Mutable variables (`mut`)
  - [ ] Type inference
  - [ ] Type annotations
- [ ] **Data Types**
  - [ ] `int` (i32) and other integer types (`i8`, `i16`, `i64`, `u8`, `u16`, `u32`, `u64`)
  - [ ] `double` (f64) and `float` (f32)
  - [ ] `char`
  - [ ] `bool`
  - [ ] `string`
  - [ ] `void`
- [ ] **Type Casting**
  - [ ] `type_cast<T>()`

## Phase 3: Operators and Expressions

- [ ] **Arithmetic Operators**
  - [ ] `+`, `-`, `*`, `/`, `%`
- [ ] **Comparison Operators**
  - [ ] `==`, `!=`, `>`, `>=`, `<`, `<=`
- [ ] **Logical Operators**
  - [ ] `!`, `&&`, `||`
- [ ] **Bitwise Operators**
  - [ ] `&`, `|`, `^`, `~`, `<<`, `>>`
- [ ] **Assignment Operators**
  - [ ] `=`, `+=`, `-=`, `*=`, `/=`, `%=`
- [ ] **Increment/Decrement Operators**
  - [ ] `++`, `--`

## Phase 4: Control Flow

- [ ] **Conditional Statements**
  - [ ] `if-else`
  - [ ] `switch-case` with `break` and `fallthrough`
- [ ] **Loops**
  - [ ] `while` loop
  - [ ] `for` loop (C-style)
  - [ ] `do-while` loop

## Phase 5: Functions and Scope

- [ ] **Function Definitions**
  - [ ] `fn` keyword
  - [ ] Return types
  - [ ] Function calls
- [ ] **Lambda Expressions**
  - [ ] Capturing external variables
- [ ] **Scope and Ownership**
  - [ ] Ownership and move semantics
  - [ ] Borrowing (`&` and `&mut`)

## Phase 6: Compound Data Types

- [ ] **Arrays**
  - [ ] Static arrays (`int[4]`)
  - [ ] Dynamic arrays (`int[]`)
- [ ] **Structs**
  - [ ] `struct` definitions
  - [ ] Member functions
- [ ] **Enums**
  - [ ] Enum definitions
  - [ ] Enums with associated data
- [ ] **Classes**
  - [ ] `class` definitions
  - [ ] Member variables (`let`, `mut`)
  - [ ] Constructors
  - [ ] Destructors (`~`)
  - [ ] Member functions (with `self`, `&self`, `&mut self`)
  - [ ] Access modifiers (`public`, `private`)

## Phase 7: Advanced Features

- [ ] **Generics**
  - [ ] Generic functions
  - [ ] Generic classes
- [ ] **Error Handling**
  - [ ] `Result<T, E>` type
  - [ ] `?` operator for error propagation
- [ ] **Modules and Imports**
  - [ ] `import` statement for files and standard library modules
  - [ ] `use` statement for bringing symbols into scope
  - [ ] `package` declaration
- [ ] **Iterators**
  - [ ] `foreach` loop
  - [ ] `iterator` requests
- [ ] **Optional Type**
  - [ ] `optional<T>`
- [ ] **Requests (Traits/Interfaces)**
  - [ ] `request` definitions
  - [ ] Associated types
  - [ ] `require` keyword for dependencies

## Phase 8: Standard Library

- [ ] **IO Module**
  - [ ] `println` and `print` functions
- [ ] **String Module**
  - [ ] String manipulation functions
- [ ] **Math Module**
  - [ ] Basic mathematical functions

This roadmap will be updated as the project progresses.
