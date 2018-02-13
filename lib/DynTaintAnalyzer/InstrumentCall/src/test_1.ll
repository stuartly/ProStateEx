; ModuleID = '/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.bc'
source_filename = "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @_Z3fooi(i32 %a) #0 !dbg !7 {
entry:
  %a.addr = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %a.addr, metadata !11, metadata !12), !dbg !13
  call void @llvm.dbg.declare(metadata i32* %b, metadata !14, metadata !12), !dbg !15
  %0 = load i32, i32* %a.addr, align 4, !dbg !16
  %add = add nsw i32 %0, 3, !dbg !17
  store i32 %add, i32* %b, align 4, !dbg !15
  %1 = load i32, i32* %b, align 4, !dbg !18
  ret i32 %1, !dbg !19
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main() #2 !dbg !20 {
entry:
  %retval = alloca i32, align 4
  %input = alloca i32, align 4
  %a = alloca i32, align 4
  %second = alloca i32, align 4
  %T3 = alloca i32*, align 8
  %third = alloca i32, align 4
  %four = alloca i32, align 4
  %five = alloca i32, align 4
  %T6 = alloca i32, align 4
  %T7 = alloca i32, align 4
  %T8 = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @llvm.dbg.declare(metadata i32* %input, metadata !23, metadata !12), !dbg !24
  %call = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* %input), !dbg !25
  call void @llvm.dbg.declare(metadata i32* %a, metadata !26, metadata !12), !dbg !27
  store i32 3, i32* %a, align 4, !dbg !27
  call void @llvm.dbg.declare(metadata i32* %second, metadata !28, metadata !12), !dbg !29
  %0 = load i32, i32* %input, align 4, !dbg !30
  %add = add nsw i32 %0, 2, !dbg !31
  store i32 %add, i32* %second, align 4, !dbg !29
  call void @llvm.dbg.declare(metadata i32** %T3, metadata !32, metadata !12), !dbg !34
  store i32* %input, i32** %T3, align 8, !dbg !34
  call void @llvm.dbg.declare(metadata i32* %third, metadata !35, metadata !12), !dbg !36
  %1 = load i32, i32* %second, align 4, !dbg !37
  %2 = load i32, i32* %a, align 4, !dbg !38
  %add1 = add nsw i32 %1, %2, !dbg !39
  store i32 %add1, i32* %third, align 4, !dbg !36
  call void @llvm.dbg.declare(metadata i32* %four, metadata !40, metadata !12), !dbg !41
  %3 = load i32, i32* %third, align 4, !dbg !42
  %mul = mul nsw i32 %3, 4, !dbg !43
  store i32 %mul, i32* %four, align 4, !dbg !41
  call void @llvm.dbg.declare(metadata i32* %five, metadata !44, metadata !12), !dbg !45
  %4 = load i32, i32* %four, align 4, !dbg !46
  %call2 = call i32 @_Z3fooi(i32 %4), !dbg !47
  store i32 %call2, i32* %five, align 4, !dbg !45
  %5 = load i32, i32* %second, align 4, !dbg !48
  %cmp = icmp sgt i32 %5, 5, !dbg !50
  br i1 %cmp, label %if.then, label %if.end, !dbg !51

if.then:                                          ; preds = %entry
  store i32 6, i32* %a, align 4, !dbg !52
  br label %if.end, !dbg !54

if.end:                                           ; preds = %if.then, %entry
  call void @llvm.dbg.declare(metadata i32* %T6, metadata !55, metadata !12), !dbg !56
  %6 = load i32, i32* %a, align 4, !dbg !57
  store i32 %6, i32* %T6, align 4, !dbg !56
  call void @llvm.dbg.declare(metadata i32* %T7, metadata !58, metadata !12), !dbg !59
  %7 = load i32, i32* %a, align 4, !dbg !60
  %tobool = icmp ne i32 %7, 0, !dbg !60
  br i1 %tobool, label %lor.end, label %lor.lhs.false, !dbg !61

lor.lhs.false:                                    ; preds = %if.end
  %8 = load i32, i32* %second, align 4, !dbg !62
  %tobool3 = icmp ne i32 %8, 0, !dbg !62
  br i1 %tobool3, label %lor.end, label %lor.lhs.false4, !dbg !63

lor.lhs.false4:                                   ; preds = %lor.lhs.false
  %9 = load i32, i32* %four, align 4, !dbg !64
  %tobool5 = icmp ne i32 %9, 0, !dbg !64
  br i1 %tobool5, label %lor.end, label %lor.rhs, !dbg !65

lor.rhs:                                          ; preds = %lor.lhs.false4
  %10 = load i32, i32* %five, align 4, !dbg !66
  %tobool6 = icmp ne i32 %10, 0, !dbg !66
  br label %lor.end, !dbg !65

lor.end:                                          ; preds = %lor.rhs, %lor.lhs.false4, %lor.lhs.false, %if.end
  %11 = phi i1 [ true, %lor.lhs.false4 ], [ true, %lor.lhs.false ], [ true, %if.end ], [ %tobool6, %lor.rhs ]
  %conv = zext i1 %11 to i32, !dbg !60
  store i32 %conv, i32* %T7, align 4, !dbg !59
  call void @llvm.dbg.declare(metadata i32* %T8, metadata !67, metadata !12), !dbg !68
  %12 = load i32, i32* %a, align 4, !dbg !69
  %tobool7 = icmp ne i32 %12, 0, !dbg !69
  br i1 %tobool7, label %land.rhs, label %land.end, !dbg !70

land.rhs:                                         ; preds = %lor.end
  %13 = load i32, i32* %second, align 4, !dbg !71
  %tobool8 = icmp ne i32 %13, 0, !dbg !71
  br label %land.end

land.end:                                         ; preds = %land.rhs, %lor.end
  %14 = phi i1 [ false, %lor.end ], [ %tobool8, %land.rhs ]
  %conv9 = zext i1 %14 to i32, !dbg !69
  store i32 %conv9, i32* %T8, align 4, !dbg !68
  %15 = load i32, i32* %four, align 4, !dbg !72
  %call10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i32 0, i32 0), i32 %15), !dbg !73
  ret i32 0, !dbg !74
}

declare i32 @scanf(i8*, ...) #3

declare i32 @printf(i8*, ...) #3

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5}
!llvm.ident = !{!6}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "clang version 5.0.0 (tags/RELEASE_500/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.cpp", directory: "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{!"clang version 5.0.0 (tags/RELEASE_500/final)"}
!7 = distinct !DISubprogram(name: "foo", linkageName: "_Z3fooi", scope: !1, file: !1, line: 9, type: !8, isLocal: false, isDefinition: true, scopeLine: 9, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!8 = !DISubroutineType(types: !9)
!9 = !{!10, !10}
!10 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!11 = !DILocalVariable(name: "a", arg: 1, scope: !7, file: !1, line: 9, type: !10)
!12 = !DIExpression()
!13 = !DILocation(line: 9, column: 13, scope: !7)
!14 = !DILocalVariable(name: "b", scope: !7, file: !1, line: 11, type: !10)
!15 = !DILocation(line: 11, column: 9, scope: !7)
!16 = !DILocation(line: 11, column: 11, scope: !7)
!17 = !DILocation(line: 11, column: 12, scope: !7)
!18 = !DILocation(line: 13, column: 12, scope: !7)
!19 = !DILocation(line: 13, column: 5, scope: !7)
!20 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 16, type: !21, isLocal: false, isDefinition: true, scopeLine: 17, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!21 = !DISubroutineType(types: !22)
!22 = !{!10}
!23 = !DILocalVariable(name: "input", scope: !20, file: !1, line: 18, type: !10)
!24 = !DILocation(line: 18, column: 9, scope: !20)
!25 = !DILocation(line: 19, column: 5, scope: !20)
!26 = !DILocalVariable(name: "a", scope: !20, file: !1, line: 21, type: !10)
!27 = !DILocation(line: 21, column: 9, scope: !20)
!28 = !DILocalVariable(name: "second", scope: !20, file: !1, line: 23, type: !10)
!29 = !DILocation(line: 23, column: 9, scope: !20)
!30 = !DILocation(line: 23, column: 16, scope: !20)
!31 = !DILocation(line: 23, column: 22, scope: !20)
!32 = !DILocalVariable(name: "T3", scope: !20, file: !1, line: 25, type: !33)
!33 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !10, size: 64)
!34 = !DILocation(line: 25, column: 10, scope: !20)
!35 = !DILocalVariable(name: "third", scope: !20, file: !1, line: 27, type: !10)
!36 = !DILocation(line: 27, column: 9, scope: !20)
!37 = !DILocation(line: 27, column: 15, scope: !20)
!38 = !DILocation(line: 27, column: 24, scope: !20)
!39 = !DILocation(line: 27, column: 22, scope: !20)
!40 = !DILocalVariable(name: "four", scope: !20, file: !1, line: 29, type: !10)
!41 = !DILocation(line: 29, column: 9, scope: !20)
!42 = !DILocation(line: 29, column: 15, scope: !20)
!43 = !DILocation(line: 29, column: 20, scope: !20)
!44 = !DILocalVariable(name: "five", scope: !20, file: !1, line: 31, type: !10)
!45 = !DILocation(line: 31, column: 9, scope: !20)
!46 = !DILocation(line: 31, column: 18, scope: !20)
!47 = !DILocation(line: 31, column: 14, scope: !20)
!48 = !DILocation(line: 33, column: 8, scope: !49)
!49 = distinct !DILexicalBlock(scope: !20, file: !1, line: 33, column: 8)
!50 = !DILocation(line: 33, column: 15, scope: !49)
!51 = !DILocation(line: 33, column: 8, scope: !20)
!52 = !DILocation(line: 34, column: 10, scope: !53)
!53 = distinct !DILexicalBlock(scope: !49, file: !1, line: 33, column: 19)
!54 = !DILocation(line: 35, column: 5, scope: !53)
!55 = !DILocalVariable(name: "T6", scope: !20, file: !1, line: 37, type: !10)
!56 = !DILocation(line: 37, column: 9, scope: !20)
!57 = !DILocation(line: 37, column: 12, scope: !20)
!58 = !DILocalVariable(name: "T7", scope: !20, file: !1, line: 39, type: !10)
!59 = !DILocation(line: 39, column: 9, scope: !20)
!60 = !DILocation(line: 39, column: 13, scope: !20)
!61 = !DILocation(line: 39, column: 15, scope: !20)
!62 = !DILocation(line: 39, column: 18, scope: !20)
!63 = !DILocation(line: 39, column: 25, scope: !20)
!64 = !DILocation(line: 39, column: 28, scope: !20)
!65 = !DILocation(line: 39, column: 33, scope: !20)
!66 = !DILocation(line: 39, column: 36, scope: !20)
!67 = !DILocalVariable(name: "T8", scope: !20, file: !1, line: 41, type: !10)
!68 = !DILocation(line: 41, column: 9, scope: !20)
!69 = !DILocation(line: 41, column: 14, scope: !20)
!70 = !DILocation(line: 41, column: 16, scope: !20)
!71 = !DILocation(line: 41, column: 19, scope: !20)
!72 = !DILocation(line: 43, column: 20, scope: !20)
!73 = !DILocation(line: 43, column: 5, scope: !20)
!74 = !DILocation(line: 45, column: 5, scope: !20)
