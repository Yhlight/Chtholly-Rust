//! The Code Generator for the Chtholly language.

use crate::ast::{Program, Statement, Expression};
use llvm_sys::prelude::*;
use llvm_sys::core::*;
use std::ffi::CString;

pub struct Compiler {
    context: LLVMContextRef,
    module: LLVMModuleRef,
    builder: LLVMBuilderRef,
}

impl Compiler {
    pub fn new() -> Self {
        unsafe {
            let context = LLVMContextCreate();
            let module = LLVMModuleCreateWithNameInContext(CString::new("chtholly").unwrap().as_ptr(), context);
            let builder = LLVMCreateBuilderInContext(context);

            Compiler {
                context,
                module,
                builder,
            }
        }
    }

    pub fn compile(&mut self, program: Program) -> Result<String, String> {
        unsafe {
            let i64_type = LLVMInt64TypeInContext(self.context);
            let fn_type = LLVMFunctionType(i64_type, std::ptr::null_mut(), 0, 0);
            let function = LLVMAddFunction(self.module, CString::new("main").unwrap().as_ptr(), fn_type);
            let entry = LLVMAppendBasicBlockInContext(self.context, function, CString::new("entry").unwrap().as_ptr());
            LLVMPositionBuilderAtEnd(self.builder, entry);

            let mut result = None;

            for stmt in program.statements {
                result = self.compile_statement(stmt)?;
            }

            if let Some(value) = result {
                LLVMBuildRet(self.builder, value);
            }

            let module_str_ptr = LLVMPrintModuleToString(self.module);
            let module_str = CString::from_raw(module_str_ptr).to_string_lossy().into_owned();
            // LLVMDisposeMessage(module_str_ptr); // This would cause a double free, as from_raw takes ownership
            Ok(module_str)
        }
    }

    fn compile_statement(&mut self, stmt: Statement) -> Result<Option<LLVMValueRef>, String> {
        match stmt {
            Statement::Expression(exp_stmt) => {
                let value = self.compile_expression(exp_stmt.expression)?;
                Ok(Some(value))
            }
            _ => Err("Not implemented".to_string()),
        }
    }

    fn compile_expression(&mut self, exp: Expression) -> Result<LLVMValueRef, String> {
        unsafe {
            match exp {
                Expression::IntegerLiteral(int_lit) => {
                    let i64_type = LLVMInt64TypeInContext(self.context);
                    Ok(LLVMConstInt(i64_type, int_lit.value as u64, 0))
                }
                Expression::Boolean(boolean) => {
                    let i1_type = LLVMInt1TypeInContext(self.context);
                    Ok(LLVMConstInt(i1_type, boolean.value as u64, 0))
                }
                Expression::Prefix(prefix) => {
                    let right = self.compile_expression(*prefix.right)?;

                    match prefix.operator.as_str() {
                        "!" => Ok(LLVMBuildNot(self.builder, right, CString::new("not").unwrap().as_ptr())),
                        "-" => Ok(LLVMBuildNeg(self.builder, right, CString::new("neg").unwrap().as_ptr())),
                        _ => Err(format!("Unknown prefix operator: {}", prefix.operator)),
                    }
                }
                Expression::Infix(infix) => {
                    let left = self.compile_expression(*infix.left)?;
                    let right = self.compile_expression(*infix.right)?;

                    match infix.operator.as_str() {
                        "+" => Ok(LLVMBuildAdd(self.builder, left, right, CString::new("add").unwrap().as_ptr())),
                        "-" => Ok(LLVMBuildSub(self.builder, left, right, CString::new("sub").unwrap().as_ptr())),
                        "*" => Ok(LLVMBuildMul(self.builder, left, right, CString::new("mul").unwrap().as_ptr())),
                        "/" => Ok(LLVMBuildSDiv(self.builder, left, right, CString::new("div").unwrap().as_ptr())),
                        "==" => Ok(LLVMBuildICmp(self.builder, llvm_sys::LLVMIntPredicate::LLVMIntEQ, left, right, CString::new("eq").unwrap().as_ptr())),
                        "!=" => Ok(LLVMBuildICmp(self.builder, llvm_sys::LLVMIntPredicate::LLVMIntNE, left, right, CString::new("ne").unwrap().as_ptr())),
                        "<" => Ok(LLVMBuildICmp(self.builder, llvm_sys::LLVMIntPredicate::LLVMIntSLT, left, right, CString::new("lt").unwrap().as_ptr())),
                        ">" => Ok(LLVMBuildICmp(self.builder, llvm_sys::LLVMIntPredicate::LLVMIntSGT, left, right, CString::new("gt").unwrap().as_ptr())),
                        _ => Err(format!("Unknown infix operator: {}", infix.operator)),
                    }
                }
                _ => Err("Not implemented".to_string()),
            }
        }
    }
}

impl Drop for Compiler {
    fn drop(&mut self) {
        unsafe {
            LLVMDisposeBuilder(self.builder);
            LLVMDisposeModule(self.module);
            LLVMContextDispose(self.context);
        }
    }
}


#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;
    use crate::parser::Parser;

    fn run_compiler_test(input: &str, expected: &str) {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        let mut compiler = Compiler::new();
        let result = compiler.compile(program).unwrap();

        assert!(result.contains(expected));
    }

    #[test]
    fn test_integer_and_boolean_literals() {
        run_compiler_test("5;", "i64 5");
        run_compiler_test("true;", "i1 true");
        run_compiler_test("false;", "i1 false");
    }

    #[test]
    fn test_prefix_expressions() {
        run_compiler_test("!true;", "i1 false");
        run_compiler_test("!false;", "i1 true");
        run_compiler_test("-15;", "i64 -15");
    }

    #[test]
    fn test_infix_expressions() {
        run_compiler_test("1 + 2;", "i64 3");
        run_compiler_test("1 - 2;", "i64 -1");
        run_compiler_test("2 * 3;", "i64 6");
        run_compiler_test("4 / 2;", "i64 2");
        run_compiler_test("true == true;", "i1 true");
        run_compiler_test("false == false;", "i1 true");
        run_compiler_test("true != false;", "i1 true");
        run_compiler_test("1 < 2;", "i1 true");
        run_compiler_test("1 > 2;", "i1 false");
    }
}
