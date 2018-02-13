; ModuleID = '/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.pro.bc'
source_filename = "/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.id.bc"

@global_arg = common global i32 0, align 4
@global_arg.1 = global [10 x i32] zeroinitializer, align 16

declare void @MarkTaintAddr(i8*, i32)

declare void @RemoveTaintAddr(i8*, i32)

declare void @MarkTaint(i32)

declare void @RemoveTaint(i32)

declare void @printTaint(i32)

declare void @PropagateTaintRegToReg(i32, i32)

declare void @PropagateTaintRegToAddr(i32, i8*, i32)

declare void @PropagateTaintAddrToAddr(i8*, i32, i8*, i32)

declare void @PropagateTaintAddrToReg(i8*, i32, i32)
