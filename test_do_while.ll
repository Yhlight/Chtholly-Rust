; ModuleID = 'ChthollyModule'
source_filename = "ChthollyModule"

declare i32 @printf(ptr, ...)

declare ptr @malloc(i64)

declare ptr @strcpy(ptr, ptr)

declare void @free(ptr)

define i32 @main() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %loop.body

loop.body:                                        ; preds = %loop.cond, %entry
  %i1 = load i32, ptr %i, align 4
  %addtmp = add i32 %i1, 1
  store i32 %addtmp, ptr %i, align 4
  br label %loop.cond

loop.cond:                                        ; preds = %loop.body
  %i2 = load i32, ptr %i, align 4
  %lttmp = icmp slt i32 %i2, 5
  %dowhilecond = icmp ne i1 %lttmp, false
  br i1 %dowhilecond, label %loop.body, label %loop.exit

loop.exit:                                        ; preds = %loop.cond
  %i3 = load i32, ptr %i, align 4
  ret i32 %i3
}
