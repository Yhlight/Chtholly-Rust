use inkwell::builder::{Builder, BuilderError};
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{BasicValueEnum, FunctionValue, PointerValue};
use inkwell::{FloatPredicate, IntPredicate};
use std::collections::HashMap;
use thiserror::Error;

use crate::ast::{ASTNode, BinaryOperator, Type};

#[derive(Debug, Clone, PartialEq)]
enum OwnershipState {
    Owned,
    Moved,
    ImmutableBorrow { count: usize },
    MutableBorrow,
}

struct OwnershipTracker {
    states: HashMap<String, OwnershipState>,
}

impl OwnershipTracker {
    fn new() -> Self {
        OwnershipTracker {
            states: HashMap::new(),
        }
    }

    fn declare_variable(&mut self, name: &str) {
        self.states
            .insert(name.to_string(), OwnershipState::Owned);
    }

    fn move_variable(&mut self, name: &str) {
        self.states
            .insert(name.to_string(), OwnershipState::Moved);
    }

    fn get_state(&self, name: &str) -> Option<&OwnershipState> {
        self.states.get(name)
    }

    fn borrow_variable(&mut self, name: &str, is_mutable: bool) -> CompileResult<()> {
        let state = self
            .states
            .get_mut(name)
            .ok_or_else(|| CompileError::UndefinedVariable(name.to_string()))?;

        match state {
            OwnershipState::Moved => return Err(CompileError::VariableMoved(name.to_string())),
            OwnershipState::MutableBorrow => {
                return Err(CompileError::CannotBorrowAsImmutable(name.to_string()))
            }
            OwnershipState::ImmutableBorrow { count } => {
                if is_mutable {
                    return Err(CompileError::CannotBorrowAsMutable(name.to_string()));
                }
                *count += 1;
            }
            OwnershipState::Owned => {
                if is_mutable {
                    *state = OwnershipState::MutableBorrow;
                } else {
                    *state = OwnershipState::ImmutableBorrow { count: 1 };
                }
            }
        }
        Ok(())
    }
}

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
    #[error("Variable '{0}' has been moved and cannot be used")]
    VariableMoved(String),
    #[error("Cannot borrow '{0}' as mutable because it is already borrowed as immutable")]
    CannotBorrowAsMutable(String),
    #[error("Cannot borrow '{0}' as immutable because it is already borrowed as mutable")]
    CannotBorrowAsImmutable(String),
    #[error("Cannot assign to '{0}' because it is borrowed")]
    CannotAssignToBorrowedVariable(String),
    #[error("Cannot use '{0}' because it is mutably borrowed")]
    CannotUseMutablyBorrowedVariable(String),
    #[error("Cannot dereference a non-pointer type")]
    DereferenceNonPointer,
}

type CompileResult<T> = Result<T, CompileError>;

#[derive(Clone)]
pub enum Value<'ctx> {
    Integer(inkwell::values::IntValue<'ctx>),
    Float(inkwell::values::FloatValue<'ctx>),
    Bool(inkwell::values::IntValue<'ctx>),
    String(inkwell::values::PointerValue<'ctx>),
    Pointer(inkwell::values::PointerValue<'ctx>, Type),
}

impl<'ctx> Value<'ctx> {
    fn to_basic_value(self) -> BasicValueEnum<'ctx> {
        match self {
            Value::Integer(v) => v.into(),
            Value::Float(v) => v.into(),
            Value::Bool(v) => v.into(),
            Value::String(v) => v.into(),
            Value::Pointer(v, _) => v.into(),
        }
    }

    fn get_type(&self) -> Type {
        match self {
            Value::Integer(_) => Type::I32,
            Value::Float(_) => Type::F64,
            Value::Bool(_) => Type::Bool,
            Value::String(_) => Type::String,
            Value::Pointer(_, ty) => ty.clone(),
        }
    }
}

pub struct Compiler<'ctx> {
    context: &'ctx Context,
    builder: Builder<'ctx>,
    module: Module<'ctx>,
    variables: HashMap<String, (PointerValue<'ctx>, Type, bool)>,
    ownership_tracker: OwnershipTracker,
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
            ownership_tracker: OwnershipTracker::new(),
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
            ASTNode::ForStatement { init, condition, increment, body } => {
                let function = self.builder.get_insert_block().unwrap().get_parent().unwrap();

                if let Some(init) = init {
                    self.compile_statement(init)?;
                }

                let loop_cond_bb = self.context.append_basic_block(function, "loop_cond");
                let loop_body_bb = self.context.append_basic_block(function, "loop_body");
                let loop_inc_bb = self.context.append_basic_block(function, "loop_inc");
                let after_loop_bb = self.context.append_basic_block(function, "after_loop");

                self.builder.build_unconditional_branch(loop_cond_bb)?;
                self.builder.position_at_end(loop_cond_bb);

                if let Some(condition) = condition {
                    let condition_value = self.compile_expression(condition)?;
                    let condition_value = match condition_value {
                        Value::Bool(v) => v,
                        _ => return Err(CompileError::MissingTypeAnnotation),
                    };
                    self.builder.build_conditional_branch(condition_value, loop_body_bb, after_loop_bb)?;
                } else {
                    self.builder.build_unconditional_branch(loop_body_bb)?;
                }

                self.builder.position_at_end(loop_body_bb);
                self.compile_block(body)?;
                self.builder.build_unconditional_branch(loop_inc_bb)?;

                self.builder.position_at_end(loop_inc_bb);
                if let Some(increment) = increment {
                    let _ = self.compile_expression(increment)?;
                }
                self.builder.build_unconditional_branch(loop_cond_bb)?;

                self.builder.position_at_end(after_loop_bb);
            }
            ASTNode::AssignmentExpression { left, right } => {
                self.compile_assignment(left, right)?;
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

                if let Some(value_node) = value {
                    if let ASTNode::Identifier(source_name) = &**value_node {
                        let (_, ty, _) = self
                            .variables
                            .get(source_name)
                            .ok_or_else(|| CompileError::UndefinedVariable(source_name.clone()))?;

                        if !ty.is_copy() {
                            self.ownership_tracker.move_variable(source_name);
                        }
                    }
                }

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
                    self.builder
                        .build_store(alloca, initial_value.to_basic_value())?;
                }

                self.variables
                    .insert(name.clone(), (alloca, ty, *is_mutable));
                self.ownership_tracker.declare_variable(name);
            }
            _ => {}
        }
        Ok(())
    }

    fn compile_expression(&mut self, node: &ASTNode) -> CompileResult<Value<'ctx>> {
        match node {
            ASTNode::IntegerLiteral(value) => Ok(Value::Integer(
                self.context.i32_type().const_int(*value as u64, false),
            )),
            ASTNode::FloatLiteral(value) => {
                Ok(Value::Float(self.context.f64_type().const_float(*value)))
            }
            ASTNode::BoolLiteral(value) => Ok(Value::Bool(
                self.context.bool_type().const_int(*value as u64, false),
            )),
            ASTNode::StringLiteral(value) => {
                let str_ptr = self.builder.build_global_string_ptr(value, ".str")?;
                Ok(Value::String(str_ptr.as_pointer_value()))
            }
            ASTNode::Identifier(name) => {
                let state = self.ownership_tracker.get_state(name).cloned();
                match state {
                    Some(OwnershipState::Moved) => {
                        return Err(CompileError::VariableMoved(name.clone()))
                    }
                    Some(OwnershipState::MutableBorrow) => {
                        return Err(CompileError::CannotUseMutablyBorrowedVariable(
                            name.clone(),
                        ))
                    }
                    _ => {}
                }

                let (ptr, ty, _) = self
                    .variables
                    .get(name)
                    .ok_or(CompileError::UndefinedVariable(name.clone()))?;
                let loaded_value = match ty {
                    Type::I32 => self.builder.build_load(self.context.i32_type(), *ptr, name)?,
                    Type::F64 => self.builder.build_load(self.context.f64_type(), *ptr, name)?,
                    Type::Bool => self.builder.build_load(self.context.bool_type(), *ptr, name)?,
                    Type::String => self.builder.build_load(
                        self.context
                            .ptr_type(inkwell::AddressSpace::default()),
                        *ptr,
                        name,
                    )?,
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
                    _ => todo!(),
                }
            }
            ASTNode::AssignmentExpression { left, right } => self.compile_assignment(left, right),
            ASTNode::Reference {
                expression,
                is_mutable,
            } => {
                if let ASTNode::Identifier(name) = &**expression {
                    self.ownership_tracker
                        .borrow_variable(name, *is_mutable)?;
                    let (ptr, ty, _) = self
                        .variables
                        .get(name)
                        .ok_or_else(|| CompileError::UndefinedVariable(name.clone()))?;
                    Ok(Value::Pointer(*ptr, ty.clone()))
                } else {
                    todo!() // Handle references to complex expressions
                }
            }
            ASTNode::Dereference(expr) => {
                let value = self.compile_expression(expr)?;
                if let Value::Pointer(ptr, ty) = value {
                    let loaded_value = match ty {
                        Type::I32 => self.builder.build_load(self.context.i32_type(), ptr, "deref")?,
                        Type::F64 => self.builder.build_load(self.context.f64_type(), ptr, "deref")?,
                        Type::Bool => self.builder.build_load(self.context.bool_type(), ptr, "deref")?,
                        Type::String => self.builder.build_load(
                            self.context.ptr_type(inkwell::AddressSpace::default()),
                            ptr,
                            "deref",
                        )?,
                    };
                    match ty {
                        Type::I32 => Ok(Value::Integer(loaded_value.into_int_value())),
                        Type::F64 => Ok(Value::Float(loaded_value.into_float_value())),
                        Type::Bool => Ok(Value::Bool(loaded_value.into_int_value())),
                        Type::String => Ok(Value::String(loaded_value.into_pointer_value())),
                    }
                } else {
                    Err(CompileError::DereferenceNonPointer)
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

    fn compile_assignment(
        &mut self,
        name: &ASTNode,
        value_node: &ASTNode,
    ) -> CompileResult<Value<'ctx>> {
        let value = self.compile_expression(value_node)?;

        match name {
            ASTNode::Identifier(name) => {
                if let Some(OwnershipState::ImmutableBorrow { .. })
                | Some(OwnershipState::MutableBorrow) = self.ownership_tracker.get_state(name)
                {
                    return Err(CompileError::CannotAssignToBorrowedVariable(
                        name.to_string(),
                    ));
                }

                let (ptr, is_mutable) = {
                    let (ptr_val, _, is_mut) = self
                        .variables
                        .get(name)
                        .ok_or_else(|| CompileError::UndefinedVariable(name.to_string()))?;
                    (*ptr_val, *is_mut)
                };

                if !is_mutable {
                    return Err(CompileError::CannotAssignToImmutableVariable(
                        name.to_string(),
                    ));
                }

                if let ASTNode::Identifier(source_name) = value_node {
                    let (_, ty, _) = self
                        .variables
                        .get(source_name)
                        .ok_or_else(|| CompileError::UndefinedVariable(source_name.clone()))?;

                    if !ty.is_copy() {
                        self.ownership_tracker.move_variable(source_name);
                    }
                }

                self.builder
                    .build_store(ptr, value.clone().to_basic_value())?;
                Ok(value)
            }
            ASTNode::Dereference(expr) => {
                let ptr_value = self.compile_expression(expr)?;
                if let Value::Pointer(ptr, _) = ptr_value {
                    self.builder
                        .build_store(ptr, value.clone().to_basic_value())?;
                    Ok(value)
                } else {
                    Err(CompileError::DereferenceNonPointer)
                }
            }
            _ => todo!(),
        }
    }

    pub fn to_string(&self) -> String {
        self.module.to_string()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::Parser;

    fn compile_code(code: &str) -> Result<OwnershipTracker, CompileError> {
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();
        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        compiler.compile(&ast)?;
        Ok(compiler.ownership_tracker)
    }

    #[test]
    fn test_move_semantics() {
        let code = r#"
            fn main() {
                let s1: string = "hello";
                let s2: string = s1;
            }
        "#;
        let ownership_tracker = compile_code(code).unwrap();
        assert_eq!(
            ownership_tracker.get_state("s1"),
            Some(&OwnershipState::Moved)
        );
        assert_eq!(
            ownership_tracker.get_state("s2"),
            Some(&OwnershipState::Owned)
        );
    }

    #[test]
    fn test_use_after_move() {
        let code = r#"
            fn main() {
                let s1: string = "hello";
                let s2: string = s1;
                let s3: string = s1;
            }
        "#;
        let result = compile_code(code);
        assert!(matches!(result, Err(CompileError::VariableMoved(_))));
    }

    #[test]
    fn test_copy_semantics() {
        let code = r#"
            fn main() {
                let x: i32 = 5;
                let y: i32 = x;
            }
        "#;
        let ownership_tracker = compile_code(code).unwrap();
        assert_eq!(
            ownership_tracker.get_state("x"),
            Some(&OwnershipState::Owned)
        );
        assert_eq!(
            ownership_tracker.get_state("y"),
            Some(&OwnershipState::Owned)
        );
    }

    #[test]
    fn test_immutable_borrow() {
        let code = r#"
            fn main() {
                let s: string = "hello";
                let r1 = &s;
                let r2 = &s;
            }
        "#;
        let ownership_tracker = compile_code(code).unwrap();
        assert_eq!(
            ownership_tracker.get_state("s"),
            Some(&OwnershipState::ImmutableBorrow { count: 2 })
        );
    }

    #[test]
    fn test_mutable_borrow() {
        let code = r#"
            fn main() {
                let mut s: string = "hello";
                let r1 = &mut s;
            }
        "#;
        let ownership_tracker = compile_code(code).unwrap();
        assert_eq!(
            ownership_tracker.get_state("s"),
            Some(&OwnershipState::MutableBorrow)
        );
    }

    #[test]
    fn test_cannot_borrow_as_mutable_when_immutable_borrow_exists() {
        let code = r#"
            fn main() {
                let mut s: string = "hello";
                let r1 = &s;
                let r2 = &mut s;
            }
        "#;
        let result = compile_code(code);
        assert!(matches!(
            result,
            Err(CompileError::CannotBorrowAsMutable(_))
        ));
    }

    #[test]
    fn test_cannot_borrow_as_immutable_when_mutable_borrow_exists() {
        let code = r#"
            fn main() {
                let mut s: string = "hello";
                let r1 = &mut s;
                let r2 = &s;
            }
        "#;
        let result = compile_code(code);
        assert!(matches!(
            result,
            Err(CompileError::CannotBorrowAsImmutable(_))
        ));
    }
}
