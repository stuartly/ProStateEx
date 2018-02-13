#include <fstream>

#include "Common/BasicBlockCommon.h"
#include "Common/InstructionCommon.h"
#include "LoopCounter/LoopCounter.h"


#define _DEBUG 1


static RegisterPass<LoopCounter> X(
        "Loop-Count",
        "count loop of module",
        false,
        true);


static cl::opt<std::string> strFileName(
        "strFileName",
        cl::desc("The name of File to store system library"), cl::Optional,
        cl::value_desc("strFileName"));


char LoopCounter::ID = 0;


std::set<Loop *> LoopSet; /*Set storing loop and subloop */

void LoopCounter::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

void LoopCounter::print(raw_ostream &O, const Module *M) const {
    return;
}

LoopCounter::LoopCounter() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializePostDominatorTreeWrapperPassPass(Registry);
    initializeDominatorTreeWrapperPassPass(Registry);
    initializeLoopInfoWrapperPassPass(Registry);
}


void getSubLoopSet(Loop *lp) {

    vector<Loop *> workList;
    if (lp != NULL) {
        workList.push_back(lp);
    }

    while (workList.size() != 0) {

        Loop *loop = workList.back();
        LoopSet.insert(loop);
        workList.pop_back();

        if (loop != nullptr && !loop->empty()) {

            std::vector<Loop *> &subloopVect = lp->getSubLoopsVector();
            if (subloopVect.size() != 0) {
                for (std::vector<Loop *>::const_iterator SI = subloopVect.begin(); SI != subloopVect.end(); SI++) {
                    if (*SI != NULL) {
                        if (LoopSet.find(*SI) == LoopSet.end()) {
                            workList.push_back(*SI);
                        }
                    }
                }

            }
        }
    }
}

void getLoopSet(Loop *lp) {
    if (lp != NULL && lp->getHeader() != NULL && !lp->empty()) {
        LoopSet.insert(lp);
        const std::vector<Loop *> &subloopVect = lp->getSubLoops();
        if (!subloopVect.empty()) {
            for (std::vector<Loop *>::const_iterator subli = subloopVect.begin(); subli != subloopVect.end(); subli++) {
                Loop *subloop = *subli;
                getLoopSet(subloop);
            }
        }
    }
}


bool LoopCounter::runOnModule(Module &M) {

    if (strFileName.empty()) {
        errs() << "Please set file name!" << "\n";
        exit(1);
    }

    ofstream loopCounterFile;
    loopCounterFile.open(strFileName, std::ofstream::out | std::ofstream::app);

    //identify loops, and store loop-header into HeaderSet
    for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {

        Function *F = &*FI;
        if (F->empty())
            continue;

        //clear loop set
        LoopSet.clear();

        LoopInfo &LoopInfo = getAnalysis<LoopInfoWrapperPass>(*F).getLoopInfo();

        if (LoopInfo.empty()) {
            continue;
        }

        for (auto &loop:LoopInfo) {
            //LoopSet.insert(loop);
            getSubLoopSet(loop); //including the loop itself
        }

        if (LoopSet.empty()) {
            continue;
        }

        for (Loop *loop:LoopSet) {

            if (loop == nullptr)
                continue;
            Instruction *inst = &*(loop->getHeader()->begin());
            std::string str = printSrcCodeInfo(inst);
            loopCounterFile << str;
        }
    }

    loopCounterFile.close();
    return false;

}



