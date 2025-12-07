# Chtholly Language Implementation Roadmap

This document outlines the development plan and tracks the progress of the Chtholly programming language implementation.

## Phase 1: Core Language Features

- [ ] **Comments:**
  - [ ] Single-line comments (`//`)
  - [ ] Multi-line comments (`/* */`)
- [ ] **Main Function:**
  - [ ] `fn main(args: string[]): Result<i32, SystemError>`
- [ ] **Resource Management:**
  - [ ] Ownership
  - [ ] Borrowing
  - [ ] Move Semantics
- [ ] **Variables:**
  - [x] Immutable variables (`let`)
  - [x] Mutable variables (`mut`)
- [ ] **Data Types:**
  - [ ] Integer types (`i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`)
  - [ ] Floating-point types (`f32`, `f64`)
  - [ ] Character type (`char`)
  - [ ] Boolean type (`bool`)
  - [ ] Void type (`void`)
  - [ ] String type (`string`)
  - [ ] Array types (`T[]`, `T[N]`)
- [ ] **Type Casting:**
  - [ ] `type_cast<T>()`
- [ ] **Operators:**
  - [ ] Arithmetic operators (`+`, `-`, `*`, `/`, `%`)
  - [ ] Comparison operators (`==`, `!=`, `>`, `>=`, `<`, `<=`)
  - [ ] Logical operators (`!`, `&&`, `||`)
  - [ ] Bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`)
  - [ ] Assignment operators (`=`, `+=`, `-=`, `*=`, `/=`, `%=`)
  - [ ] Increment and decrement operators (`++`, `--`)
- [ ] **Functions:**
  - [x] Function definition (`fn`)
  - [ ] Function calls
  - [ ] Lambda expressions
- [ ] **Control Flow:**
  - [ ] `if-else` statements
  - [ ] `switch-case` statements
  - [ ] `while` loops
  - [ ] `for` loops
  - [ ] `foreach` loops
  - [ ] `do-while` loops
- [ ] **Error Handling:**
  - [ ] `Result<T, E>` type
  - [ ] `?` operator
  - [ ] Pattern matching on `Result`

## Phase 2: Composite Types

- [ ] **Structs:**
  - [ ] Definition
  - [ ] Instantiation
  - [ ] Member access
  - [ ] Member functions
- [ ] **Classes:**
  - [ ] Definition
  - [ ] Member variables (`let`, `mut`)
  - [ ] Constructors
  - [ ] Destructors
  - [ ] Member functions (`self`, `&self`, `&mut self`)
  - [ ] Access modifiers (`public`, `private`)
- [ ] **Enums:**
  - [ ] Definition
  - [ ] Enums with states
  - [ ] Pattern matching on enums

## Phase 3: Advanced Features

- [ ] **Generics:**
  - [ ] Generic functions
  - [ ] Generic classes
  - [ ] Generic member functions in classes
- [ ] **Modules:**
  - [ ] `import` statement
  - [ ] `use` statement
  - [ ] `package` statement
- [ ] **Optional Type:**
  - [ ] `optional<T>`
  - [ ] `unwrap` and `unwrap_or`
- [ ] **Requests (Traits/Interfaces):**
  - [ ] `request` definition
  - [ ] `require` clause
  - [ ] `Self` type in requests
  - [ ] Associated types
  - [ ] Associated constraints
- [ ] **Built-in Constraints:**
  - [ ] `iterator` module

## Phase 4: Standard Library

- [ ] **IO Stream:**
  - [ ] `iostream` module (`print`, `println`)
- [ ] **String Manipulation:**
  - [ ] `string` module (`combine`, `concat`)
- [ ] **File System:**
  - [ ] `file` module (`open`)
- [ ] **Optional:**
  - [ ] `optional` module
- [ ] **Iterator:**
  - [ ] `iterator` module

## Phase 5: Tooling and Backend

- [ ] **LLVM Backend:**
  - [ ] Code generation for all language features
- [ ] **Build System:**
  - [ ] Compiler driver
  - [ ] Package manager (optional)
- [ ] **Testing Framework:**
  - [ ] Unit testing framework
  - [ ] Integration testing framework
