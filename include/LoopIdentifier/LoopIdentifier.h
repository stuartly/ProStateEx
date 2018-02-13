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
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "Common/Constant.h"


using namespace llvm;
using namespace std;


class LoopOfInterestInfo {
public:

    Function *Fun;
    BasicBlock *block;
    Instruction *inst;
    Loop *loop;


    bool operator<(const LoopOfInterestInfo &other) const { return loop < other.loop; }
};


struct LoopIdentifier : public ModulePass {
    static char ID;

    LoopIdentifier();

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;

    virtual bool runOnModule(Module &M);

};


#endif