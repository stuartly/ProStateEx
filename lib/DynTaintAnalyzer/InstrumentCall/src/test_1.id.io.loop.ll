; ModuleID = '/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.id.io.loop.bc'
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
  %0 = load i32, i32* @global, align 4, !dbg !14, !func_id !15, !bb_id !15, !ins_id !15
  ret i32 %0, !dbg !16, !bb_id !15, !ins_id !17
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @foo2(i32 %b) #0 !dbg !18 {
entry:
  %b.addr = alloca i32, align 4, !func_id !17, !bb_id !17, !ins_id !21
  %a = alloca i32, align 4, !bb_id !17, !ins_id !22
  store i32 %b, i32* %b.addr, align 4, !bb_id !17, !ins_id !23
  call void @llvm.dbg.declare(metadata i32* %b.addr, metadata !24, metadata !25), !dbg !26, !bb_id !17, !ins_id !27
  call void @llvm.dbg.declare(metadata i32* %a, metadata !28, metadata !25), !dbg !29, !bb_id !17, !ins_id !30
  %0 = load i32, i32* %b.addr, align 4, !dbg !31, !bb_id !17, !ins_id !32
  %add = add nsw i32 %0, 3, !dbg !33, !bb_id !17, !ins_id !34
  store i32 %add, i32* %a, align 4, !dbg !29, !bb_id !17, !ins_id !35
  %1 = load i32, i32* %a, align 4, !dbg !36, !bb_id !17, !ins_id !37
  ret i32 %1, !dbg !38, !bb_id !17, !ins_id !39
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 !dbg !40 {
entry:
  %retval = alloca i32, align 4, !func_id !21, !bb_id !21, !ins_id !41
  %input = alloca i32, align 4, !bb_id !21, !ins_id !42
  %T14 = alloca i32, align 4, !bb_id !21, !ins_id !43
  %T15 = alloca i32, align 4, !bb_id !21, !ins_id !44
  store i32 0, i32* %retval, align 4, !bb_id !21, !ins_id !45
  call void @llvm.dbg.declare(metadata i32* %input, metadata !46, metadata !25), !dbg !47, !bb_id !21, !ins_id !48
  store i32 0, i32* %input, align 4, !dbg !47, !bb_id !21, !ins_id !49
  %call = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* %input), !dbg !50, !bb_id !21, !ins_id !51
  %0 = load i32, i32* %input, align 4, !dbg !52, !bb_id !21, !ins_id !53
  store i32 %0, i32* @global, align 4, !dbg !54, !bb_id !21, !ins_id !55
  call void @llvm.dbg.declare(metadata i32* %T14, metadata !56, metadata !25), !dbg !57, !bb_id !21, !ins_id !58
  %call1 = call i32 @foo(), !dbg !59, !bb_id !21, !ins_id !60
  store i32 %call1, i32* %T14, align 4, !dbg !57, !bb_id !21, !ins_id !61
  call void @llvm.dbg.declare(metadata i32* %T15, metadata !62, metadata !25), !dbg !63, !bb_id !21, !ins_id !64
  %1 = load i32, i32* %input, align 4, !dbg !65, !bb_id !21, !ins_id !66
  %call2 = call i32 @foo2(i32 %1), !dbg !67, !bb_id !21, !ins_id !68
  store i32 %call2, i32* %T15, align 4, !dbg !63, !bb_id !21, !ins_id !69
  %2 = load i32, i32* %T15, align 4, !dbg !70, !bb_id !21, !ins_id !71
  %call3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i32 0, i32 0), i32 %2), !dbg !72, !bb_id !21, !ins_id !73
  %call4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.2, i32 0, i32 0)), !dbg !74, !bb_id !21, !ins_id !75
  %3 = load i32, i32* %input, align 4, !dbg !76, !bb_id !21, !ins_id !77
  ret i32 %3, !dbg !78, !bb_id !21, !ins_id !79
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
!15 = !{i32 0}
!16 = !DILocation(line: 19, column: 5, scope: !11)
!17 = !{i32 1}
!18 = distinct !DISubprogram(name: "foo2", scope: !3, file: !3, line: 22, type: !19, isLocal: false, isDefinition: true, scopeLine: 22, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!19 = !DISubroutineType(types: !20)
!20 = !{!6, !6}
!21 = !{i32 2}
!22 = !{i32 3}
!23 = !{i32 4}
!24 = !DILocalVariable(name: "b", arg: 1, scope: !18, file: !3, line: 22, type: !6)
!25 = !DIExpression()
!26 = !DILocation(line: 22, column: 14, scope: !18)
!27 = !{i32 5}
!28 = !DILocalVariable(name: "a", scope: !18, file: !3, line: 24, type: !6)
!29 = !DILocation(line: 24, column: 9, scope: !18)
!30 = !{i32 6}
!31 = !DILocation(line: 24, column: 11, scope: !18)
!32 = !{i32 7}
!33 = !DILocation(line: 24, column: 12, scope: !18)
!34 = !{i32 8}
!35 = !{i32 9}
!36 = !DILocation(line: 29, column: 12, scope: !18)
!37 = !{i32 10}
!38 = !DILocation(line: 29, column: 5, scope: !18)
!39 = !{i32 11}
!40 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 34, type: !12, isLocal: false, isDefinition: true, scopeLine: 35, isOptimized: false, unit: !2, variables: !4)
!41 = !{i32 12}
!42 = !{i32 13}
!43 = !{i32 14}
!44 = !{i32 15}
!45 = !{i32 16}
!46 = !DILocalVariable(name: "input", scope: !40, file: !3, line: 36, type: !6)
!47 = !DILocation(line: 36, column: 9, scope: !40)
!48 = !{i32 17}
!49 = !{i32 18}
!50 = !DILocation(line: 37, column: 5, scope: !40)
!51 = !{i32 19}
!52 = !DILocation(line: 64, column: 12, scope: !40)
!53 = !{i32 20}
!54 = !DILocation(line: 64, column: 11, scope: !40)
!55 = !{i32 21}
!56 = !DILocalVariable(name: "T14", scope: !40, file: !3, line: 82, type: !6)
!57 = !DILocation(line: 82, column: 9, scope: !40)
!58 = !{i32 22}
!59 = !DILocation(line: 82, column: 13, scope: !40)
!60 = !{i32 23}
!61 = !{i32 24}
!62 = !DILocalVariable(name: "T15", scope: !40, file: !3, line: 84, type: !6)
!63 = !DILocation(line: 84, column: 9, scope: !40)
!64 = !{i32 25}
!65 = !DILocation(line: 84, column: 18, scope: !40)
!66 = !{i32 26}
!67 = !DILocation(line: 84, column: 13, scope: !40)
!68 = !{i32 27}
!69 = !{i32 28}
!70 = !DILocation(line: 90, column: 20, scope: !40)
!71 = !{i32 29}
!72 = !DILocation(line: 90, column: 5, scope: !40)
!73 = !{i32 30}
!74 = !DILocation(line: 92, column: 5, scope: !40)
!75 = !{i32 31}
!76 = !DILocation(line: 95, column: 12, scope: !40)
!77 = !{i32 32}
!78 = !DILocation(line: 95, column: 5, scope: !40)
!79 = !{i32 33}
