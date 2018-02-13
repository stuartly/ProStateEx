#ifndef PROTOCOLEX_LOOPIOASSIGNER_H
#define PROTOCOLEX_LOOPIOASSIGNER_H

#include "Common/Constant.h"

#include "llvm/Pass.h"
#include "llvm/IR/Constants.h"
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;




struct LoopIOFlagAssigner : public ModulePass
{
    static char ID;
    std::set<Loop *> LoopSet;

    LoopIOFlagAssigner();

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;
    virtual bool runOnModule(Module& M);
    virtual void print(raw_ostream &O, const Module * M) const;

    void getLoopSet(Loop *lp);
    void MarkLoopIOFlagToFunction(Module &M, Function *F);

};


#endif //PROTOCOLEX_LOOPIOASSIGNER_H
