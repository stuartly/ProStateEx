; ModuleID = '/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.bc'
source_filename = "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@global = global i32 0, align 4, !dbg !0
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.2 = private unnamed_addr constant [13 x i8] c"hello world\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @foo() #0 !dbg !11 {
entry:
  %0 = load i32, i32* @global, align 4, !dbg !14
  ret i32 %0, !dbg !15
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @foo2(i32 %b) #0 !dbg !16 {
entry:
  %b.addr = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 %b, i32* %b.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %b.addr, metadata !19, metadata !20), !dbg !21
  call void @llvm.dbg.declare(metadata i32* %a, metadata !22, metadata !20), !dbg !23
  %0 = load i32, i32* %b.addr, align 4, !dbg !24
  %add = add nsw i32 %0, 3, !dbg !25
  store i32 %add, i32* %a, align 4, !dbg !23
  %1 = load i32, i32* %a, align 4, !dbg !26
  ret i32 %1, !dbg !27
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 !dbg !28 {
entry:
  %retval = alloca i32, align 4
  %input = alloca i32, align 4
  %T14 = alloca i32, align 4
  %T15 = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @llvm.dbg.declare(metadata i32* %input, metadata !29, metadata !20), !dbg !30
  store i32 0, i32* %input, align 4, !dbg !30
  %call = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* %input), !dbg !31
  %0 = load i32, i32* %input, align 4, !dbg !32
  store i32 %0, i32* @global, align 4, !dbg !33
  call void @llvm.dbg.declare(metadata i32* %T14, metadata !34, metadata !20), !dbg !35
  %call1 = call i32 @foo(), !dbg !36
  store i32 %call1, i32* %T14, align 4, !dbg !35
  call void @llvm.dbg.declare(metadata i32* %T15, metadata !37, metadata !20), !dbg !38
  %1 = load i32, i32* %input, align 4, !dbg !39
  %call2 = call i32 @foo2(i32 %1), !dbg !40
  store i32 %call2, i32* %T15, align 4, !dbg !38
  %2 = load i32, i32* %T15, align 4, !dbg !41
  %call3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i32 0, i32 0), i32 %2), !dbg !42
  %call4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.2, i32 0, i32 0)), !dbg !43
  %3 = load i32, i32* %input, align 4, !dbg !44
  ret i32 %3, !dbg !45
}

declare i32 @__isoc99_scanf(i8*, ...) #2

declare i32 @printf(i8*, ...) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!7, !8, !9}
!llvm.ident = !{!10}

!0 = !DIGlobalVariableExpression(var: !1)
!1 = distinct !DIGlobalVariable(name: "global", scope: !2, file: !3, line: 7, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 5.0.0 (tags/RELEASE_500/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5)
!3 = !DIFile(filename: "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.c", directory: "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src")
!4 = !{}
!5 = !{!0}
!6 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!7 = !{i32 2, !"Dwarf Version", i32 4}
!8 = !{i32 2, !"Debug Info Version", i32 3}
!9 = !{i32 1, !"wchar_size", i32 4}
!10 = !{!"clang version 5.0.0 (tags/RELEASE_500/final)"}
!11 = distinct !DISubprogram(name: "foo", scope: !3, file: !3, line: 16, type: !12, isLocal: false, isDefinition: true, scopeLine: 16, isOptimized: false, unit: !2, variables: !4)
!12 = !DISubroutineType(types: !13)
!13 = !{!6}
!14 = !DILocation(line: 19, column: 12, scope: !11)
!15 = !DILocation(line: 19, column: 5, scope: !11)
!16 = distinct !DISubprogram(name: "foo2", scope: !3, file: !3, line: 22, type: !17, isLocal: false, isDefinition: true, scopeLine: 22, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!17 = !DISubroutineType(types: !18)
!18 = !{!6, !6}
!19 = !DILocalVariable(name: "b", arg: 1, scope: !16, file: !3, line: 22, type: !6)
!20 = !DIExpression()
!21 = !DILocation(line: 22, column: 14, scope: !16)
!22 = !DILocalVariable(name: "a", scope: !16, file: !3, line: 24, type: !6)
!23 = !DILocation(line: 24, column: 9, scope: !16)
!24 = !DILocation(line: 24, column: 11, scope: !16)
!25 = !DILocation(line: 24, column: 12, scope: !16)
!26 = !DILocation(line: 29, column: 12, scope: !16)
!27 = !DILocation(line: 29, column: 5, scope: !16)
!28 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 34, type: !12, isLocal: false, isDefinition: true, scopeLine: 35, isOptimized: false, unit: !2, variables: !4)
!29 = !DILocalVariable(name: "input", scope: !28, file: !3, line: 36, type: !6)
!30 = !DILocation(line: 36, column: 9, scope: !28)
!31 = !DILocation(line: 37, column: 5, scope: !28)
!32 = !DILocation(line: 64, column: 12, scope: !28)
!33 = !DILocation(line: 64, column: 11, scope: !28)
!34 = !DILocalVariable(name: "T14", scope: !28, file: !3, line: 82, type: !6)
!35 = !DILocation(line: 82, column: 9, scope: !28)
!36 = !DILocation(line: 82, column: 13, scope: !28)
!37 = !DILocalVariable(name: "T15", scope: !28, file: !3, line: 84, type: !6)
!38 = !DILocation(line: 84, column: 9, scope: !28)
!39 = !DILocation(line: 84, column: 18, scope: !28)
!40 = !DILocation(line: 84, column: 13, scope: !28)
!41 = !DILocation(line: 90, column: 20, scope: !28)
!42 = !DILocation(line: 90, column: 5, scope: !28)
!43 = !DILocation(line: 92, column: 5, scope: !28)
!44 = !DILocation(line: 95, column: 12, scope: !28)
!45 = !DILocation(line: 95, column: 5, scope: !28)
