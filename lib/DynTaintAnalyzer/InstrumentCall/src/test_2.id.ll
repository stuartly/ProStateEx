; ModuleID = '/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.id.bc'
source_filename = "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@global = global i32 0, align 4, !dbg !0
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

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
  %one = alloca i32, align 4, !bb_id !21, !ins_id !43
  %second = alloca i32, align 4, !bb_id !21, !ins_id !44
  %T15 = alloca i32, align 4, !bb_id !21, !ins_id !45
  %T16 = alloca i32, align 4, !bb_id !21, !ins_id !46
  store i32 0, i32* %retval, align 4, !bb_id !21, !ins_id !47
  call void @llvm.dbg.declare(metadata i32* %input, metadata !48, metadata !25), !dbg !49, !bb_id !21, !ins_id !50
  store i32 0, i32* %input, align 4, !dbg !49, !bb_id !21, !ins_id !51
  %call = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* %input), !dbg !52, !bb_id !21, !ins_id !53
  call void @llvm.dbg.declare(metadata i32* %one, metadata !54, metadata !25), !dbg !55, !bb_id !21, !ins_id !56
  store i32 3, i32* %one, align 4, !dbg !55, !bb_id !21, !ins_id !57
  call void @llvm.dbg.declare(metadata i32* %second, metadata !58, metadata !25), !dbg !59, !bb_id !21, !ins_id !60
  %0 = load i32, i32* %input, align 4, !dbg !61, !bb_id !21, !ins_id !62
  %sub = sub nsw i32 %0, 2, !dbg !63, !bb_id !21, !ins_id !64
  store i32 %sub, i32* %second, align 4, !dbg !59, !bb_id !21, !ins_id !65
  call void @llvm.dbg.declare(metadata i32* %T15, metadata !66, metadata !25), !dbg !67, !bb_id !21, !ins_id !68
  %1 = load i32, i32* %input, align 4, !dbg !69, !bb_id !21, !ins_id !70
  %call1 = call i32 @foo2(i32 %1), !dbg !71, !bb_id !21, !ins_id !72
  store i32 %call1, i32* %T15, align 4, !dbg !67, !bb_id !21, !ins_id !73
  call void @llvm.dbg.declare(metadata i32* %T16, metadata !74, metadata !25), !dbg !75, !bb_id !21, !ins_id !76
  %2 = load i32, i32* %one, align 4, !dbg !77, !bb_id !21, !ins_id !78
  %tobool = icmp ne i32 %2, 0, !dbg !77, !bb_id !21, !ins_id !79
  br i1 %tobool, label %lor.end, label %lor.rhs, !dbg !80, !bb_id !21, !ins_id !81

lor.rhs:                                          ; preds = %entry
  %3 = load i32, i32* %second, align 4, !dbg !82, !bb_id !22, !ins_id !83
  %tobool2 = icmp ne i32 %3, 0, !dbg !80, !bb_id !22, !ins_id !84
  br label %lor.end, !dbg !80, !bb_id !22, !ins_id !85

lor.end:                                          ; preds = %lor.rhs, %entry
  %4 = phi i1 [ true, %entry ], [ %tobool2, %lor.rhs ], !bb_id !23, !ins_id !86
  %lor.ext = zext i1 %4 to i32, !dbg !80, !bb_id !23, !ins_id !87
  store i32 %lor.ext, i32* %T16, align 4, !dbg !75, !bb_id !23, !ins_id !88
  %5 = load i32, i32* %input, align 4, !dbg !89, !bb_id !23, !ins_id !90
  ret i32 %5, !dbg !91, !bb_id !23, !ins_id !92
}

declare i32 @__isoc99_scanf(i8*, ...) #2

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
!46 = !{i32 17}
!47 = !{i32 18}
!48 = !DILocalVariable(name: "input", scope: !40, file: !3, line: 36, type: !6)
!49 = !DILocation(line: 36, column: 9, scope: !40)
!50 = !{i32 19}
!51 = !{i32 20}
!52 = !DILocation(line: 37, column: 5, scope: !40)
!53 = !{i32 21}
!54 = !DILocalVariable(name: "one", scope: !40, file: !3, line: 39, type: !6)
!55 = !DILocation(line: 39, column: 10, scope: !40)
!56 = !{i32 22}
!57 = !{i32 23}
!58 = !DILocalVariable(name: "second", scope: !40, file: !3, line: 41, type: !6)
!59 = !DILocation(line: 41, column: 10, scope: !40)
!60 = !{i32 24}
!61 = !DILocation(line: 41, column: 17, scope: !40)
!62 = !{i32 25}
!63 = !DILocation(line: 41, column: 23, scope: !40)
!64 = !{i32 26}
!65 = !{i32 27}
!66 = !DILocalVariable(name: "T15", scope: !40, file: !3, line: 78, type: !6)
!67 = !DILocation(line: 78, column: 11, scope: !40)
!68 = !{i32 28}
!69 = !DILocation(line: 78, column: 20, scope: !40)
!70 = !{i32 29}
!71 = !DILocation(line: 78, column: 15, scope: !40)
!72 = !{i32 30}
!73 = !{i32 31}
!74 = !DILocalVariable(name: "T16", scope: !40, file: !3, line: 80, type: !6)
!75 = !DILocation(line: 80, column: 11, scope: !40)
!76 = !{i32 32}
!77 = !DILocation(line: 80, column: 16, scope: !40)
!78 = !{i32 33}
!79 = !{i32 34}
!80 = !DILocation(line: 80, column: 20, scope: !40)
!81 = !{i32 35}
!82 = !DILocation(line: 80, column: 23, scope: !40)
!83 = !{i32 36}
!84 = !{i32 37}
!85 = !{i32 38}
!86 = !{i32 39}
!87 = !{i32 40}
!88 = !{i32 41}
!89 = !DILocation(line: 87, column: 12, scope: !40)
!90 = !{i32 42}
!91 = !DILocation(line: 87, column: 5, scope: !40)
!92 = !{i32 43}
