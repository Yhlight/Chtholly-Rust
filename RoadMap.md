# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Core Language and Compiler Infrastructure

- [x] **Project Setup**
  - [x] Initialize Rust project with Cargo.
  - [x] Configure `.gitignore`.
  - [x] Set up LLVM 18 environment.
- [x] **Compiler Infrastructure**
  - [x] Lexer (Tokenizer)
  - [x] Abstract Syntax Tree (AST)
  - [x] Parser
  - [x] LLVM Code Generator
- [x] **Comments**
  - [x] Single-line comments (`//`)
  - [x] Multi-line comments (`/* */`)
- [x] **Basic Language Features**
  - [x] `main` function entry point
  - [x] Integer literals
  - [x] String literals
  - [x] Character literals

## Phase 2: Variables and Data Types

- [x] **Variable Declarations**
  - [x] Immutable variables (`let`)
  - [x] Mutable variables (`mut`)
  - [ ] Type inference
  - [ ] Type annotations
- [x] **Data Types**
  - [x] `int` (i32)
  - [ ] `double` (f64) and `float` (f32)
  - [ ] `char`
  - [x] `bool`
  - [ ] `string`
  - [ ] `void`
- [ ] **Type Casting**
  - [ ] `type_cast<T>()`

## Phase 3: Operators and Expressions

- [x] **Arithmetic Operators**
  - [x] `+`, `-`, `*`, `/`
  - [ ] `%`
- [x] **Comparison Operators**
  - [x] `==`, `!=`, `>`, `<`, `>=`, `<=`
- [x] **Logical Operators**
  - [x] `!`
  - [x] `&&`, `||`
- [ ] **Bitwise Operators**
  - [ ] `&`, `|`, `^`, `~`, `<<`, `>>`
- [ ] **Assignment Operators**
  - [ ] `=`, `+=`, `-=`, `*=`, `/=`, `%=`
- [ ] **Increment/Decrement Operators**
  - [ ] `++`, `--`

## Phase 4: Control Flow

- [x] **Conditional Statements**
  - [x] `if-else`
  - [ ] `switch-case` with `break` and `fallthrough`
- [x] **Loops**
  - [x] `while` loop
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
