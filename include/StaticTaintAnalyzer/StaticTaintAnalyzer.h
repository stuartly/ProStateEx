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


struct StaticTaintAnalyzer : public ModulePass {

    static char ID;

    set<TaintFunArg> funArgListSet;

    StaticTaintAnalyzer();

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;


    virtual bool runOnModule(Module &M);

    void ImportConfig();

    int printSrcCodeInfo(Instruction *pInst);



};


#endif
