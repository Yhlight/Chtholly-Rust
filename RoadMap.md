# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development will follow a Scrum + TDD methodology, with a focus on incremental implementation and stability.

## Phase 1: Core Language Features

### Sprint 1: Project Setup and Basic I/O
- [x] Create `RoadMap.md`
- [ ] Initialize Rust project with Cargo.
- [ ] Add LLVM dependencies (`inkwell`).
- [ ] Create `.gitignore`.
- [ ] Implement basic `main` function parsing.
- [ ] Implement `println` function for basic output.
- [ ] Add basic tests for the `main` function and `println`.

### Sprint 2: Variables and Data Types
- [ ] Implement `let` for immutable variables.
- [ ] Implement `mut` for mutable variables.
- [ ] Implement basic data types: `i32`, `f64`, `char`, `string`, `bool`.
- [ ] Implement type inference and type annotations.
- [ ] Add tests for variable declaration and data types.

### Sprint 3: Operators
- [ ] Implement arithmetic operators (`+`, `-`, `*`, `/`, `%`).
- [ ] Implement comparison operators (`==`, `!=`, `>`, `>=`, `<`, `<=`).
- [ ] Implement logical operators (`&&`, `||`, `!`).
- [ ] Implement bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`).
- [ ] Implement assignment operators (`=`, `+=`, `-=`, `*=`, `/=`, `%=`).
- [ ] Implement increment and decrement operators (`++`, `--`).
- [ ] Add tests for all operators.

### Sprint 4: Control Flow
- [ ] Implement `if-else` statements.
- [ ] Implement `while` loops.
- [ ] Implement `for` loops.
- [ ] Implement `foreach` loops.
- [ ] Implement `do-while` loops.
- [ ] Implement `switch-case` statements.
- [ ] Add tests for all control flow structures.

## Phase 2: Functions and Scope

### Sprint 5: Functions
- [ ] Implement function definition and calling with `fn`.
- [ ] Implement function parameters and return values.
- [ ] Implement `void` return type.
- [ ] Implement first-class functions (assigning functions to variables).
- [ ] Add tests for functions.

### Sprint 6: Ownership and Borrowing
- [ ] Implement move semantics for variable assignment.
- [ ] Implement immutable borrowing (`&`).
- [ ] Implement mutable borrowing (`&mut`).
- [ ] Implement lifetime rules to prevent dangling pointers.
- [ ] Add tests for ownership and borrowing.

### Sprint 7: Lambdas
- [ ] Implement lambda expressions.
- [ ] Implement variable capturing (`[]`, `[&]`, `[&mut]`).
- [ ] Add tests for lambda expressions.

## Phase 3: Composite Data Types

### Sprint 8: Structs
- [ ] Implement `struct` definitions.
- [ ] Implement struct instantiation (aggregate and designated initialization).
- [ ] Implement member access.
- [ ] Implement member functions.
- [ ] Add tests for structs.

### Sprint 9: Classes
- [ ] Implement `class` definitions.
- [ ] Implement member variables (`let`, `mut`).
- [ ] Implement constructors.
- [ ] Implement destructors.
- [ ] Implement member functions and `self`.
- [ ] Implement access modifiers (`public`, `private`).
- [ ] Add tests for classes.

### Sprint 10: Enums
- [ ] Implement `enum` definitions.
- [ ] Implement enums with associated data.
- [ ] Implement pattern matching for enums in `if` and `switch`.
- [ ] Add tests for enums.

### Sprint 11: Arrays
- [ ] Implement static arrays (`i32[4]`).
- [ ] Implement dynamic arrays (`i32[]`).
- [ ] Implement array indexing.
- [ ] Add tests for arrays.

## Phase 4: Generics and Error Handling

### Sprint 12: Generics
- [ ] Implement generic functions.
- [ ] Implement generic classes.
- [ ] Implement generic member functions.
- [ ] Implement template specialization.
- [ ] Add tests for generics.

### Sprint 13: Error Handling
- [ ] Implement `Result<T, E>` enum.
- [ ] Implement the `?` operator for error propagation.
- [ ] Implement pattern matching on `Result` types.
- [ ] Add tests for error handling.

## Phase 5: Modules and Standard Library

### Sprint 14: Modules
- [ ] Implement `import` for file-based modules.
- [ ] Implement `import` for standard library modules.
- [ ] Implement `use` for bringing symbols into scope.
- [ ] Implement module aliasing.
- [ ] Implement `package` support.
- [ ] Add tests for the module system.

### Sprint 15: Standard Library
- [ ] Implement `iostream` module.
- [ ] Implement `optional` module.
- [ ] Implement `iterator` module with associated requests.
- [ ] Add tests for the standard library modules.

## Phase 6: Advanced Features

### Sprint 16: Requests (Traits/Interfaces)
- [ ] Implement `request` definitions.
- [ ] Implement `require` for request inheritance.
- [ ] Implement associated types.
- [ ] Implement associated constraints.
- [ ] Add tests for requests.

### Sprint 17: Type Casting
- [ ] Implement `type_cast<T>()`.
- [ ] Add tests for type casting.
