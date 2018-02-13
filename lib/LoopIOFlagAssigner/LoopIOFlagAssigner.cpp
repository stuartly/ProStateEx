#include <fstream>
#include <set>
#include "Common/Constant.h"
#include "Common/FunctionCommon.h"
#include "LoopIOFlagAssigner/LoopIOFlagAssigner.h"

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/MDBuilder.h"


using namespace std;
using namespace llvm;

static RegisterPass<LoopIOFlagAssigner> X(
        "Loop-IO-Flag-Assigner",
        "Assigner Loop and IO flag to a function",
        false,
        true);

static cl::opt<std::string> strFileName(
        "strFileName",
        cl::desc("The name of File to store candidate function name"), cl::Optional,
        cl::value_desc("strFileName"),
        cl::init(""));

char LoopIOFlagAssigner::ID = 0;

/* local functions start */
/*
 * If a loop has a constant step, we will ignore it.
 *
 * for(int i = 0, i < n; i++) {
 *      .......
 * }
 *
 */
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
                    // This can be used to get iterate number.
//                    if (SE->isSCEVable(pCmp->getOperand(1)->getType())) {
//
//                        const SCEV *V = SE->getSCEV(pCmp->getOperand(1));
//                        if (V != NULL) {
//                            V->dump();
//                            errs() << V->isOne() << "\n";
//                        }
//                    }
                }
            }
        }
    }

    return false;
}


void _MarkFlagFunction(Module &M, Function *F,
                       int _type, const std::string flag) {
    IntegerType *IntType = IntegerType::get(M.getContext(), 32);
    MDBuilder MDHelper(M.getContext());

    for (Function::iterator BB = F->begin(); BB != F->end(); BB++) {
        if (BB->begin() != BB->end()) {
            Constant *BBID = ConstantInt::get(IntType, _type);
            SmallVector<Metadata *, 1> Vals;
            Vals.push_back(MDHelper.createConstant(BBID));
            BB->begin()->setMetadata(flag, MDNode::get(M.getContext(), Vals));
            break;
        }
    }
}

void _MarkLoopIOFlagToFunction(Module &M, Function *F, std::set<Loop *> LoopSet) {

    if (!HasIO(F) && LoopSet.empty()) {
        _MarkFlagFunction(M, F, NoIOLoop, LOOP_IO_FLAG);

    } else if (!HasIO(F) && !LoopSet.empty()) {
        _MarkFlagFunction(M, F, OnlyLoop, LOOP_IO_FLAG);

    } else if (HasIO(F) && LoopSet.empty()) {
        _MarkFlagFunction(M, F, OnlyIO, LOOP_IO_FLAG);

    } else {
        for (Function::iterator BI = F->begin(); BI != F->end(); BI++) {

            BasicBlock *BB = &*BI;

            for (BasicBlock::iterator II = BB->begin(); II != BB->end(); ++II) {

                bool io_in_loop_flag = false;
                Instruction *inst = &*II;

                if (HasIO(inst)) {

                    // instruction in loop?
                    for (auto *loop:LoopSet) {

                        if (loop->contains(inst)) {

                            io_in_loop_flag = true;
                            if (HasLooPIO(F) == IOOutOfLoop) {

                                _MarkFlagFunction(M, F, IOLoop, LOOP_IO_FLAG);
                            } else {

                                _MarkFlagFunction(M, F, IOInLoop, LOOP_IO_FLAG);
                            }
                        }
                    }

                    if (!io_in_loop_flag && HasLooPIO(F) == IOInLoop) {
                        _MarkFlagFunction(M, F, IOLoop, LOOP_IO_FLAG);
                    } else if (!io_in_loop_flag && HasLooPIO(F) < IOOutOfLoop) {
                        _MarkFlagFunction(M, F, IOOutOfLoop, LOOP_IO_FLAG);
                    }
                }
            }
        }
    }
}


//*
// * This function is not needed.
// */
//void LoopIOFlagAssigner::MarkLoopFlagToFunction(Module &M, Function *F) {
//    _MarkFlagFunction(M, F, 1, LOOP_FLAG);
//}

/* local function end */


void LoopIOFlagAssigner::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<ScalarEvolutionWrapperPass>();
}

void LoopIOFlagAssigner::print(raw_ostream &O, const Module *M) const {
    return;
}

LoopIOFlagAssigner::LoopIOFlagAssigner() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializeScalarEvolutionWrapperPassPass(Registry);
}

void LoopIOFlagAssigner::getLoopSet(Loop *lp) {
    vector<Loop *> workList;
    LoopSet.clear();

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

void LoopIOFlagAssigner::MarkLoopIOFlagToFunction(Module &M, Function *F) {

    if (F->empty()) {
        return;
    }

    LoopInfo &LoopInfo = getAnalysis<LoopInfoWrapperPass>(*F).getLoopInfo();

    // FIXME::this code must be below the front one!!!
    ScalarEvolution *SE = &getAnalysis<ScalarEvolutionWrapperPass>(*F).getSE();

    if (LoopInfo.empty()) {
        return;
    }

    for (auto &loop:LoopInfo) {
        //including the loop itself and its sub loop
        getLoopSet(loop);
    }

    std::set<Loop *> CandidateLoopSet;
    for (auto &loop:LoopInfo) {

        if (IsIgnoreLoop(&*loop, SE)) {
            continue;
        }

        CandidateLoopSet.insert(loop);
    }


    _MarkLoopIOFlagToFunction(M, F, CandidateLoopSet);
}

bool LoopIOFlagAssigner::runOnModule(Module &M) {

    for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {

        Function *F = &*FI;
        MarkLoopIOFlagToFunction(M, F);
    }

    // echo loop io function to temp file
    if (!strFileName.empty()) {

        ofstream funcLoopIOFile;
        funcLoopIOFile.open(strFileName, std::ofstream::out);

        std::set<Function *> funcSet_IOOutOfLoop = GetSpecialFunctionList(M, IOOutOfLoop);
        std::set<Function *> funcSet_IOInLoop = GetSpecialFunctionList(M, IOInLoop);
        std::set<Function *> funcSet_IOLoop = GetSpecialFunctionList(M, IOLoop);

        for (auto *func: funcSet_IOOutOfLoop) {

            funcLoopIOFile << func->getName().str() << "," << IOOutOfLoop << "\n";
        }

        for (auto *func: funcSet_IOInLoop) {

            funcLoopIOFile << func->getName().str() << "," << IOInLoop << "\n";
        }

        for (auto *func: funcSet_IOLoop) {

            funcLoopIOFile << func->getName().str() << "," << IOLoop << "\n";
        }

        funcLoopIOFile.close();
    }

    return false;
}