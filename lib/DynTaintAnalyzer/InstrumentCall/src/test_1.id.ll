; ModuleID = '/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.id.bc'
source_filename = "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @_Z3fooi(i32 %a) #0 !dbg !7 {
entry:
  %a.addr = alloca i32, align 4, !func_id !11, !bb_id !11, !ins_id !11
  %b = alloca i32, align 4, !bb_id !11, !ins_id !12
  store i32 %a, i32* %a.addr, align 4, !bb_id !11, !ins_id !13
  call void @llvm.dbg.declare(metadata i32* %a.addr, metadata !14, metadata !15), !dbg !16, !bb_id !11, !ins_id !17
  call void @llvm.dbg.declare(metadata i32* %b, metadata !18, metadata !15), !dbg !19, !bb_id !11, !ins_id !20
  %0 = load i32, i32* %a.addr, align 4, !dbg !21, !bb_id !11, !ins_id !22
  %add = add nsw i32 %0, 3, !dbg !23, !bb_id !11, !ins_id !24
  store i32 %add, i32* %b, align 4, !dbg !19, !bb_id !11, !ins_id !25
  %1 = load i32, i32* %b, align 4, !dbg !26, !bb_id !11, !ins_id !27
  ret i32 %1, !dbg !28, !bb_id !11, !ins_id !29
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main() #2 !dbg !30 {
entry:
  %retval = alloca i32, align 4, !func_id !12, !bb_id !12, !ins_id !33
  %input = alloca i32, align 4, !bb_id !12, !ins_id !34
  %a = alloca i32, align 4, !bb_id !12, !ins_id !35
  %second = alloca i32, align 4, !bb_id !12, !ins_id !36
  %T3 = alloca i32*, align 8, !bb_id !12, !ins_id !37
  %third = alloca i32, align 4, !bb_id !12, !ins_id !38
  %four = alloca i32, align 4, !bb_id !12, !ins_id !39
  %five = alloca i32, align 4, !bb_id !12, !ins_id !40
  %T6 = alloca i32, align 4, !bb_id !12, !ins_id !41
  %T7 = alloca i32, align 4, !bb_id !12, !ins_id !42
  %T8 = alloca i32, align 4, !bb_id !12, !ins_id !43
  store i32 0, i32* %retval, align 4, !bb_id !12, !ins_id !44
  call void @llvm.dbg.declare(metadata i32* %input, metadata !45, metadata !15), !dbg !46, !bb_id !12, !ins_id !47
  %call = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* %input), !dbg !48, !bb_id !12, !ins_id !49
  call void @llvm.dbg.declare(metadata i32* %a, metadata !50, metadata !15), !dbg !51, !bb_id !12, !ins_id !52
  store i32 3, i32* %a, align 4, !dbg !51, !bb_id !12, !ins_id !53
  call void @llvm.dbg.declare(metadata i32* %second, metadata !54, metadata !15), !dbg !55, !bb_id !12, !ins_id !56
  %0 = load i32, i32* %input, align 4, !dbg !57, !bb_id !12, !ins_id !58
  %add = add nsw i32 %0, 2, !dbg !59, !bb_id !12, !ins_id !60
  store i32 %add, i32* %second, align 4, !dbg !55, !bb_id !12, !ins_id !61
  call void @llvm.dbg.declare(metadata i32** %T3, metadata !62, metadata !15), !dbg !64, !bb_id !12, !ins_id !65
  store i32* %input, i32** %T3, align 8, !dbg !64, !bb_id !12, !ins_id !66
  call void @llvm.dbg.declare(metadata i32* %third, metadata !67, metadata !15), !dbg !68, !bb_id !12, !ins_id !69
  %1 = load i32, i32* %second, align 4, !dbg !70, !bb_id !12, !ins_id !71
  %2 = load i32, i32* %a, align 4, !dbg !72, !bb_id !12, !ins_id !73
  %add1 = add nsw i32 %1, %2, !dbg !74, !bb_id !12, !ins_id !75
  store i32 %add1, i32* %third, align 4, !dbg !68, !bb_id !12, !ins_id !76
  call void @llvm.dbg.declare(metadata i32* %four, metadata !77, metadata !15), !dbg !78, !bb_id !12, !ins_id !79
  %3 = load i32, i32* %third, align 4, !dbg !80, !bb_id !12, !ins_id !81
  %mul = mul nsw i32 %3, 4, !dbg !82, !bb_id !12, !ins_id !83
  store i32 %mul, i32* %four, align 4, !dbg !78, !bb_id !12, !ins_id !84
  call void @llvm.dbg.declare(metadata i32* %five, metadata !85, metadata !15), !dbg !86, !bb_id !12, !ins_id !87
  %4 = load i32, i32* %four, align 4, !dbg !88, !bb_id !12, !ins_id !89
  %call2 = call i32 @_Z3fooi(i32 %4), !dbg !90, !bb_id !12, !ins_id !91
  store i32 %call2, i32* %five, align 4, !dbg !86, !bb_id !12, !ins_id !92
  %5 = load i32, i32* %second, align 4, !dbg !93, !bb_id !12, !ins_id !95
  %cmp = icmp sgt i32 %5, 5, !dbg !96, !bb_id !12, !ins_id !97
  br i1 %cmp, label %if.then, label %if.end, !dbg !98, !bb_id !12, !ins_id !99

if.then:                                          ; preds = %entry
  store i32 6, i32* %a, align 4, !dbg !100, !bb_id !13, !ins_id !102
  br label %if.end, !dbg !103, !bb_id !13, !ins_id !104

if.end:                                           ; preds = %if.then, %entry
  call void @llvm.dbg.declare(metadata i32* %T6, metadata !105, metadata !15), !dbg !106, !bb_id !17, !ins_id !107
  %6 = load i32, i32* %a, align 4, !dbg !108, !bb_id !17, !ins_id !109
  store i32 %6, i32* %T6, align 4, !dbg !106, !bb_id !17, !ins_id !110
  call void @llvm.dbg.declare(metadata i32* %T7, metadata !111, metadata !15), !dbg !112, !bb_id !17, !ins_id !113
  %7 = load i32, i32* %a, align 4, !dbg !114, !bb_id !17, !ins_id !115
  %tobool = icmp ne i32 %7, 0, !dbg !114, !bb_id !17, !ins_id !116
  br i1 %tobool, label %lor.end, label %lor.lhs.false, !dbg !117, !bb_id !17, !ins_id !118

lor.lhs.false:                                    ; preds = %if.end
  %8 = load i32, i32* %second, align 4, !dbg !119, !bb_id !20, !ins_id !120
  %tobool3 = icmp ne i32 %8, 0, !dbg !119, !bb_id !20, !ins_id !121
  br i1 %tobool3, label %lor.end, label %lor.lhs.false4, !dbg !122, !bb_id !20, !ins_id !123

lor.lhs.false4:                                   ; preds = %lor.lhs.false
  %9 = load i32, i32* %four, align 4, !dbg !124, !bb_id !22, !ins_id !125
  %tobool5 = icmp ne i32 %9, 0, !dbg !124, !bb_id !22, !ins_id !126
  br i1 %tobool5, label %lor.end, label %lor.rhs, !dbg !127, !bb_id !22, !ins_id !128

lor.rhs:                                          ; preds = %lor.lhs.false4
  %10 = load i32, i32* %five, align 4, !dbg !129, !bb_id !24, !ins_id !130
  %tobool6 = icmp ne i32 %10, 0, !dbg !129, !bb_id !24, !ins_id !131
  br label %lor.end, !dbg !127, !bb_id !24, !ins_id !132

lor.end:                                          ; preds = %lor.rhs, %lor.lhs.false4, %lor.lhs.false, %if.end
  %11 = phi i1 [ true, %lor.lhs.false4 ], [ true, %lor.lhs.false ], [ true, %if.end ], [ %tobool6, %lor.rhs ], !bb_id !25, !ins_id !133
  %conv = zext i1 %11 to i32, !dbg !114, !bb_id !25, !ins_id !134
  store i32 %conv, i32* %T7, align 4, !dbg !112, !bb_id !25, !ins_id !135
  call void @llvm.dbg.declare(metadata i32* %T8, metadata !136, metadata !15), !dbg !137, !bb_id !25, !ins_id !138
  %12 = load i32, i32* %a, align 4, !dbg !139, !bb_id !25, !ins_id !140
  %tobool7 = icmp ne i32 %12, 0, !dbg !139, !bb_id !25, !ins_id !141
  br i1 %tobool7, label %land.rhs, label %land.end, !dbg !142, !bb_id !25, !ins_id !143

land.rhs:                                         ; preds = %lor.end
  %13 = load i32, i32* %second, align 4, !dbg !144, !bb_id !27, !ins_id !145
  %tobool8 = icmp ne i32 %13, 0, !dbg !144, !bb_id !27, !ins_id !146
  br label %land.end, !bb_id !27, !ins_id !147

land.end:                                         ; preds = %land.rhs, %lor.end
  %14 = phi i1 [ false, %lor.end ], [ %tobool8, %land.rhs ], !bb_id !29, !ins_id !148
  %conv9 = zext i1 %14 to i32, !dbg !139, !bb_id !29, !ins_id !149
  store i32 %conv9, i32* %T8, align 4, !dbg !137, !bb_id !29, !ins_id !150
  %15 = load i32, i32* %four, align 4, !dbg !151, !bb_id !29, !ins_id !152
  %call10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i32 0, i32 0), i32 %15), !dbg !153, !bb_id !29, !ins_id !154
  ret i32 0, !dbg !155, !bb_id !29, !ins_id !156
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
!11 = !{i32 0}
!12 = !{i32 1}
!13 = !{i32 2}
!14 = !DILocalVariable(name: "a", arg: 1, scope: !7, file: !1, line: 9, type: !10)
!15 = !DIExpression()
!16 = !DILocation(line: 9, column: 13, scope: !7)
!17 = !{i32 3}
!18 = !DILocalVariable(name: "b", scope: !7, file: !1, line: 11, type: !10)
!19 = !DILocation(line: 11, column: 9, scope: !7)
!20 = !{i32 4}
!21 = !DILocation(line: 11, column: 11, scope: !7)
!22 = !{i32 5}
!23 = !DILocation(line: 11, column: 12, scope: !7)
!24 = !{i32 6}
!25 = !{i32 7}
!26 = !DILocation(line: 13, column: 12, scope: !7)
!27 = !{i32 8}
!28 = !DILocation(line: 13, column: 5, scope: !7)
!29 = !{i32 9}
!30 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 16, type: !31, isLocal: false, isDefinition: true, scopeLine: 17, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!31 = !DISubroutineType(types: !32)
!32 = !{!10}
!33 = !{i32 10}
!34 = !{i32 11}
!35 = !{i32 12}
!36 = !{i32 13}
!37 = !{i32 14}
!38 = !{i32 15}
!39 = !{i32 16}
!40 = !{i32 17}
!41 = !{i32 18}
!42 = !{i32 19}
!43 = !{i32 20}
!44 = !{i32 21}
!45 = !DILocalVariable(name: "input", scope: !30, file: !1, line: 18, type: !10)
!46 = !DILocation(line: 18, column: 9, scope: !30)
!47 = !{i32 22}
!48 = !DILocation(line: 19, column: 5, scope: !30)
!49 = !{i32 23}
!50 = !DILocalVariable(name: "a", scope: !30, file: !1, line: 21, type: !10)
!51 = !DILocation(line: 21, column: 9, scope: !30)
!52 = !{i32 24}
!53 = !{i32 25}
!54 = !DILocalVariable(name: "second", scope: !30, file: !1, line: 23, type: !10)
!55 = !DILocation(line: 23, column: 9, scope: !30)
!56 = !{i32 26}
!57 = !DILocation(line: 23, column: 16, scope: !30)
!58 = !{i32 27}
!59 = !DILocation(line: 23, column: 22, scope: !30)
!60 = !{i32 28}
!61 = !{i32 29}
!62 = !DILocalVariable(name: "T3", scope: !30, file: !1, line: 25, type: !63)
!63 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !10, size: 64)
!64 = !DILocation(line: 25, column: 10, scope: !30)
!65 = !{i32 30}
!66 = !{i32 31}
!67 = !DILocalVariable(name: "third", scope: !30, file: !1, line: 27, type: !10)
!68 = !DILocation(line: 27, column: 9, scope: !30)
!69 = !{i32 32}
!70 = !DILocation(line: 27, column: 15, scope: !30)
!71 = !{i32 33}
!72 = !DILocation(line: 27, column: 24, scope: !30)
!73 = !{i32 34}
!74 = !DILocation(line: 27, column: 22, scope: !30)
!75 = !{i32 35}
!76 = !{i32 36}
!77 = !DILocalVariable(name: "four", scope: !30, file: !1, line: 29, type: !10)
!78 = !DILocation(line: 29, column: 9, scope: !30)
!79 = !{i32 37}
!80 = !DILocation(line: 29, column: 15, scope: !30)
!81 = !{i32 38}
!82 = !DILocation(line: 29, column: 20, scope: !30)
!83 = !{i32 39}
!84 = !{i32 40}
!85 = !DILocalVariable(name: "five", scope: !30, file: !1, line: 31, type: !10)
!86 = !DILocation(line: 31, column: 9, scope: !30)
!87 = !{i32 41}
!88 = !DILocation(line: 31, column: 18, scope: !30)
!89 = !{i32 42}
!90 = !DILocation(line: 31, column: 14, scope: !30)
!91 = !{i32 43}
!92 = !{i32 44}
!93 = !DILocation(line: 33, column: 8, scope: !94)
!94 = distinct !DILexicalBlock(scope: !30, file: !1, line: 33, column: 8)
!95 = !{i32 45}
!96 = !DILocation(line: 33, column: 15, scope: !94)
!97 = !{i32 46}
!98 = !DILocation(line: 33, column: 8, scope: !30)
!99 = !{i32 47}
!100 = !DILocation(line: 34, column: 10, scope: !101)
!101 = distinct !DILexicalBlock(scope: !94, file: !1, line: 33, column: 19)
!102 = !{i32 48}
!103 = !DILocation(line: 35, column: 5, scope: !101)
!104 = !{i32 49}
!105 = !DILocalVariable(name: "T6", scope: !30, file: !1, line: 37, type: !10)
!106 = !DILocation(line: 37, column: 9, scope: !30)
!107 = !{i32 50}
!108 = !DILocation(line: 37, column: 12, scope: !30)
!109 = !{i32 51}
!110 = !{i32 52}
!111 = !DILocalVariable(name: "T7", scope: !30, file: !1, line: 39, type: !10)
!112 = !DILocation(line: 39, column: 9, scope: !30)
!113 = !{i32 53}
!114 = !DILocation(line: 39, column: 13, scope: !30)
!115 = !{i32 54}
!116 = !{i32 55}
!117 = !DILocation(line: 39, column: 15, scope: !30)
!118 = !{i32 56}
!119 = !DILocation(line: 39, column: 18, scope: !30)
!120 = !{i32 57}
!121 = !{i32 58}
!122 = !DILocation(line: 39, column: 25, scope: !30)
!123 = !{i32 59}
!124 = !DILocation(line: 39, column: 28, scope: !30)
!125 = !{i32 60}
!126 = !{i32 61}
!127 = !DILocation(line: 39, column: 33, scope: !30)
!128 = !{i32 62}
!129 = !DILocation(line: 39, column: 36, scope: !30)
!130 = !{i32 63}
!131 = !{i32 64}
!132 = !{i32 65}
!133 = !{i32 66}
!134 = !{i32 67}
!135 = !{i32 68}
!136 = !DILocalVariable(name: "T8", scope: !30, file: !1, line: 41, type: !10)
!137 = !DILocation(line: 41, column: 9, scope: !30)
!138 = !{i32 69}
!139 = !DILocation(line: 41, column: 14, scope: !30)
!140 = !{i32 70}
!141 = !{i32 71}
!142 = !DILocation(line: 41, column: 16, scope: !30)
!143 = !{i32 72}
!144 = !DILocation(line: 41, column: 19, scope: !30)
!145 = !{i32 73}
!146 = !{i32 74}
!147 = !{i32 75}
!148 = !{i32 76}
!149 = !{i32 77}
!150 = !{i32 78}
!151 = !DILocation(line: 43, column: 20, scope: !30)
!152 = !{i32 79}
!153 = !DILocation(line: 43, column: 5, scope: !30)
!154 = !{i32 80}
!155 = !DILocation(line: 45, column: 5, scope: !30)
!156 = !{i32 81}
