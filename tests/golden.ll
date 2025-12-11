; ModuleID = 'ChthollyJIT'
source_filename = "ChthollyJIT"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 10, ptr %x, align 4
  %x1 = load i32, ptr %x, align 4
  %addtmp = add i32 %x1, 5
  ret i32 %addtmp
}
