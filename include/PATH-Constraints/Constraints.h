#ifndef _H_SONGLH_CONSTRAINTS
#define _H_SONGLH_CONSTRAINTS

#include "llvm/Pass.h"
#include "llvm/IR/DerivedTypes.h"

using namespace llvm;

struct Constraints : public ModulePass {
    static char ID;

    Constraints();

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;

    virtual bool runOnModule(Module &M);

    virtual void print(raw_ostream &O, const Module *M) const;

    bool declareMakeSymbolic(Module *M, Value *S, Instruction *Inst,
                             unsigned AllocSize, std::string Label, bool IsStruct);

    void callMakeSymbolic(Module *M, Value *S, BasicBlock *BB, const std::string Label, unsigned size = 0);

    void callMakeSymbolic(Module *M, Value *S, Instruction *Inst, const std::string Label, unsigned size = 0);

    void insertMakeSymbolicInst(Module *M);

    void insertMakeSymbolicInst(Module *M, Function *TargetFunction);

    void MakeSymbolicAllocInst(Module *M);

    void MakeSymbolicAllocInst(Module *M, Function *TargetFunction);

    void MakeSymbolicCallInst(Module *M);

    void MakeSymbolicCallInst(Module *M, Function *TargetFunction);

    void insertMakeSymbolicFunction(Module *M, Function *AfterFunc);

    void MakeSymbolics(Module &M, const std::vector<int> &BlockIds);

};


#endif