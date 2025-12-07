use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use crate::parser::ast::Program;

pub struct CodeGen<'ctx> {
    context: &'ctx Context,
    builder: Builder<'ctx>,
    module: Module<'ctx>,
}

impl<'ctx> CodeGen<'ctx> {
    pub fn new(context: &'ctx Context) -> Self {
        let builder = context.create_builder();
        let module = context.create_module("chtholly");
        CodeGen {
            context,
            builder,
            module,
        }
    }

    pub fn compile(&self, _program: Program) {
        let i32_type = self.context.i32_type();
        let fn_type = i32_type.fn_type(&[], false);
        let function = self.module.add_function("main", fn_type, None);
        let basic_block = self.context.append_basic_block(function, "entry");
        self.builder.position_at_end(basic_block);
        self.builder.build_return(Some(&i32_type.const_int(0, false)));

        self.module.print_to_stderr();
    }
}
