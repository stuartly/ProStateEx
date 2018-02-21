//
// Created by stly on 1/16/18.
//


#include <stdio.h>

#include <ctime>
#include <map>
#include <set>
#include <vector>
#include <string.h>
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <ostream>
#include <fstream>

#include "llvm/Pass.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include <boost/algorithm/string.hpp>
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Analysis/CallGraph.h"


using namespace llvm;
using namespace std;


#ifdef __cplusplus
using namespace std;


extern "C" {

void isValue(Value* va);

bool isTaintAddr(void *memory_addr, unsigned int offset);
void MarkTaintAddr(void *memory_addr, unsigned int offset);
void RemoveTaintAddr(void *memory_addr, unsigned int offset);


bool isTaint(int Inst_id);
void MarkTaint(int Inst_id);
void RemoveTaint(int Inst_id);
void PropagateTaintRegToReg(int OpInst, int ResultInst);
void PropagateTaintRegToAddr(int OpInst, void *dest_mem_addr, unsigned int dest_offset);
void PropagateTaintAddrToReg(void *src_mem_addr, unsigned int src_offset, int ResultInst);
void PropagateTaintAddrToAddr(void *src_mem_addr, unsigned int src_offset,
                              void *dest_mem_addr, unsigned int dest_offset);
void printTaint(int Inst_id);

};

#endif
