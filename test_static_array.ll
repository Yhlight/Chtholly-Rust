; ModuleID = 'ChthollyModule'
source_filename = "ChthollyModule"

declare i32 @printf(ptr, ...)

declare ptr @malloc(i64)

declare ptr @strcpy(ptr, ptr)

declare void @free(ptr)

define i32 @main() {
entry:
  %arraylit = alloca [3 x i32], align 4
  %0 = getelementptr [3 x i32], ptr %arraylit, i32 0, i32 0
  store i32 1, ptr %0, align 4
  %1 = getelementptr [3 x i32], ptr %arraylit, i32 0, i32 1
  store i32 2, ptr %1, align 4
  %2 = getelementptr [3 x i32], ptr %arraylit, i32 0, i32 2
  store i32 3, ptr %2, align 4
  %a = alloca [3 x i32], align 4
  call void @llvm.memcpy.p0.p0.i64(ptr %a, ptr %arraylit, i64 12, i1 false)
  %3 = getelementptr [3 x i32], ptr %a, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  %b = alloca i32, align 4
  store i32 %4, ptr %b, align 4
  %5 = getelementptr [3 x i32], ptr %a, i32 0, i32 1
  %6 = load i32, ptr %5, align 4
  %c = alloca i32, align 4
  store i32 %6, ptr %c, align 4
  %7 = getelementptr [3 x i32], ptr %a, i32 0, i32 2
  %8 = load i32, ptr %7, align 4
  %d = alloca i32, align 4
  store i32 %8, ptr %d, align 4
  %b1 = load i32, ptr %b, align 4
  %c2 = load i32, ptr %c, align 4
  %addtmp = add i32 %b1, %c2
  %d3 = load i32, ptr %d, align 4
  %addtmp4 = add i32 %addtmp, %d3
  ret i32 %addtmp4
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
