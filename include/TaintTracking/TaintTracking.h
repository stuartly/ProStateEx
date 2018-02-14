#ifndef _H_SONGLH_LOOPIDENTIFIER
#define _H_SONGLH_LOOPIDENTIFIER

#include <set>
#include <map>
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
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/User.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/ConstantFolding.h"





using namespace llvm;
using namespace std;

//* InstVisitor sub-struct that implements how to handle taint
//* tracking for different types of instructions.
//*/



struct TaintTracking : public ModulePass {


    /* variable */
    static char ID;

    // Map Value* objects (registers) to the memory address that their
    // taint value is stored at.
    std::map<Value *, Value *> RegToTaintVal;

    // Map Value* objects (memory addresses) to the memory address that
    // their taint value is stored at.
    std::map<Value *, Value *> AddrToTaintAddr;

    // Iterator that can be used with the above maps.
    std::map<Value *, Value *>::iterator ValToValIt;

    // A map of return instructions to their taint values.
    std::map<Function *, GlobalVariable *> FunctionToReturnTaintAddr;
    std::map<Function *, GlobalVariable *>::iterator FunctionToGlobalVarIt;

    // A map of functions to a list of their parameters' taint values.
    std::map<Function *, std::vector<GlobalVariable *> *> FunctionToParamTaintAddr;
    std::map<Function *, std::vector<GlobalVariable *> *>::iterator FunctionToGlobalVarVecIt;

    // A map of array indices to the array pointer.
    std::map<Value *, Value *> ArrIdxPtrs;

    // The 1-bit integer type we will use to store taint values.
    IntegerType *TaintIntType;

    // Pointer to the abort basic block.
    BasicBlock *abortBB;

    // Pointer to the pass we are running so we can split BB's.
    Pass *taintPass;

    // A list of the functions actually definde in the source code we are instrumenting.
    std::set<Function *> DefinedFunctions;

    // List of protected functions that will not be called if a parameter is tainted.
    std::set<std::string> ProtectedFunctions;

    // List of functions that will be treated as sources.
    std::set<std::string> SourceFunctions;





    /* function */

    TaintTracking();

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;

    virtual bool runOnModule(Module &M);

    bool isSource(CallInst *call);


    void findDefinedFunctions(Module &M);
    //void createAbortBB(Module &M, Function &F);

    Value* getRegOpTaintVal(Value *regOp);
    Value* getTaintAddrForMemAddr(Value *memAddr);
    Value* getMemOpTaintVal(Value *addrOp, Instruction &I);

    void visitInstruction(Instruction* Inst);
    void visitBranchInst(BranchInst &I);
    void visitCallInst(CallInst &I);
    void visitReturnInst(ReturnInst &I);
    void visitGetElementPtrInst(GetElementPtrInst &I);
    void visitBinaryOperator(BinaryOperator &I);
    void visitCmpInst(CmpInst &I);
    void visitUnaryInstruction(UnaryInstruction &I);
    void visitPHINode(PHINode &I);
    void visitStoreInst(StoreInst &I);
    void visitLoadInst(LoadInst &I);

    void storeRegTaint(Value *reg, Value *taintVal);

    void insertParamTaintStore(CallInst &I);
    void insertReturnTaintLoad(CallInst &I);
    void insertFuncProtector(CallInst &I);
    void insertParamTaintLoads(Function *F);
    void allocFuncParamTaint(Module &M);
    void allocFuncTaintReturn(Module &M);

    void handleReturnFromMain(ReturnInst &I);
    void binaryOp(Instruction &I);


    void createAbortBB(Module &M, Function &F);





};


#endif
