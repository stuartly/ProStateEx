#ifndef _H_SONGLH_LOOPIDENTIFIER
#define _H_SONGLH_LOOPIDENTIFIER

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
#include "Common/Constant.h"
#include "Common/InstructionCommon.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Analysis/CallGraph.h"


using namespace llvm;
using namespace std;

struct TaintFunArg {
    string funName;
    vector<int> argList;

    bool operator<(const TaintFunArg &other) const { return funName < other.funName; }
};


struct DynTaintAnalyzer : public ModulePass {

    static char ID;

    set<TaintFunArg> funArgListSet;

    DynTaintAnalyzer();

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;


    virtual bool runOnModule(Module &M);

    void ImportConfig();

    int printSrcCodeInfo(Instruction *pInst);

    void InitForInstument();

    void Init_Instrument_Type();

    void Init_Instrument_Var();

    void Init_Instrument_Function();


    void DoInstrument();


    int TaintSeed(Function *F);

    int TaintPropagate(Function *F);


    int TaintSeed(Instruction *Inst);

    int TaintPropagate(Instruction *Inst);

    int PrintTaintInst(Instruction *Inst);

    int insertMarkTaintAddr(Value *addr, Instruction *Inst);

    int insertRemoveTaintAddr(Value *va, Instruction *location);

    int insertPropagateTaintAddrToAddr(Value *memSrcValue, Value *memDestValue, Instruction *location);

    int insertPropagateTaintRegToReg(Instruction *src, Instruction *dest, Instruction *location);

    int insertPropagateTaintRegToAddr(Instruction *src, Value *memDestValue, Instruction *location);

    int insertPropagateTaintAddrToReg(Value *memSrcValue, Instruction *dest, Instruction *location);

    int insertMarkTaint(Instruction *Inst, Instruction *location);

    int insertRemoveTaint(Instruction *Inst, Instruction *location);

    int insertPropagateTaint(Value *src, Value *dest, Instruction *location);


    Value *obtainDestMemAddrValue(Value *va);

    set<Value *> obtainSrcMemAddrValueSet(Value *va);


    /* Type */
    Module *pModule;
    IntegerType *IntType;
    IntegerType *LongType;
    Type *VoidType;
    ArrayType *arrayType;
    PointerType *VoidPointerType;
    /* ********** */


    /* Var */
    GlobalVariable *gvar_int32_global_arg;
    GlobalVariable *gvar_array_global_arg;

    /* Function */

    Function *func_MarkTaintAddr;
    Function *func_RemoveTaintAddr;

    Function *func_MarkTaint;
    Function *func_RemoveTaint;

    Function *func_printTaint;

    Function *func_PropagateTaintRegToReg;
    Function *func_PropagateTaintRegToAddr;
    Function *func_PropagateTaintAddrToAddr;
    Function *func_PropagateTaintAddrToReg;


};


#endif
