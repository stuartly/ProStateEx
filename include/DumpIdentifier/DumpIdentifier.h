#ifndef _H_SONGLH_LOOPIDENTIFIER
#define _H_SONGLH_LOOPIDENTIFIER

#include "llvm/Pass.h"

using namespace llvm;

struct DumpIdentifier : public ModulePass {
    static char ID;

    DumpIdentifier();

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;

    virtual bool runOnModule(Module &M);

    virtual void print(raw_ostream &O, const Module *M) const;

};

#endif
