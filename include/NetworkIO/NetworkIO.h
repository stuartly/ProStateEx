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
#include "llvm/IR/IntrinsicInst.h"



using namespace llvm;
using namespace std;


struct NetworkIO : public ModulePass {
    static char ID;

    NetworkIO();

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;

    virtual bool runOnModule(Module &M);

    virtual void MarkIOFlagToFunction(Module &M, Function *F);



};


#endif