#include <set>
#include <string>
#include <vector>
#include <map>

#include "Common/InstructionCommon.h"
#include "Common/BasicBlockCommon.h"
#include "DumpIdentifier/DumpIdentifier.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/DebugInfo.h"

using namespace std;
using namespace llvm;

static RegisterPass<DumpIdentifier> X(
        "dump-function",
        "dump function",
        false,
        true);


static cl::opt<std::string> strFuncName(
        "strFuncName",
        cl::desc("Function Name"), cl::Optional,
        cl::value_desc("strFuncName"));


char DumpIdentifier::ID = 0;

void DumpIdentifier::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
}

DumpIdentifier::DumpIdentifier() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializeLoopInfoWrapperPassPass(Registry);
}

void DumpIdentifier::print(raw_ostream &O, const Module *M) const {
    return;
}

bool DumpIdentifier::runOnModule(Module &M) {
    Function *pFunction = M.getFunction(strFuncName);

    if (pFunction == NULL) {
        errs() << "Cannot find the input function\n";
        return false;
    }

    errs() << "Find function" << "\n";

    for (Function::iterator BI = pFunction->begin(); BI != pFunction->end(); BI++) {
        BasicBlock *BB = &*BI;

        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {

            Instruction *Inst = &*II;

            errs() << *Inst << " ====BB_ID: " << GetBasicBlockID(BB)
                   << " ====Inst_ID: " << GetInstructionID(Inst) << "\n";
        }

    }
}
