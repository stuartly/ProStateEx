; ModuleID = '/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.opt.bc'
source_filename = "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@global = global i32 0, align 4, !dbg !0
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @foo() #0 !dbg !11 {
entry:
  %a = alloca i32, align 4
  call void @llvm.dbg.declare(metadata i32* %a, metadata !14, metadata !15), !dbg !16
  %0 = load i32, i32* @global, align 4, !dbg !17
  store i32 %0, i32* %a, align 4, !dbg !16
  %1 = load i32, i32* %a, align 4, !dbg !18
  ret i32 %1, !dbg !19
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @foo2(i32 %a, i32 %b, i32 %c) #0 !dbg !20 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %c.addr = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %a.addr, metadata !23, metadata !15), !dbg !24
  store i32 %b, i32* %b.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %b.addr, metadata !25, metadata !15), !dbg !26
  store i32 %c, i32* %c.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %c.addr, metadata !27, metadata !15), !dbg !28
  call void @llvm.dbg.declare(metadata i32* %d, metadata !29, metadata !15), !dbg !30
  %0 = load i32, i32* %a.addr, align 4, !dbg !31
  store i32 %0, i32* %d, align 4, !dbg !30
  %1 = load i32, i32* %d, align 4, !dbg !32
  ret i32 %1, !dbg !33
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 !dbg !34 {
entry:
  %retval = alloca i32, align 4
  %input = alloca i32, align 4
  %one = alloca i32, align 4
  %two = alloca i32, align 4
  %three = alloca i32, align 4
  %T11 = alloca i32, align 4
  %T12 = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @llvm.dbg.declare(metadata i32* %input, metadata !35, metadata !15), !dbg !36
  store i32 0, i32* %input, align 4, !dbg !36
  %call = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* %input), !dbg !37
  call void @llvm.dbg.declare(metadata i32* %one, metadata !38, metadata !15), !dbg !39
  store i32 3, i32* %one, align 4, !dbg !39
  call void @llvm.dbg.declare(metadata i32* %two, metadata !40, metadata !15), !dbg !41
  %0 = load i32, i32* %input, align 4, !dbg !42
  store i32 %0, i32* %two, align 4, !dbg !41
  call void @llvm.dbg.declare(metadata i32* %three, metadata !43, metadata !15), !dbg !44
  %1 = load i32, i32* %two, align 4, !dbg !45
  %2 = load i32, i32* %one, align 4, !dbg !46
  %add = add nsw i32 %1, %2, !dbg !47
  store i32 %add, i32* %three, align 4, !dbg !44
  call void @llvm.dbg.declare(metadata i32* %T11, metadata !48, metadata !15), !dbg !49
  %3 = bitcast i32* %T11 to i8*, !dbg !50
  %4 = bitcast i32* %input to i8*, !dbg !51
  %call1 = call i8* @strcpy(i8* %3, i8* %4) #4, !dbg !52
  call void @llvm.dbg.declare(metadata i32* %T12, metadata !53, metadata !15), !dbg !54
  %5 = load i32, i32* %T11, align 4, !dbg !55
  store i32 %5, i32* %T12, align 4, !dbg !54
  ret i32 0, !dbg !56
}

declare i32 @__isoc99_scanf(i8*, ...) #2

; Function Attrs: nounwind
declare i8* @strcpy(i8*, i8*) #3

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!7, !8, !9}
!llvm.ident = !{!10}

!0 = !DIGlobalVariableExpression(var: !1)
!1 = distinct !DIGlobalVariable(name: "global", scope: !2, file: !3, line: 18, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 5.0.0 (tags/RELEASE_500/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5)
!3 = !DIFile(filename: "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.c", directory: "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src")
!4 = !{}
!5 = !{!0}
!6 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!7 = !{i32 2, !"Dwarf Version", i32 4}
!8 = !{i32 2, !"Debug Info Version", i32 3}
!9 = !{i32 1, !"wchar_size", i32 4}
!10 = !{!"clang version 5.0.0 (tags/RELEASE_500/final)"}
!11 = distinct !DISubprogram(name: "foo", scope: !3, file: !3, line: 26, type: !12, isLocal: false, isDefinition: true, scopeLine: 26, isOptimized: false, unit: !2, variables: !4)
!12 = !DISubroutineType(types: !13)
!13 = !{!6}
!14 = !DILocalVariable(name: "a", scope: !11, file: !3, line: 28, type: !6)
!15 = !DIExpression()
!16 = !DILocation(line: 28, column: 9, scope: !11)
!17 = !DILocation(line: 28, column: 13, scope: !11)
!18 = !DILocation(line: 30, column: 12, scope: !11)
!19 = !DILocation(line: 30, column: 5, scope: !11)
!20 = distinct !DISubprogram(name: "foo2", scope: !3, file: !3, line: 33, type: !21, isLocal: false, isDefinition: true, scopeLine: 33, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!21 = !DISubroutineType(types: !22)
!22 = !{!6, !6, !6, !6}
!23 = !DILocalVariable(name: "a", arg: 1, scope: !20, file: !3, line: 33, type: !6)
!24 = !DILocation(line: 33, column: 14, scope: !20)
!25 = !DILocalVariable(name: "b", arg: 2, scope: !20, file: !3, line: 33, type: !6)
!26 = !DILocation(line: 33, column: 21, scope: !20)
!27 = !DILocalVariable(name: "c", arg: 3, scope: !20, file: !3, line: 33, type: !6)
!28 = !DILocation(line: 33, column: 28, scope: !20)
!29 = !DILocalVariable(name: "d", scope: !20, file: !3, line: 35, type: !6)
!30 = !DILocation(line: 35, column: 9, scope: !20)
!31 = !DILocation(line: 35, column: 13, scope: !20)
!32 = !DILocation(line: 36, column: 12, scope: !20)
!33 = !DILocation(line: 36, column: 5, scope: !20)
!34 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 41, type: !12, isLocal: false, isDefinition: true, scopeLine: 41, isOptimized: false, unit: !2, variables: !4)
!35 = !DILocalVariable(name: "input", scope: !34, file: !3, line: 42, type: !6)
!36 = !DILocation(line: 42, column: 9, scope: !34)
!37 = !DILocation(line: 44, column: 5, scope: !34)
!38 = !DILocalVariable(name: "one", scope: !34, file: !3, line: 48, type: !6)
!39 = !DILocation(line: 48, column: 9, scope: !34)
!40 = !DILocalVariable(name: "two", scope: !34, file: !3, line: 50, type: !6)
!41 = !DILocation(line: 50, column: 9, scope: !34)
!42 = !DILocation(line: 50, column: 15, scope: !34)
!43 = !DILocalVariable(name: "three", scope: !34, file: !3, line: 52, type: !6)
!44 = !DILocation(line: 52, column: 9, scope: !34)
!45 = !DILocation(line: 52, column: 17, scope: !34)
!46 = !DILocation(line: 52, column: 23, scope: !34)
!47 = !DILocation(line: 52, column: 21, scope: !34)
!48 = !DILocalVariable(name: "T11", scope: !34, file: !3, line: 80, type: !6)
!49 = !DILocation(line: 80, column: 9, scope: !34)
!50 = !DILocation(line: 81, column: 12, scope: !34)
!51 = !DILocation(line: 81, column: 18, scope: !34)
!52 = !DILocation(line: 81, column: 5, scope: !34)
!53 = !DILocalVariable(name: "T12", scope: !34, file: !3, line: 83, type: !6)
!54 = !DILocation(line: 83, column: 9, scope: !34)
!55 = !DILocation(line: 83, column: 13, scope: !34)
!56 = !DILocation(line: 103, column: 5, scope: !34)
