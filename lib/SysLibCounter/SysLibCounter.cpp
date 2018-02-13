#include <fstream>
#include <set>

#include "SysLibCounter/SysLibCounter.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/DebugInfo.h"

using namespace std;
using namespace llvm;

static RegisterPass<SysLibCounter> X(
        "Count-System-Call",
        "count system call in the module",
        false,
        true);

static cl::opt<std::string> strFileName(
        "strFileName",
        cl::desc("The name of File to store system library"), cl::Optional,
        cl::value_desc("strFileName"));


char SysLibCounter::ID = 0;

void SysLibCounter::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
}

SysLibCounter::SysLibCounter() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializeLoopInfoWrapperPassPass(Registry);
}

void SysLibCounter::print(raw_ostream &O, const Module *M) const {
    return;
}

bool SysLibCounter::runOnModule(Module &M) {

    if (strFileName.empty()) {
        errs() << "Please set file name!" << "\n";
        exit(1);
    }

    ofstream sysCallFile;
    sysCallFile.open(strFileName, std::ofstream::out | std::ofstream::app);

    for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {

        Function *f = &*FI;

        if (!f->isIntrinsic()) { // function name should not starts with "llvm."
            if (f->begin() == f->end()) {
                std::string fName = f->getName();
                sysCallFile << fName + "\n";
            }
        }

    }

    sysCallFile.close();
}