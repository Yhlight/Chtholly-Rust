# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Core Language Features

- [ ] **Lexer:** Implement the lexical analyzer to tokenize the source code.
- [ ] **Parser:** Implement the parser to build an Abstract Syntax Tree (AST) from the tokens.
- [ ] **AST:** Define the data structures for the AST.
- [ ] **Comments:** Implement support for single-line and multi-line comments.
- [ ] **Main Function:** Implement the entry point of the program.
- [ ] **Variables:**
    - [ ] Immutable variables (`let`).
    - [ ] Mutable variables (`mut`).
    - [ ] Type inference.
    - [ ] Type annotations.
- [ ] **Data Types:**
    - [ ] `int`, `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`
    - [ ] `double`, `float`
    - [ ] `char`
    - [ ] `bool`
    - [ ] `void`
    - [ ] `string`
- [ ] **Operators:**
    - [ ] Arithmetic: `+`, `-`, `*`, `/`, `%`
    - [ ] Comparison: `==`, `!=`, `>`, `>=`, `<`, `<=`
    - [ ] Logical: `!`, `&&`, `||`
    - [ ] Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>`
    - [ ] Assignment: `=`, `+=`, `-=`, `*=`, `/=`, `%=`
    - [ ] Unary: `++`, `--`
- [ ] **Functions:**
    - [ ] Function definition and calls.
    - [ ] Function as a variable.
- [ ] **Control Flow:**
    - [ ] `if-else` statements.
    - [ ] `switch-case` statements.
    - [ ] `while` loops.
    - [ ] `for` loops.
    - [ ] `do-while` loops.
    - [ ] `foreach` loops.
- [ ] **Arrays:**
    - [ ] Static arrays (`int[4]`).
    - [ ] Dynamic arrays (`int[]`).

## Phase 2: Advanced Language Features

- [ ] **Ownership and Borrowing:**
    - [ ] Ownership system.
    - [ ] Move semantics.
    - [ ] Borrowing (`&` and `&mut`).
- [ ] **Classes:**
    - [ ] Class definition.
    - [ ] Member variables (`let` and `mut`).
    - [ ] Constructors.
    - [ ] Destructors.
    - [ ] Member functions (`self`, `&self`, `&mut self`).
    - [ ] Access modifiers (`public`, `private`).
- [ ] **Structs:**
    - [ ] Struct definition.
    - [ ] Member variables.
    - [ ] Member functions.
- [ ] **Enums:**
    - [ ] Enum definition.
    - [ ] Enums with states.
- [ ] **Generics:**
    - [ ] Generic functions.
    - [ ] Generic classes.
- [ ] **Error Handling:**
    - [ ] `Result<T, E>` type.
    - [ ] `?` operator.
- [ ] **Modules:**
    - [ ] `import` statement.
    - [ ] `use` statement.
    - [ ] `package` statement.

## Phase 3: Standard Library

- [ ] `iostream` module.
- [ ] `optional` module.
- [ ] `iterator` module.

## Phase 4: LLVM Backend

- [ ] **Code Generation:**
    - [ ] Implement the LLVM IR code generation from the AST.
- [ ] **Compiler:**
    - [ ] Create a command-line interface for the compiler.
