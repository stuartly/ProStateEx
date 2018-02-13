#!/usr/bin/env bash

HOME=/home/lzs243/Documents/ProtocolEx
WORK=/home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/src
LIB=/home/lzs243/Documents/ProtocolEx/cmake-build-debug/lib
TEST_PROGRAM=${WORK}/test_1.cpp

BC_FILE=${WORK}/test_1.bc
BC_OPT_FILE=${WORK}/test_1.opt.bc
BC_ID_FILE=${WORK}/test_1.id.bc

O_PROF_FILE=${WORK}/test_1.o
BC_PROF_FILE=${WORK}/test_1.pro.bc



CALNGDIR=/home/lzs243/Documents/binutils-gdb/build/installed/bin/

RUNTIME_LIB=${HOME}/lib/DynTaintAnalyzer/InstrumentCall/cmake-build-debug/libInstrumentCall.a

CLANG=/home/lzs243/Documents/MissingCheck/llvm-3.6/build/bin/clang
CLANGPLUS=/home/lzs243/Documents/MissingCheck/llvm-3.6/build/bin/clang++
LLC=/home/lzs243/Documents/MissingCheck/llvm-3.6/build/bin/llc
OPT=/home/lzs243/Documents/binutils-gdb/build/installed/bin/opt
LLVMDIS=/home/lzs243/Documents/MissingCheck/llvm-3.6/build/bin/llvm-dis


{

 #llvm-3.6
# ${CLANG} -emit-llvm -c -g ${TEST_PROGRAM} -o ${BC_FILE}
#
# ${LLVMDIS} ${BC_FILE}
#
# ${LLC} -march=cpp ${BC_FILE}

 #llvm-5.0
 clang -emit-llvm -c -g ${TEST_PROGRAM} -o ${BC_FILE}

 llvm-dis ${BC_FILE}

 opt -loop-simplify -mem2reg  ${BC_FILE} -o ${BC_OPT_FILE}

 llvm-dis ${BC_OPT_FILE}

 opt -load ${LIB}/IDAssigner/libIDAssignerPass.so -tag-id ${BC_FILE} > ${BC_ID_FILE}

 llvm-dis ${BC_ID_FILE}

 opt -load ${LIB}/DynTaintAnalyzer/libDynTaintAnalyzerPass.so -analysis-taint-var ${BC_ID_FILE}  -strFileName taintSource -NetworkIOPath /home/lzs243/Documents/ProtocolEx/lib/DynTaintAnalyzer/InstrumentCall/config/IO_Fun_Arg  > ${BC_PROF_FILE}

 llvm-dis  ${BC_PROF_FILE}

 llc -filetype=obj ${BC_PROF_FILE} -o ${O_PROF_FILE}

 clang++ ${BC_PROF_FILE} ${RUNTIME_LIB} -lstdc++

 ./a.out
}