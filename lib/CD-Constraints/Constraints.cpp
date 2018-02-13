#include <fstream>
#include <string>

#include "CFG/CFG.h"
#include "CD-Constraints/Constraints.h"
#include "Common/BasicBlockCommon.h"
#include "Common/FunctionCommon.h"
#include "Common/InstructionCommon.h"
#include "ReachAnalysis/ReachAnalysis.h"


#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/CommandLine.h"

using namespace std;
using namespace llvm;

static RegisterPass<Constraints> X(
        "collect-constraints",
        "collect constraints for side effect instructions inside a loop or a call back",
        false,
        true);


static cl::opt<std::string> strFuncName(
        "strFuncName",
        cl::desc("Function Name"), cl::Optional,
        cl::value_desc("strFuncName"));


static cl::opt<unsigned> numLoopHeader(
        "numLoopHeader",
        cl::desc("Block Number for Inner Loop Header"), cl::Optional,
        cl::value_desc("numLoopHeader"));


static cl::opt<std::string> strInstFile(
        "strInstFile",
        cl::desc("Function Name for Side Effect Instructions"), cl::Optional,
        cl::value_desc("strInstFile"));


static cl::opt<std::string> strSTName(
        "strSTName",
        cl::desc("Struct Name"), cl::Optional,
        cl::value_desc("strSTName"));


void ParseFeaturedInstFile(string &sFileName, vector<int> &vecInstIndex) {
    ifstream ifFile(sFileName.c_str());
    string line;

    if (ifFile.is_open()) {
        while (getline(ifFile, line)) {
            if (line.find("Inst") == 0) {
                if (line.find(":") == string::npos) {
                    continue;
                }

                string sIndex = line.substr(5, line.find(':'));
                int numIndex = atoi(sIndex.c_str());
                vecInstIndex.push_back(numIndex);
            }
        }

        ifFile.close();
    }
}


void CollectDominateBlock(Instruction *pInst, Loop *pLoop, ControlDependenceGraphBase &CDG,
                          map<BasicBlock *, BasicBlock *> &mapBlockNext) {
    BasicBlock *pEndBB = NULL;

    if (PHINode *pPHI = dyn_cast<PHINode>(pInst)) {
        BasicBlock *pCurrent = pPHI->getParent();

        for (unsigned i = 0; i < pPHI->getNumIncomingValues(); i++) {
            if (isa<ConstantInt>(pPHI->getIncomingValue(i))) {
                pEndBB = pPHI->getIncomingBlock(i);
                mapBlockNext[pEndBB] = pCurrent;
                break;
            }
        }

    } else {
        pEndBB = pInst->getParent();
    }

    set<BasicBlock *> setControlBlock;

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;

        if (BB == pEndBB) {
            continue;
        }

        if (BB == pLoop->getHeader()) {
            continue;
        }

        if (CDG.influences(BB, pEndBB)) {

            BasicBlock *pBNext = nextstep(BB, pEndBB);
            assert(pBNext != NULL);
            mapBlockNext[BB] = pBNext;
/*
			TerminatorInst * pTerm = BB->getTerminator();


			if(BranchInst * pBranch = dyn_cast<BranchInst>(pTerm))
			{
				unsigned numSucc = pBranch->getNumSuccessors();
				unsigned numIndex = 0;

				while(numIndex < numSucc)
				{
					if(pBranch->getSuccessor(numIndex) == pBNext)
					{
						break;
					}

					numIndex ++;
				}

				errs() << numIndex << "\n";
			}
			else if(SwitchInst * pSwitch = dyn_cast<SwitchInst>(pTerm))
			{
				unsigned numSucc = pSwitch->getNumSuccessors();
				unsigned numIndex = 0;

				pSwitch->getSuccessor(0)->dump();
				pSwitch->getSuccessor(1)->dump();

				while(numIndex < numSucc)
				{
					if(pSwitch->getSuccessor(numIndex) == pBNext)
					{
						break;
					}

					numIndex ++;
				}

				errs() << numIndex << "\n";
			}
*/

        }
    }
}

void PrintLoad(LoadInst *pLoad) {

    vector<int> vecIndex;

    if (Instruction *pCurrent = dyn_cast<Instruction>(pLoad->getPointerOperand())) {
        while (true) {
            if (GetElementPtrInst *pGet = dyn_cast<GetElementPtrInst>(pCurrent)) {
                assert(pGet->getNumIndices() == 2);

                GetElementPtrInst::op_iterator itOpBegin = pGet->idx_begin();
                itOpBegin++;

                if (ConstantInt *pConst = dyn_cast<ConstantInt>(*itOpBegin)) {
                    //errs() << pConst->getZExtValue() << "\n";
                    vecIndex.push_back(pConst->getZExtValue());
                }

                if (Instruction *pInst = dyn_cast<Instruction>(pGet->getPointerOperand())) {
                    pCurrent = pInst;
                } else {
                    assert(0);
                }
            } else if (BitCastInst *pBit = dyn_cast<BitCastInst>(pCurrent)) {
                if (Instruction *pInst = dyn_cast<Instruction>(pBit->getOperand(0))) {
                    pCurrent = pInst;
                } else {
                    assert(0);
                }
            } else {
                break;
            }
        }

        //pCurrent->getType()->dump();

        if (PointerType *pPType = dyn_cast<PointerType>(pCurrent->getType())) {
            if (StructType *pSTType = dyn_cast<StructType>(pPType->getElementType())) {
                assert(pSTType->getName() == "struct.MIDI_MSG");
            }
        } else {
            assert(false);
        }
    }

    errs() << "struct.MIDI_MSG(";

    for (int i = vecIndex.size() - 1; i >= 0; i--) {
        errs() << vecIndex[i];

        if (i != 0) {
            errs() << ", ";
        }
    }

    errs() << ")";
}


void PrintConditions(map<BasicBlock *, BasicBlock *> &mapBlockNext) {
    map<BasicBlock *, BasicBlock *>::iterator itMapBegin = mapBlockNext.begin();
    map<BasicBlock *, BasicBlock *>::iterator itMapEnd = mapBlockNext.end();

    unsigned i = 0;

    for (; itMapBegin != itMapEnd; itMapBegin++) {
        TerminatorInst *pTerm = itMapBegin->first->getTerminator();

        if (BranchInst *pBranch = dyn_cast<BranchInst>(pTerm)) {
            unsigned numSucc = pBranch->getNumSuccessors();

            if (numSucc == 1) {
                i++;
                continue;
            }

            unsigned numIndex = 0;

            while (numIndex < numSucc) {
                if (pBranch->getSuccessor(numIndex) == itMapBegin->second) {
                    break;
                }

                numIndex++;
            }

            assert(numIndex < 2);

            //pBranch->getCondition()->dump();

            if (CmpInst *pCom = dyn_cast<CmpInst>(pBranch->getCondition())) {
                if (pCom->getPredicate() == CmpInst::ICMP_EQ) {
                    bool flag = true;

                    for (User::op_iterator OI = pCom->op_begin(); OI != pCom->op_end(); ++OI) {
                        Value *V = OI->get();

                        if (LoadInst *pLoad = dyn_cast<LoadInst>(V)) {
                            PrintLoad(pLoad);
                        } else if (Argument *pArg = dyn_cast<Argument>(V)) {
                            errs() << "Argument " << pArg->getArgNo();
                        } else if (PHINode *pPHI = dyn_cast<PHINode>(V)) {
                            errs() << "State ";
                        } else {
                            errs() << "others: ";
                            V->dump();
                        }

                        if (flag) {
                            errs() << " == ";
                            flag = false;
                        }
                    }

                    errs() << "\n";

                    printSrcCodeInfo(pCom);
                }
            }
        } else if (SwitchInst *pSwitch = dyn_cast<SwitchInst>(pTerm)) {
            if (LoadInst *pLoad = dyn_cast<LoadInst>(pSwitch->getCondition())) {
                PrintLoad(pLoad);
            }

            errs() << " == ";

            for (SwitchInst::CaseIt ci = pSwitch->case_begin(); ci != pSwitch->case_end(); ci++) {
                if (ci->getCaseSuccessor() == itMapBegin->second) {
                    if (ConstantInt *pConst = dyn_cast<ConstantInt>(ci->getCaseValue())) {
                        errs() << pConst->getZExtValue();
                    }
                }

            }

            errs() << "\n";
            errs() << printSrcCodeInfo(pSwitch) << "\n";
        }
    }
}


char Constraints::ID = 0;

void Constraints::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

Constraints::Constraints() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializePostDominatorTreeWrapperPassPass(Registry);
    initializeDominatorTreeWrapperPassPass(Registry);
    initializeLoopInfoWrapperPassPass(Registry);
}

void Constraints::print(raw_ostream &O, const Module *M) const {
    return;
}

bool Constraints::runOnModule(Module &M) {
    Function *pFunction = SearchFunctionByName(M, strFuncName);

    if (pFunction == NULL) {
        errs() << "Cannot find the input function\n";
        return false;
    }

    BasicBlock *pHeader = SearchBlockByIndex(pFunction, numLoopHeader);

    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>(*pFunction).getDomTree();
    PostDominatorTree *PDT = &getAnalysis<PostDominatorTreeWrapperPass>(*pFunction).getPostDomTree();
    LoopInfo *pLoopInfo = &(getAnalysis<LoopInfoWrapperPass>(*pFunction).getLoopInfo());

    ControlDependenceGraphBase CDG;
    CDG.graphForFunction(*pFunction, *PDT);


    if (pHeader == NULL) {
        errs() << "Cannot find the given loop header\n";
        return false;
    }

    Loop *pLoop = pLoopInfo->getLoopFor(pHeader);

    vector<int> vecInstIndex;
    ParseFeaturedInstFile(strInstFile, vecInstIndex);

    vector<int>::iterator itVecIntBegin = vecInstIndex.begin();
    vector<int>::iterator itVecIntEnd = vecInstIndex.end();

    vector<Instruction *> vecSDInst;

    while (itVecIntBegin != itVecIntEnd) {
        Instruction *I = SearchInstByIndex(pFunction, *itVecIntBegin);
        vecSDInst.push_back(I);
        itVecIntBegin++;
    }

    vector<Instruction *>::iterator itVecInstBegin = vecSDInst.begin();
    vector<Instruction *>::iterator itVecInstEnd = vecSDInst.end();

    map<Instruction *, map<BasicBlock *, BasicBlock *> > mapSECondition;

    while (itVecInstBegin != itVecInstEnd) {
        errs() << "Conditions (AND):\n\n";

        map<BasicBlock *, BasicBlock *> mapBlockNext;
        CollectDominateBlock(*itVecInstBegin, pLoop, CDG, mapBlockNext);
        mapSECondition[*itVecInstBegin] = mapBlockNext;

        PrintConditions(mapBlockNext);


        errs() << "Action :\n\n";

        if (LoadInst *pLoad = dyn_cast<LoadInst>(*itVecInstBegin)) {
            errs() << "State = ";
            PrintLoad(pLoad);
            errs() << "\n";
            errs() << printSrcCodeInfo(pLoad) << "\n";
        } else if (PHINode *pPHI = dyn_cast<PHINode>(*itVecInstBegin)) {
            for (unsigned i = 0; i < pPHI->getNumIncomingValues(); i++) {
                if (ConstantInt *pConst = dyn_cast<ConstantInt>(pPHI->getIncomingValue(i))) {
                    errs() << "State = " << pConst->getSExtValue() << "\n";
                    break;
                }
            }
        }

        errs() << "==========================================\n";

        itVecInstBegin++;

    }

    return false;
}