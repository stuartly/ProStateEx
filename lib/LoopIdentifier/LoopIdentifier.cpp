#include "Common/BasicBlockCommon.h"
#include "Common/InstructionCommon.h"

#include "LoopIdentifier/LoopIdentifier.h"


#define Debug


static RegisterPass<LoopIdentifier> X(
        "identify-loop",
        "identify loop of interest",
        false,
        true);

#ifdef DynamicTaintAnalysis
static cl::opt<std::string> TaintedInstID(
        "TaintedInstID",
        cl::desc("Path of file containing id of tainted instruction"), cl::Optional,
        cl::value_desc("TaintedInstID"));
#endif

static cl::opt<std::string> strFileName(
        "strFileName",
        cl::desc("The name of File to store candidate loop id"), cl::Optional,
        cl::value_desc("strFileName"),
        cl::init(""));


char LoopIdentifier::ID = 0;


set<Loop *> LoopSet; /*Set storing loop and subloop */
set<LoopOfInterestInfo> LoopHeaderOfInterest;   /*Set storing the loop header of interest */


void LoopIdentifier::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<ScalarEvolutionWrapperPass>();
}


LoopIdentifier::LoopIdentifier() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializePostDominatorTreeWrapperPassPass(Registry);
    initializeDominatorTreeWrapperPassPass(Registry);
    initializeLoopInfoWrapperPassPass(Registry);
    initializeScalarEvolutionWrapperPassPass(Registry);
}


set<int> getTaintedInstIDSet(const string &TaintedInstIDPATH) {
    set<int> InstIDSet;
    ifstream file(TaintedInstIDPATH);

    if (!file) {
        errs() << "Invalid struct of interest path\n";
    } else {
        string line;
        while (getline(file, line)) {
            int inst_id = atoi(line.c_str());
            InstIDSet.insert(inst_id);
        }
    }

    return InstIDSet;
}


void getLoopSet(Loop *lp) {

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


bool IsIgnoreLoop(Loop *pLoop, ScalarEvolution *SE) {

    if (BasicBlock *ExitingBB = pLoop->getExitingBlock()) {

        TerminatorInst *pTerminator = ExitingBB->getTerminator();

        if (pTerminator == NULL) {
            return false;
        }

        if (BranchInst *pBranch = dyn_cast<BranchInst>(pTerminator)) {
            if (pBranch->isConditional()) {

                if (ICmpInst *pCmp = dyn_cast<ICmpInst>(pBranch->getCondition())) {

                    if (SE->isSCEVable(pCmp->getOperand(0)->getType())) {

                        const SCEV *V = SE->getSCEV(pCmp->getOperand(0));
                        if (V != NULL) {

                            if (const SCEVAddRecExpr *AddRec = dyn_cast<const SCEVAddRecExpr>(V)) {
                                if (AddRec->getOperand(1)->isOne()) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}


bool isIOInst(Instruction *Inst) {

    MDNode *Node = Inst->getMetadata(IO_CALL_FLAG);
    if (Node) {
        return true;
    }

    return false;
}


bool HasIO(Function *F) {

    for (Function::iterator BB = F->begin(); BB != F->end(); BB++) {

        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); ++II) {

            Instruction *inst = &*II;
            MDNode *Node = inst->getMetadata(IO_FLAG);
            if (Node) {
                return true;
            }
        }
    }

    return false;
}

int HasLooPIO(Function *F) {

    for (Function::iterator BB = F->begin(); BB != F->end(); BB++) {
        if (BB->begin() == BB->end()) {
            return -1;
        }

        Instruction *I = &*(BB->begin());

        MDNode *Node = I->getMetadata(LOOP_IO_FLAG);
        if (!Node) {
            return -1;
        }

        assert(Node->getNumOperands() == 1);
        const Metadata *MD = Node->getOperand(0);
        if (auto *MDV = dyn_cast<ValueAsMetadata>(MD)) {
            Value *V = MDV->getValue();
            ConstantInt *CI = dyn_cast<ConstantInt>(V);
            assert(CI);
            return CI->getZExtValue();
        }
    }

    return -1;
}


bool isTaintedInst(Instruction *Inst, set<int> &TaintedInstIDSet) {
    int inst_id = GetInstructionID(Inst);
    if (TaintedInstIDSet.find(inst_id) != TaintedInstIDSet.end()) {
        return true;
    }

    return false;
}


bool LoopIdentifier::runOnModule(Module &M) {

    if (strFileName.empty()) {
        errs() << "Please set output file path." << "\n";
        return false;
    }

#ifdef DynamicTaintAnalysis
    if (TaintedInstID.empty()) {
        errs() << "Please set Tainted Inst ID file path." << "\n";
        return false;
    }
    set<int> TaintedInstIDSet = getTaintedInstIDSet(TaintedInstID);
#endif

    LoopHeaderOfInterest.clear();

    //identify loops, and store loop-header into HeaderSet
    for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {
        Function *F = &*FI;

        if (F->empty() || HasIO(F) == false)
            continue;

        LoopInfo &LoopInfo = getAnalysis<LoopInfoWrapperPass>(*F).getLoopInfo();

        ScalarEvolution *SE = &getAnalysis<ScalarEvolutionWrapperPass>(*F).getSE();

        if (LoopInfo.empty()) {
            continue;
        }

        //get loop set
        LoopSet.clear();
        for (auto &loop:LoopInfo) {
            getLoopSet(loop); //including the loop itself
        }

        if (LoopSet.empty()) {
            continue;
        }


        for (Loop *loop:LoopSet) {

            if (IsIgnoreLoop(loop, SE)) {
                continue;
            }

            for (Loop::block_iterator BI = loop->block_begin(); BI != loop->block_end(); BI++) {
                BasicBlock *B = *BI;
                for (BasicBlock::iterator BBI = B->begin(); BBI != B->end(); BBI++) {
                    Instruction *Inst = &*BBI;

#ifdef DynamicTaintAnalysis
                    if (isTaintedInst(Inst, TaintedInstIDSet)) {
                         LoopOfInterestInfo LI;
                        LI.inst = Inst;
                        LI.block = loop->getHeader();
                        LI.loop = loop;
                        LI.Fun = F;
                        LoopHeaderOfInterest.insert(LI);
                    }
#endif


#ifndef DynamicTaintAnalysis
                    // Case 1: IO in Loop
                    if (isIOInst(Inst)) {
                        LoopOfInterestInfo LI;
                        LI.inst = Inst;
                        LI.block = loop->getHeader();
                        LI.loop = loop;
                        LI.Fun = F;
                        LoopHeaderOfInterest.insert(LI);
                    }
#endif
                }
            }
        }


    }

    /* output to file */
    ofstream f_out;
    f_out.open(strFileName, std::ofstream::out | std::ofstream::app);

    if (!LoopHeaderOfInterest.empty()) {
        for (auto &ele: LoopHeaderOfInterest) {
            f_out << "Loop in Function: " << ele.Fun->getName().str() << "\n";
            f_out << "Loop header ID: " << GetBasicBlockID(ele.block) << "\n";
            f_out << "Inst in Loop: " << printSrcCodeInfo(ele.inst) << "\n";
        }
    }

    f_out.close();

    return false;

}



