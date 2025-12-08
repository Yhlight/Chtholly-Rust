use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{BasicValueEnum, PointerValue};
use inkwell::types::BasicTypeEnum;
use inkwell::IntPredicate;
use inkwell::FloatPredicate;
use std::collections::HashMap;

use crate::compiler::ast::{BinaryOp, Expr, Literal, Stmt};

/// A compiler that generates LLVM IR from a Chtholly AST.
pub struct Compiler<'ctx> {
    pub context: &'ctx Context,
    pub builder: Builder<'ctx>,
    pub module: Module<'ctx>,
    variables: HashMap<String, (PointerValue<'ctx>, BasicTypeEnum<'ctx>, bool)>, // ptr, type, is_mutable
}

impl<'ctx> Compiler<'ctx> {
    /// Creates a new `Compiler` instance.
    pub fn new(context: &'ctx Context) -> Self {
        let builder = context.create_builder();
        let module = context.create_module("chtholly_module");
        Self {
            context,
            builder,
            module,
            variables: HashMap::new(),
        }
    }

    /// Compiles a `Stmt` into LLVM IR.
    pub fn compile_stmt(&mut self, stmt: &Stmt) -> Result<(), &'static str> {
        match stmt {
            Stmt::Let(name, is_mutable, expr) => {
                let value = self.compile_expr(expr)?;
                let ty = value.get_type();
                let ptr = self.builder.build_alloca(ty, name).map_err(|_| "Failed to allocate variable")?;
                self.builder.build_store(ptr, value).map_err(|_| "Failed to store variable")?;
                self.variables.insert(name.clone(), (ptr, ty, *is_mutable));
                Ok(())
            }
            Stmt::Expr(expr) => {
                self.compile_expr(expr)?;
                Ok(())
            }
            Stmt::If(cond, then_block, else_block) => {
                self.compile_if(cond, then_block, else_block.as_deref())
            }
        }
    }

    fn compile_block(&mut self, stmts: &[Stmt]) -> Result<(), &'static str> {
        for stmt in stmts {
            self.compile_stmt(stmt)?;
        }
        Ok(())
    }

    fn compile_if(
        &mut self,
        cond: &Expr,
        then_block: &[Stmt],
        else_block: Option<&[Stmt]>,
    ) -> Result<(), &'static str> {
        let function = self.builder.get_insert_block().unwrap().get_parent().unwrap();

        let cond = self.compile_expr(cond)?.into_int_value();
        let then_bb = self.context.append_basic_block(function, "then");
        let else_bb = self.context.append_basic_block(function, "else");
        let merge_bb = self.context.append_basic_block(function, "merge");

        self.builder
            .build_conditional_branch(cond, then_bb, else_bb)
            .map_err(|_| "Failed to build conditional branch")?;

        self.builder.position_at_end(then_bb);
        self.compile_block(then_block)?;
        self.builder.build_unconditional_branch(merge_bb).map_err(|_| "Failed to build unconditional branch")?;

        self.builder.position_at_end(else_bb);
        if let Some(else_block) = else_block {
            self.compile_block(else_block)?;
        }
        self.builder.build_unconditional_branch(merge_bb).map_err(|_| "Failed to build unconditional branch")?;

        self.builder.position_at_end(merge_bb);
        Ok(())
    }
    /// Compiles an `Expr` into an LLVM value.
    pub fn compile_expr(&self, expr: &Expr) -> Result<BasicValueEnum<'ctx>, &'static str> {
        match expr {
            Expr::Literal(literal) => self.compile_literal(literal),
            Expr::Ident(name) => {
                match self.variables.get(name) {
                    Some((ptr, ty, _)) => {
                        self.builder.build_load(*ty, *ptr, name).map_err(|_| "Failed to build load")
                    },
                    None => Err("Undefined variable"),
                }
            }
            Expr::Assignment(name, expr) => {
                match self.variables.get(name) {
                    Some((ptr, _, is_mutable)) => {
                        if !is_mutable {
                            return Err("Cannot assign to immutable variable");
                        }
                        let value = self.compile_expr(expr)?;
                        self.builder.build_store(*ptr, value).map_err(|_| "Failed to store variable")?;
                        Ok(value)
                    },
                    None => Err("Undefined variable"),
                }
            }
            Expr::Binary(lhs, op, rhs) => self.compile_binary(lhs, op, rhs),
        }
    }

    /// Compiles a `Literal` into an LLVM value.
    fn compile_literal(&self, literal: &Literal) -> Result<BasicValueEnum<'ctx>, &'static str> {
        match literal {
            Literal::Int(int) => Ok(self.context.i64_type().const_int(*int as u64, false).into()),
            Literal::Float(float) => Ok(self.context.f64_type().const_float(*float).into()),
            Literal::String(s) => self.builder.build_global_string_ptr(s, "str").map(|v| v.as_pointer_value().into()).map_err(|_| "Failed to build global string ptr"),
            Literal::Char(c) => Ok(self.context.i8_type().const_int(*c as u64, false).into()),
            Literal::Bool(b) => Ok(self.context.bool_type().const_int(*b as u64, false).into()),
        }
    }

    /// Compiles a binary expression into an LLVM value.
    fn compile_binary(
        &self,
        lhs: &Expr,
        op: &BinaryOp,
        rhs: &Expr,
    ) -> Result<BasicValueEnum<'ctx>, &'static str> {
        if *op == BinaryOp::And || *op == BinaryOp::Or {
            let lhs = self.compile_expr(lhs)?;
            if !lhs.is_int_value() || lhs.into_int_value().get_type().get_bit_width() != 1 {
                return Err("Logical operators can only be used with booleans");
            }

            let rhs = self.compile_expr(rhs)?;
            if !rhs.is_int_value() || rhs.into_int_value().get_type().get_bit_width() != 1 {
                return Err("Logical operators can only be used with booleans");
            }

            return match op {
                BinaryOp::And => self.builder
                    .build_and(lhs.into_int_value(), rhs.into_int_value(), "andtmp")
                    .map(|v| v.into())
                    .map_err(|_| "Failed to build and"),
                BinaryOp::Or => self.builder
                    .build_or(lhs.into_int_value(), rhs.into_int_value(), "ortmp")
                    .map(|v| v.into())
                    .map_err(|_| "Failed to build or"),
                _ => unreachable!(),
            };
        }

        let lhs = self.compile_expr(lhs)?;
        let rhs = self.compile_expr(rhs)?;

        if lhs.get_type() != rhs.get_type() {
            return Err("Type mismatch in binary operation");
        }

        match op {
            BinaryOp::Add => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_add(lhs.into_int_value(), rhs.into_int_value(), "addtmp")
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int add")
                } else {
                    self.builder
                        .build_float_add(lhs.into_float_value(), rhs.into_float_value(), "addtmp")
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float add")
                }
            }
            BinaryOp::Sub => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_sub(lhs.into_int_value(), rhs.into_int_value(), "subtmp")
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int sub")
                } else {
                    self.builder
                        .build_float_sub(lhs.into_float_value(), rhs.into_float_value(), "subtmp")
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float sub")
                }
            }
            BinaryOp::Mul => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_mul(lhs.into_int_value(), rhs.into_int_value(), "multmp")
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int mul")
                } else {
                    self.builder
                        .build_float_mul(lhs.into_float_value(), rhs.into_float_value(), "multmp")
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float mul")
                }
            }
            BinaryOp::Div => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_signed_div(lhs.into_int_value(), rhs.into_int_value(), "divtmp")
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int div")
                } else {
                    self.builder
                        .build_float_div(lhs.into_float_value(), rhs.into_float_value(), "divtmp")
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float div")
                }
            }
            BinaryOp::Mod => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_signed_rem(lhs.into_int_value(), rhs.into_int_value(), "remtmp")
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int rem")
                } else {
                    Err("Cannot perform modulo operation on floats")
                }
            }
            BinaryOp::Eq => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_compare(
                            IntPredicate::EQ,
                            lhs.into_int_value(),
                            rhs.into_int_value(),
                            "eqtmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int compare")
                } else {
                    self.builder
                        .build_float_compare(
                            FloatPredicate::OEQ,
                            lhs.into_float_value(),
                            rhs.into_float_value(),
                            "eqtmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float compare")
                }
            }
            BinaryOp::NotEq => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_compare(
                            IntPredicate::NE,
                            lhs.into_int_value(),
                            rhs.into_int_value(),
                            "neqtmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int compare")
                } else {
                    self.builder
                        .build_float_compare(
                            FloatPredicate::ONE,
                            lhs.into_float_value(),
                            rhs.into_float_value(),
                            "neqtmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float compare")
                }
            }
            BinaryOp::Lt => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_compare(
                            IntPredicate::SLT,
                            lhs.into_int_value(),
                            rhs.into_int_value(),
                            "lttmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int compare")
                } else {
                    self.builder
                        .build_float_compare(
                            FloatPredicate::OLT,
                            lhs.into_float_value(),
                            rhs.into_float_value(),
                            "lttmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float compare")
                }
            }
            BinaryOp::LtEq => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_compare(
                            IntPredicate::SLE,
                            lhs.into_int_value(),
                            rhs.into_int_value(),
                            "leqtmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int compare")
                } else {
                    self.builder
                        .build_float_compare(
                            FloatPredicate::OLE,
                            lhs.into_float_value(),
                            rhs.into_float_value(),
                            "leqtmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float compare")
                }
            }
            BinaryOp::Gt => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_compare(
                            IntPredicate::SGT,
                            lhs.into_int_value(),
                            rhs.into_int_value(),
                            "gttmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int compare")
                } else {
                    self.builder
                        .build_float_compare(
                            FloatPredicate::OGT,
                            lhs.into_float_value(),
                            rhs.into_float_value(),
                            "gttmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float compare")
                }
            }
            BinaryOp::GtEq => {
                if lhs.is_int_value() {
                    self.builder
                        .build_int_compare(
                            IntPredicate::SGE,
                            lhs.into_int_value(),
                            rhs.into_int_value(),
                            "geqtmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build int compare")
                } else {
                    self.builder
                        .build_float_compare(
                            FloatPredicate::OGE,
                            lhs.into_float_value(),
                            rhs.into_float_value(),
                            "geqtmp",
                        )
                        .map(|v| v.into())
                        .map_err(|_| "Failed to build float compare")
                }
            }
            BinaryOp::And | BinaryOp::Or => unreachable!(),
        }
    }
}
