use inkwell::context::Context;
use inkwell::module::Module;
use std::error::Error;

pub struct CodeGenerator<'ctx> {
    context: &'ctx Context,
}

impl<'ctx> CodeGenerator<'ctx> {
    pub fn new(context: &'ctx Context) -> Self {
        CodeGenerator { context }
    }

    /// Creates a module with a main function that returns 0.
    /// This function simulates the code generation for a hardcoded AST
    /// representing `fn main(): i32 { return 0; }`.
    pub fn compile_hardcoded_main(&self) -> Result<Module<'ctx>, Box<dyn Error>> {
        let module = self.context.create_module("main");
        let builder = self.context.create_builder();

        // Define the function signature: `fn main() -> i32`
        let i32_type = self.context.i32_type();
        let main_fn_type = i32_type.fn_type(&[], false);
        let main_fn = module.add_function("main", main_fn_type, None);

        // Create the entry basic block
        let entry_bb = self.context.append_basic_block(main_fn, "entry");
        builder.position_at_end(entry_bb);

        // Build the return statement: `return 0;`
        let return_value = i32_type.const_int(0, false);
        builder.build_return(Some(&return_value)).unwrap();

        // Verify the function's correctness
        if main_fn.verify(true) {
            Ok(module)
        } else {
            // It's good practice to dump the IR on verification failure
            // to help with debugging.
            module.print_to_stderr();
            Err("Failed to verify main function".into())
        }
    }
}
