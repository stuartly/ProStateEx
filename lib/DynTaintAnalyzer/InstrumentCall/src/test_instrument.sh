HOME=/home/lzs243/Documents/ProtocolEx
WORK=/home/lzs243/Documents/ProtocolEx/stubs
LIB=/home/lzs243/Documents/ProtocolEx/cmake-build-debug/lib


IO_CONFIG=/home/lzs243/Documents/LibBC/system-io-config.txt
IO_FUN_Arg=/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/config/IO_Fun_Arg


OUT_PUT_FILE=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src

TESTPROGRAM=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.cpp
TEST_PROGRAM2=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src/test_2.c

BC_FILE=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.bc
BC_OPT_FILE=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.opt.bc
BC_ID_FILE=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.id.bc
BC_ID_IO_FILE=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.id.io.bc
BC_ID_IO_LOOP_FILE=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.id.io.loop.bc
BC_ID_IO_LOOP_TAINT_FILE=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.id.io.loop.taint.bc

O_FILE=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/src/test_1.o



#clang3.6
CALNGDIR=/home/lzs243/Documents/binutils-gdb/build/installed/bin/
RUNTIME_LIB=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/cmake-build-debug/libInstrumentCall.a
CLANG=/home/lzs243/Documents/MissingCheck/llvm-3.6/build/bin/clang
CLANGPLUS=/home/lzs243/Documents/MissingCheck/llvm-3.6/build/bin/clang++
LLC=/home/lzs243/Documents/MissingCheck/llvm-3.6/build/bin/llc
OPT=/home/lzs243/Documents/binutils-gdb/build/installed/bin/opt
LLVMDIS=/home/lzs243/Documents/MissingCheck/llvm-3.6/build/bin/llvm-dis



 #llvm-3.6
# ${CLANG} -emit-llvm -c -g ${TEST_PROGRAM2} -o ${BC_FILE}
#
# ${LLVMDIS} ${BC_FILE}
#
# ${LLC} -march=cpp ${BC_FILE}


#llvm-5.0
 clang -emit-llvm -c -g ${TEST_PROGRAM2} -o ${BC_FILE}

 llvm-dis ${BC_FILE}

 opt  -loop-simplify -mem2reg  ${BC_FILE} -o ${BC_OPT_FILE}

 llvm-dis ${BC_OPT_FILE}

 #add id_Flag

 opt -load ${LIB}/IDAssigner/libIDAssignerPass.so -tag-id ${BC_OPT_FILE} -strFileName id-tag > ${BC_ID_FILE}

 #add IO_Call_Flag

 opt -load ${LIB}/NetworkIO/libNetworkIOPass.so -network-io ${BC_ID_FILE} -SystemIOPath ${IO_CONFIG} -strFileName ${OUT_PUT_FILE}/network-io.txt > ${BC_ID_IO_FILE}

 #add  Loop_IO_Call_Flag
 opt -load ${LIB}/LoopIOFlagAssigner/libLoopIOFlagAssignerPass.so -Loop-IO-Flag-Assigner ${BC_ID_IO_FILE} -strFileName ${OUT_PUT_FILE}/loop-io-assigner.txt > ${BC_ID_IO_LOOP_FILE}

 llvm-dis ${BC_ID_IO_LOOP_FILE}

 #do Instructment
 #opt -load ${LIB}/DynTaintAnalyzer/libDynTaintAnalyzerPass.so -analysis-taint-var ${BC_ID_IO_LOOP_FILE} -NetworkIOPath ${IO_FUN_Arg} -strFileName taintSource > ${BC_ID_IO_LOOP_TAINT_FILE}

 opt -load ${LIB}/TaintTracking/libTaintTrackingPass.so -taint-track ${BC_ID_IO_LOOP_FILE} -strFileName TaintTrack.txt -user-sources > ${BC_ID_IO_LOOP_TAINT_FILE}
 llvm-dis ${BC_ID_IO_LOOP_TAINT_FILE}

 opt -load ${LIB}/LoopIdentifier/libLoopIdentifierPass.so -identify-loop ${BC_ID_IO_LOOP_FILE} -strFileName ${OUT_PUT_FILE}/identify-loop.txt > /dev/null

# opt -load ${LIB}/SideEffect/libSideEffectPass.so -sideeffect-analysis ${BC_ID_IO_LOOP_FILE} -strFuncName _modbus_receive_msg -numLoopHeader 147 > /dev/null
opt -load ${LIB}/StateVarIdentifier/libStateVarIdentifierPass.so -identify-StateVar ${BC_ID_IO_LOOP_FILE} -FunLoopHeader ${OUT_PUT_FILE}/identify-loop.txt -strFileName ${OUT_PUT_FILE}/identify-StateVar.txt > /dev/null



# llc -filetype=obj ${BC_ID_IO_LOOP_TAINT_FILE} -o ${O_FILE}
##
# clang++  ${O_FILE} ${RUNTIME_LIB} -O0 -lstdc++
#
# ./a.out



