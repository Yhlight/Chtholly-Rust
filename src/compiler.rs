use inkwell::builder::{Builder, BuilderError};
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{FunctionValue, PointerValue, BasicValueEnum};
use inkwell::{IntPredicate, FloatPredicate};
use std::collections::HashMap;
use thiserror::Error;
use crate::ast::{ASTNode, Type, BinaryOperator};

#[derive(Debug, Error)]
pub enum CompileError {
    #[error("Builder error: {0}")]
    BuilderError(#[from] BuilderError),
    #[error("Missing type annotation")]
    MissingTypeAnnotation,
    #[error("Undefined variable: {0}")]
    UndefinedVariable(String),
    #[error("Cannot assign to immutable variable: {0}")]
    CannotAssignToImmutableVariable(String),
}

type CompileResult<T> = Result<T, CompileError>;

pub enum Value<'ctx> {
    Integer(inkwell::values::IntValue<'ctx>),
    Float(inkwell::values::FloatValue<'ctx>),
    Bool(inkwell::values::IntValue<'ctx>),
    String(inkwell::values::PointerValue<'ctx>),
}

impl<'ctx> Value<'ctx> {
    fn to_basic_value(self) -> BasicValueEnum<'ctx> {
        match self {
            Value::Integer(v) => v.into(),
            Value::Float(v) => v.into(),
            Value::Bool(v) => v.into(),
            Value::String(v) => v.into(),
        }
    }

    fn get_type(&self) -> Type {
        match self {
            Value::Integer(_) => Type::I32,
            Value::Float(_) => Type::F64,
            Value::Bool(_) => Type::Bool,
            Value::String(_) => Type::String,
        }
    }
}

pub struct Compiler<'ctx> {
    context: &'ctx Context,
    builder: Builder<'ctx>,
    module: Module<'ctx>,
    variables: HashMap<String, (PointerValue<'ctx>, Type, bool)>,
}

impl<'ctx> Compiler<'ctx> {
    pub fn new(context: &'ctx Context) -> Self {
        let builder = context.create_builder();
        let module = context.create_module("chtholly");
        Compiler {
            context,
            builder,
            module,
            variables: HashMap::new(),
        }
    }

    pub fn compile(&mut self, ast: &[ASTNode]) -> CompileResult<()> {
        for node in ast {
            self.compile_node(node)?;
        }
        Ok(())
    }

    fn compile_node(&mut self, node: &ASTNode) -> CompileResult<()> {
        match node {
            ASTNode::Function { name, body, .. } => {
                let _function = self.compile_function(name, body)?;
            }
            _ => {}
        }
        Ok(())
    }

    fn compile_function(&mut self, name: &str, body: &[ASTNode]) -> CompileResult<FunctionValue<'ctx>> {
        let i32_type = self.context.i32_type();
        let fn_type = i32_type.fn_type(&[], false);
        let function = self.module.add_function(name, fn_type, None);
        let basic_block = self.context.append_basic_block(function, "entry");

        self.builder.position_at_end(basic_block);

        for node in body {
            self.compile_statement(node)?;
        }

        self.builder.build_return(Some(&i32_type.const_int(0, false)))?;

        Ok(function)
    }

    fn compile_statement(&mut self, node: &ASTNode) -> CompileResult<()> {
        match node {
            ASTNode::IfStatement { condition, then_block, else_block } => {
                let condition_value = self.compile_expression(condition)?;
                let condition_value = match condition_value {
                    Value::Bool(v) => v,
                    _ => return Err(CompileError::MissingTypeAnnotation), // TODO: better error
                };

                let function = self.builder.get_insert_block().unwrap().get_parent().unwrap();

                let then_bb = self.context.append_basic_block(function, "then");
                let else_bb = self.context.append_basic_block(function, "else");
                let merge_bb = self.context.append_basic_block(function, "merge");

                self.builder.build_conditional_branch(condition_value, then_bb, else_bb)?;

                self.builder.position_at_end(then_bb);
                self.compile_block(then_block)?;
                if self.builder.get_insert_block().unwrap().get_terminator().is_none() {
                    self.builder.build_unconditional_branch(merge_bb)?;
                }

                self.builder.position_at_end(else_bb);
                if let Some(else_block) = else_block {
                    self.compile_block(else_block)?;
                }
                if self.builder.get_insert_block().unwrap().get_terminator().is_none() {
                    self.builder.build_unconditional_branch(merge_bb)?;
                }

                self.builder.position_at_end(merge_bb);
            }
            ASTNode::WhileStatement { condition, body } => {
                let function = self.builder.get_insert_block().unwrap().get_parent().unwrap();

                let loop_cond_bb = self.context.append_basic_block(function, "loop_cond");
                let loop_body_bb = self.context.append_basic_block(function, "loop_body");
                let after_loop_bb = self.context.append_basic_block(function, "after_loop");

                self.builder.build_unconditional_branch(loop_cond_bb)?;
                self.builder.position_at_end(loop_cond_bb);

                let condition_value = self.compile_expression(condition)?;
                let condition_value = match condition_value {
                    Value::Bool(v) => v,
                    _ => return Err(CompileError::MissingTypeAnnotation), // TODO: better error
                };

                self.builder.build_conditional_branch(condition_value, loop_body_bb, after_loop_bb)?;

                self.builder.position_at_end(loop_body_bb);
                self.compile_block(body)?;
                self.builder.build_unconditional_branch(loop_cond_bb)?;

                self.builder.position_at_end(after_loop_bb);
            }
            ASTNode::AssignmentExpression { name, value } => {
                let (ptr, _, is_mutable) = self.variables.get(name).ok_or(CompileError::UndefinedVariable(name.clone()))?;
                if !is_mutable {
                    return Err(CompileError::CannotAssignToImmutableVariable(name.clone()));
                }

                let value = self.compile_expression(value)?;
                self.builder.build_store(*ptr, value.to_basic_value())?;
            }
            ASTNode::VariableDeclaration {
                name,
                type_annotation,
                value,
                is_mutable,
            } => {
                let initial_value = value
                    .as_ref()
                    .map(|v| self.compile_expression(v))
                    .transpose()?;

                let ty = match (type_annotation, &initial_value) {
                    (Some(ty), _) => ty.clone(),
                    (None, Some(val)) => val.get_type(),
                    (None, None) => return Err(CompileError::MissingTypeAnnotation),
                };

                let alloca = match ty {
                    Type::I32 => self.builder.build_alloca(self.context.i32_type(), name)?,
                    Type::F64 => self.builder.build_alloca(self.context.f64_type(), name)?,
                    Type::Bool => self.builder.build_alloca(self.context.bool_type(), name)?,
                    Type::String => self.builder.build_alloca(self.context.ptr_type(inkwell::AddressSpace::default()), name)?,
                };

                if let Some(initial_value) = initial_value {
                    self.builder.build_store(alloca, initial_value.to_basic_value())?;
                }

                self.variables.insert(name.clone(), (alloca, ty, *is_mutable));
            }
            _ => {}
        }
        Ok(())
    }

    fn compile_expression(&self, node: &ASTNode) -> CompileResult<Value<'ctx>> {
        match node {
            ASTNode::IntegerLiteral(value) => Ok(Value::Integer(self.context.i32_type().const_int(*value as u64, false))),
            ASTNode::FloatLiteral(value) => Ok(Value::Float(self.context.f64_type().const_float(*value))),
            ASTNode::BoolLiteral(value) => Ok(Value::Bool(self.context.bool_type().const_int(*value as u64, false))),
            ASTNode::StringLiteral(value) => {
                let str_ptr = self.builder.build_global_string_ptr(value, ".str")?;
                Ok(Value::String(str_ptr.as_pointer_value()))
            }
            ASTNode::Identifier(name) => {
                let (ptr, ty, _) = self.variables.get(name).ok_or(CompileError::UndefinedVariable(name.clone()))?;
                let loaded_value = match ty {
                    Type::I32 => self.builder.build_load(self.context.i32_type(), *ptr, name)?,
                    Type::F64 => self.builder.build_load(self.context.f64_type(), *ptr, name)?,
                    Type::Bool => self.builder.build_load(self.context.bool_type(), *ptr, name)?,
                    Type::String => self.builder.build_load(self.context.ptr_type(inkwell::AddressSpace::default()), *ptr, name)?,
                };

                match ty {
                    Type::I32 => Ok(Value::Integer(loaded_value.into_int_value())),
                    Type::F64 => Ok(Value::Float(loaded_value.into_float_value())),
                    Type::Bool => Ok(Value::Bool(loaded_value.into_int_value())),
                    Type::String => Ok(Value::String(loaded_value.into_pointer_value())),
                }
            }
            ASTNode::BinaryExpression { op, left, right } => {
                let left = self.compile_expression(left)?;
                let right = self.compile_expression(right)?;

                match (left, right) {
                    (Value::Integer(left), Value::Integer(right)) => {
                        match op {
                            BinaryOperator::Add => {
                                let result = self.builder.build_int_add(left, right, "addtmp")?;
                                Ok(Value::Integer(result))
                            }
                            BinaryOperator::Subtract => {
                                let result = self.builder.build_int_sub(left, right, "subtmp")?;
                                Ok(Value::Integer(result))
                            }
                            BinaryOperator::Multiply => {
                                let result = self.builder.build_int_mul(left, right, "multmp")?;
                                Ok(Value::Integer(result))
                            }
                            BinaryOperator::Divide => {
                                let result = self.builder.build_int_signed_div(left, right, "divtmp")?;
                                Ok(Value::Integer(result))
                            }
                            BinaryOperator::Equal => {
                                let result = self.builder.build_int_compare(IntPredicate::EQ, left, right, "eqtmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::NotEqual => {
                                let result = self.builder.build_int_compare(IntPredicate::NE, left, right, "netmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::LessThan => {
                                let result = self.builder.build_int_compare(IntPredicate::SLT, left, right, "lttmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::GreaterThan => {
                                let result = self.builder.build_int_compare(IntPredicate::SGT, left, right, "gttmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::LessThanOrEqual => {
                                let result = self.builder.build_int_compare(IntPredicate::SLE, left, right, "letmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::GreaterThanOrEqual => {
                                let result = self.builder.build_int_compare(IntPredicate::SGE, left, right, "getmp")?;
                                Ok(Value::Bool(result))
                            }
                        }
                    }
                    (Value::Float(left), Value::Float(right)) => {
                        match op {
                            BinaryOperator::Add => {
                                let result = self.builder.build_float_add(left, right, "addtmp")?;
                                Ok(Value::Float(result))
                            }
                            BinaryOperator::Subtract => {
                                let result = self.builder.build_float_sub(left, right, "subtmp")?;
                                Ok(Value::Float(result))
                            }
                            BinaryOperator::Multiply => {
                                let result = self.builder.build_float_mul(left, right, "multmp")?;
                                Ok(Value::Float(result))
                            }
                            BinaryOperator::Divide => {
                                let result = self.builder.build_float_div(left, right, "divtmp")?;
                                Ok(Value::Float(result))
                            }
                            BinaryOperator::Equal => {
                                let result = self.builder.build_float_compare(FloatPredicate::OEQ, left, right, "eqtmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::NotEqual => {
                                let result = self.builder.build_float_compare(FloatPredicate::ONE, left, right, "netmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::LessThan => {
                                let result = self.builder.build_float_compare(FloatPredicate::OLT, left, right, "lttmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::GreaterThan => {
                                let result = self.builder.build_float_compare(FloatPredicate::OGT, left, right, "gttmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::LessThanOrEqual => {
                                let result = self.builder.build_float_compare(FloatPredicate::OLE, left, right, "letmp")?;
                                Ok(Value::Bool(result))
                            }
                            BinaryOperator::GreaterThanOrEqual => {
                                let result = self.builder.build_float_compare(FloatPredicate::OGE, left, right, "getmp")?;
                                Ok(Value::Bool(result))
                            }
                        }
                    }
                    _ => todo!(), // Handle other types
                }
            }
            _ => todo!(),
        }
    }

    fn compile_block(&mut self, block: &[ASTNode]) -> CompileResult<()> {
        for node in block {
            self.compile_statement(node)?;
        }
        Ok(())
    }

    pub fn to_string(&self) -> String {
        self.module.to_string()
    }
}
