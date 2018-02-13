#ifndef PROTOCOLEX_MAKESYMBOLICFORKLEE_H
#define PROTOCOLEX_MAKESYMBOLICFORKLEE_H

#include "llvm/Pass.h"
#include "llvm/IR/DerivedTypes.h"

using namespace llvm;

struct SymbolicHelper : public ModulePass {
    static char ID;

    SymbolicHelper();

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;

    virtual bool runOnModule(Module &M);

    virtual void print(raw_ostream &O, const Module *M) const;

    void callMakeSymbolic(Module *M, Value *S, BasicBlock *BB, const std::string Label, unsigned size = 0);

    void callMakeSymbolic(Module *M, Value *S, Instruction *Inst, const std::string Label, unsigned size = 0);

    void insertMakeSymbolicInst(Module *M, Function *TargetFunction);

    void MakeSymbolicAllocInst(Module *M, Function *TargetFunction);

    void MakeSymbolicCallInst(Module *M, Function *TargetFunction);

    void insertMakeSymbolicFunction(Module *M, Function *AfterFunc);

    void MakeSymbolics(Module &M, const std::vector<int> &BlockIds);

};

#endif //PROTOCOLEX_MAKESYMBOLICFORKLEE_H
