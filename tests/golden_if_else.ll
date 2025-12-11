; ModuleID = 'ChthollyJIT'
source_filename = "ChthollyJIT"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 10, ptr %x, align 4
  %x1 = load i32, ptr %x, align 4
  %cmptmp = icmp sgt i32 %x1, 5
  br i1 %cmptmp, label %then, label %else

then:                                             ; preds = %entry
  ret i32 1

else:                                             ; preds = %entry
  ret i32 0

ifcont:                                           ; No predecessors!
}
