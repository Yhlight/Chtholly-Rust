#include "semantic_analyzer.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <vector>
#include <cassert>

void test(const std::string& source, bool should_pass, const std::string& expected_error = "")
{
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();
    Chtholly::SemanticAnalyzer analyzer;

    try
    {
        analyzer.analyze(statements);
        if (!should_pass)
        {
            assert(false && "Test was expected to fail, but it passed.");
        }
    }
    catch (const std::runtime_error& e)
    {
        if (should_pass)
        {
            assert(false);
        }
        assert(std::string(e.what()) == expected_error);
    }
}

void test_ownership()
{
    // Valid cases
    test("let a: i32 = 10; let b: i32 = a;", true);
    test("let mut c: i32 = 20; c = 30;", true);

    // Invalid cases
    test("let x: string = \"hello\"; let y: string = x; let z: string = x;", false, "Variable 'x' was moved.");
    test("let s1: string = \"world\"; let s2: string = s1; let len: i32 = s1;", false, "Variable 's1' was moved.");
    test("let imm: i32 = 5; imm = 10;", false, "Cannot assign to immutable variable: imm");
}

void test_borrowing()
{
    // Valid cases
    test("let a: i32 = 10; let b: &i32 = &a;", true);
    test("let mut c: i32 = 20; let d: &mut i32 = &mut c;", true);
    test("let e: i32 = 30; let f: &i32 = &e; let g: &i32 = &e;", true);

    // Invalid cases
    test("let h: i32 = 40; let i: &i32 = &h; let j: &mut i32 = &mut h;", false, "Cannot mutably borrow 'h' as it is already borrowed.");
    test("let mut k: i32 = 50; let l: &mut i32 = &mut k; let m: &i32 = &k;", false, "Cannot immutably borrow 'k' as it is already mutably borrowed.");
    test("let n: string = \"hello\"; let o: &string = &n; let p: string = n;", false, "Cannot move 'n' as it is borrowed.");
    test("let mut q: i32 = 60; let r: &i32 = &q; q = 70;", false, "Cannot assign to 'q' as it is immutably borrowed.");
}

void test_functions()
{
    // Valid cases
    test("fn a() {}", true);
    test("fn b(x: i32, y: i32) {}", true);
    test("fn c(): i32 { return 10; }", true);
    test("fn d() {} d();", true);
    test("fn e(x: i32, y: i32) {} e(1, 2);", true);

    // Invalid cases
    test("fn f(x: i32) {} f();", false, "Expected 1 arguments but got 0.");
    test("fn g() { return 10; }", false, "Cannot return a value from a function with no return type.");
    test("fn h(): i32 { return; }", false, "Cannot return without a value from a function with a return type.");
    test("return 10;", false, "Cannot return from top-level code.");
}

void test_reassign_moved()
{
    test("let mut x: string = \"hello\"; let y: string = x; x = \"world\";", true);
}

void test_mismatched_types()
{
    test("fn a(x: i32) {} a(\"hello\");", false, "Mismatched types in function call.");
}

void test_nested_borrows()
{
    test("let mut x: i32 = 10; let y: &i32 = &x; { let z: &i32 = &x; } let w: &i32 = &x;", true);
}

void test_type_inference()
{
    test("let x = 10;", true);
    test("let y = \"hello\";", true);
    test("let z: i32 = 10.0;", false, "Mismatched types in variable declaration.");
    test("let a;", false, "Cannot infer type of variable 'a'.");
}


int main()
{
    test_ownership();
    test_borrowing();
    test_functions();
    test_reassign_moved();
    test_mismatched_types();
    test_nested_borrows();
    test_type_inference();
    return 0;
}
