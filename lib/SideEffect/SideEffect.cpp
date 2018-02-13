#include <set>
#include <string>
#include <vector>
#include <map>

#include "Common/BasicBlockCommon.h"
#include "Common/FunctionCommon.h"
#include "Common/InstructionCommon.h"
#include "SideEffect/SideEffect.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Support/CommandLine.h"

using namespace std;
using namespace llvm;

static RegisterPass<SideEffect> X(
        "sideeffect-analysis",
        "identify side effect instructions inside a loop or callback",
        false,
        true);


static cl::opt<std::string> strFuncName("strFuncName",
                                        cl::desc("Function Name"), cl::Optional,
                                        cl::value_desc("strFuncName"));

static cl::opt<unsigned> numLoopHeader("numLoopHeader",
                                       cl::desc("Block Number for Inner Loop Header"), cl::Optional,
                                       cl::value_desc("numLoopHeader"));


void SearchDominatingBlocks(Loop *pLoop, set<BasicBlock *> &setDominatingBlocks, DominatorTree *DT) {
    setDominatingBlocks.clear();

    BasicBlock *pHeader = pLoop->getHeader();
    setDominatingBlocks.insert(pHeader);

    while (true) {
        int last = setDominatingBlocks.size();

        for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
            BasicBlock *BB = *BI;

            if (setDominatingBlocks.find(BB) != setDominatingBlocks.end()) {
                continue;
            }

            bool flag = true;

            for (Loop::block_iterator BBI = pLoop->block_begin(); BBI != pLoop->block_end(); BBI++) {
                BasicBlock *BBB = *BBI;

                if (BB == BBB) {
                    continue;
                }

                if (setDominatingBlocks.find(BBB) != setDominatingBlocks.end()) {
                    continue;
                }

                if (!DT->dominates(BB, BBB)) {
                    flag = false;
                    break;
                }
            }

            if (flag) {
                setDominatingBlocks.insert(BB);
            }
        }

        if (setDominatingBlocks.size() == last) {
            break;
        }
    }
}


void GetGenKillSet(BasicBlock *b, set<BasicBlock *> &setAllLoopBlocks, set<Instruction *> &setGen,
                   set<Instruction *> &setKill) {
    setGen.clear();
    setKill.clear();

    for (BasicBlock::iterator II = b->begin(); II != b->end(); II++) {
        Instruction *I = &*II;
        setKill.insert(I);
    }

    for (BasicBlock::iterator II = b->begin(); II != b->end(); II++) {
        Instruction *I = &*II;

        if (CallInst *pCall = dyn_cast<CallInst>(I)) {
            if (pCall->getCalledFunction() != NULL && pCall->getCalledFunction()->getName() == "llvm.dbg.value") {
                continue;
            }

        }

        if (PHINode *p = dyn_cast<PHINode>(I)) {
            for (User::op_iterator op = p->op_begin(); op != p->op_end(); op++) {
                if (Instruction *pI = dyn_cast<Instruction>(op)) {
                    if (setAllLoopBlocks.find(pI->getParent()) != setAllLoopBlocks.end()) {
                        setGen.insert(pI);
                    }
                }
            }
        } else {
            for (User::op_iterator op = I->op_begin(); op != I->op_end(); op++) {
                if (Instruction *pI = dyn_cast<Instruction>(op)) {
                    if (setKill.find(pI) == setKill.end() &&
                        setAllLoopBlocks.find(pI->getParent()) != setAllLoopBlocks.end()) {
                        setGen.insert(pI);
                    }
                }
            }

        }
    }

    //errs() << setGen.size() << " "  << setKill.size() << "\n";

}


void LoopLiveAnalysis(Loop *pLoop, map<BasicBlock *, set<Instruction *> > &mapBlockAfter) {
    set<BasicBlock *> setAllLoopBlocks;
    vector<BasicBlock *> worklist;
    map<BasicBlock *, set<Instruction *> > mapBlockGen;
    map<BasicBlock *, set<Instruction *> > mapBlockKill;

    map<BasicBlock *, set<Instruction *> > mapBlockBefore;

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        setAllLoopBlocks.insert(BB);
        worklist.push_back(BB);

        set<Instruction *> setBefore;
        set<Instruction *> setAfter;

        mapBlockAfter[BB] = setAfter;
        mapBlockBefore[BB] = setBefore;
    }

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        set<Instruction *> setGen;
        set<Instruction *> setKill;

        GetGenKillSet(BB, setAllLoopBlocks, setGen, setKill);
        mapBlockGen[BB] = setGen;
        mapBlockKill[BB] = setKill;
    }

/*
	map<BasicBlock *, set<Instruction *> >::iterator itMapBegin = mapBlockGen.begin();
	map<BasicBlock *, set<Instruction *> >::iterator itMapEnd   = mapBlockGen.end();

	while(itMapBegin != itMapEnd)
	{
		itMapBegin->first->dump();
		errs() << itMapBegin->second.size() << "\n";

		itMapBegin++;
	}
*/
    while (worklist.size() != 0) {
        BasicBlock *BB = worklist.back();
        worklist.pop_back();

        set<Instruction *>::iterator itSetBegin = mapBlockAfter[BB].begin();
        set<Instruction *>::iterator itSetEnd = mapBlockAfter[BB].end();

        mapBlockBefore[BB].clear();
        while (itSetBegin != itSetEnd) {
            mapBlockBefore[BB].insert(*itSetBegin);
            itSetBegin++;
        }

        itSetBegin = mapBlockKill[BB].begin();
        itSetEnd = mapBlockKill[BB].end();

        while (itSetBegin != itSetEnd) {
            if (mapBlockBefore[BB].find(*itSetBegin) != mapBlockBefore[BB].end()) {
                mapBlockBefore[BB].erase(*itSetBegin);
            }

            itSetBegin++;
        }

        itSetBegin = mapBlockGen[BB].begin();
        itSetEnd = mapBlockGen[BB].end();

        while (itSetBegin != itSetEnd) {
            mapBlockBefore[BB].insert(*itSetBegin);
            itSetBegin++;
        }

        for (pred_iterator PI = pred_begin(BB); PI != pred_end(BB); PI++) {
            BasicBlock *pPBlock = *PI;

            if (setAllLoopBlocks.find(pPBlock) == setAllLoopBlocks.end()) {
                continue;
            }


            bool flag = true;

            itSetBegin = mapBlockBefore[BB].begin();
            itSetEnd = mapBlockBefore[BB].end();

            while (itSetBegin != itSetEnd) {
                if (mapBlockAfter[pPBlock].find(*itSetBegin) == mapBlockAfter[pPBlock].end()) {
                    mapBlockAfter[pPBlock].insert(*itSetBegin);
                    flag = false;
                }

                itSetBegin++;
            }

            if (!flag) {
                worklist.push_back(pPBlock);
            }
        }
    }


    //map<BasicBlock *, set<Instruction *> >::iterator itMapBegin = mapBlockAfter.begin();
    //map<BasicBlock *, set<Instruction *> >::iterator itMapEnd   = mapBlockAfter.end();

    //while(itMapBegin != itMapEnd)
    //{
    //	itMapBegin->first->dump();
    //	errs() << itMapBegin->second.size() << "\n";

    //	itMapBegin++;
    //}
}


void GetPreciseGenKillSet(map<BasicBlock *, map<BasicBlock *, set<Instruction *> > > &mapBlockGen,
                          map<BasicBlock *, set<Instruction *> > &mapBlockKill, Loop *pLoop) {

    set<BasicBlock *> setAllLoopBlocks;

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        setAllLoopBlocks.insert(BB);
    }

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        set<Instruction *> setKill;
        set<Instruction *> setGen;
        map<BasicBlock *, set<Instruction *> > mapGen;

        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {
            Instruction *I = &*II;

            setKill.insert(I);

            if (!isa<PHINode>(I)) {
                for (User::op_iterator op = I->op_begin(); op != I->op_end(); op++) {
                    if (Instruction *pInst = dyn_cast<Instruction>(op)) {
                        if (setKill.find(pInst) == setKill.end()) {
                            if (setAllLoopBlocks.find(pInst->getParent()) != setAllLoopBlocks.end()) {
                                setGen.insert(pInst);
                            }
                        }
                    }
                }
            }
        }

        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {
            Instruction *I = &*II;

            if (PHINode *pPHINode = dyn_cast<PHINode>(I)) {
                unsigned uIncomingNumber = pPHINode->getNumIncomingValues();
                unsigned uIndex = 0;

                for (; uIndex < uIncomingNumber; uIndex++) {
                    BasicBlock *pIncomingBlock = pPHINode->getIncomingBlock(uIndex);

                    if (setAllLoopBlocks.find(pIncomingBlock) == setAllLoopBlocks.end()) {
                        continue;
                    }

                    if (mapGen.find(pIncomingBlock) == mapGen.end()) {
                        set<Instruction *> genTmp;
                        set<Instruction *>::iterator itSetGenBegin = setGen.begin();
                        set<Instruction *>::iterator itSetGenEnd = setGen.end();

                        while (itSetGenBegin != itSetGenEnd) {
                            genTmp.insert(*itSetGenBegin);
                            itSetGenBegin++;
                        }

                        if (Instruction *pInstRes = dyn_cast<Instruction>(pPHINode->getIncomingValue(uIndex))) {
                            if (setAllLoopBlocks.find(pInstRes->getParent()) != setAllLoopBlocks.end()) {
                                genTmp.insert(pInstRes);
                            }
                        }

                        mapGen[pIncomingBlock] = genTmp;
                    } else {
                        set<Instruction *>::iterator itSetGenBegin = setGen.begin();
                        set<Instruction *>::iterator itSetGenEnd = setGen.end();

                        while (itSetGenBegin != itSetGenEnd) {
                            mapGen[pIncomingBlock].insert(*itSetGenBegin);
                            itSetGenBegin++;
                        }

                        if (Instruction *pInstRes = dyn_cast<Instruction>(pPHINode->getIncomingValue(uIndex))) {
                            if (setAllLoopBlocks.find(pInstRes->getParent()) != setAllLoopBlocks.end()) {
                                mapGen[pIncomingBlock].insert(pInstRes);
                            }
                        }
                    }
                }
            }
        }

        for (pred_iterator PI = pred_begin(BB); PI != pred_end(BB); PI++) {
            if (setAllLoopBlocks.find(*PI) == setAllLoopBlocks.end()) {
                continue;
            }

            if (mapGen.find(*PI) == mapGen.end()) {
                set<Instruction *> genTmp;
                set<Instruction *>::iterator itSetGenBegin = setGen.begin();
                set<Instruction *>::iterator itSetGenEnd = setGen.end();

                while (itSetGenBegin != itSetGenEnd) {
                    genTmp.insert(*itSetGenBegin);
                    itSetGenBegin++;
                }

                mapGen[*PI] = genTmp;
            }
        }

        mapBlockGen[BB] = mapGen;
        mapBlockKill[BB] = setKill;
    }

/*
	map<BasicBlock*, map<BasicBlock *, set<Instruction* > > >::iterator itMapBegin = mapBlockGen.begin();
	map<BasicBlock*, map<BasicBlock *, set<Instruction* > > >::iterator itMapEnd   = mapBlockGen.end();

	while(itMapBegin != itMapEnd)
	{
		map<BasicBlock *, set<Instruction* > >::iterator it = itMapBegin->second.begin();
		map<BasicBlock *, set<Instruction* > >::iterator end = itMapBegin->second.end();

		while(it != end)
		{
			errs() << it->second.size() << " ";

			it ++;
		}

		errs() << "\n";

		itMapBegin ++;
	}
*/
}

void PreciseLiveAnalysis(map<BasicBlock *, set<Instruction *> > &mapBlockAfter, Loop *pLoop) {
    vector<BasicBlock *> vecWorkList;
    mapBlockAfter.clear();

    set<BasicBlock *> setAllLoopBlocks;
    map<BasicBlock *, map<BasicBlock *, set<Instruction *> > > mapBlockBefore;

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        setAllLoopBlocks.insert(BB);
        vecWorkList.push_back(BB);
    }

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        map<BasicBlock *, set<Instruction *> > mapBefore;

        for (pred_iterator PI = pred_begin(BB); PI != pred_end(BB); PI++) {
            set<Instruction *> setBefore;
            mapBefore[*PI] = setBefore;
        }

        mapBlockBefore[BB] = mapBefore;
        set<Instruction *> setAfter;
        mapBlockAfter[BB] = setAfter;
    }

    map<BasicBlock *, map<BasicBlock *, set<Instruction *> > > mapBlockGen;
    map<BasicBlock *, set<Instruction *> > mapBlockKill;

    GetPreciseGenKillSet(mapBlockGen, mapBlockKill, pLoop);

    while (vecWorkList.size() > 0) {
        BasicBlock *b = vecWorkList.back();
        vecWorkList.pop_back();

        for (pred_iterator PI = pred_begin(b); PI != pred_end(b); PI++) {
            if (setAllLoopBlocks.find(*PI) == setAllLoopBlocks.end()) {
                continue;
            }

            mapBlockBefore[b][*PI].clear();

            set<Instruction *>::iterator itSetBegin = mapBlockAfter[b].begin();
            set<Instruction *>::iterator itSetEnd = mapBlockAfter[b].end();

            while (itSetBegin != itSetEnd) {
                mapBlockBefore[b][*PI].insert(*itSetBegin);
                itSetBegin++;
            }

            itSetBegin = mapBlockKill[b].begin();
            itSetEnd = mapBlockKill[b].end();

            while (itSetBegin != itSetEnd) {
                if (mapBlockBefore[b][*PI].find(*itSetBegin) != mapBlockBefore[b][*PI].end()) {
                    mapBlockBefore[b][*PI].erase(*itSetBegin);
                }

                itSetBegin++;
            }

            itSetBegin = mapBlockGen[b][*PI].begin();
            itSetEnd = mapBlockGen[b][*PI].end();

            while (itSetBegin != itSetEnd) {
                mapBlockBefore[b][*PI].insert(*itSetBegin);
                itSetBegin++;
            }

            itSetBegin = mapBlockBefore[b][*PI].begin();
            itSetEnd = mapBlockBefore[b][*PI].end();

            bool flag = false;

            while (itSetBegin != itSetEnd) {
                if (mapBlockAfter[*PI].find(*itSetBegin) == mapBlockAfter[*PI].end()) {
                    mapBlockAfter[*PI].insert(*itSetBegin);
                    flag = true;
                }

                itSetBegin++;
            }

            if (flag) {
                vecWorkList.push_back(*PI);
            }
        }
    }
}

void GetBackEdgeBlocks(Loop *pLoop, set<BasicBlock *> &setBackEdgeBlocks) {
    BasicBlock *pHeader = pLoop->getHeader();
    set<BasicBlock *> setAllLoopBlocks;

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        setAllLoopBlocks.insert(BB);
    }

    for (pred_iterator PI = pred_begin(pHeader); PI != pred_end(pHeader); PI++) {
        BasicBlock *BB = *PI;

        if (setAllLoopBlocks.find(BB) != setAllLoopBlocks.end()) {
            setBackEdgeBlocks.insert(BB);
        }
    }
}

void GetAllUsesInLoop(Instruction *pInst, Loop *pLoop, set<Instruction *> &setUses) {
    set<BasicBlock *> setAllLoopBlocks;

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        setAllLoopBlocks.insert(BB);
    }

    vector<Instruction *> vecWorkList;
    vecWorkList.push_back(pInst);

    set<Instruction *> setProcessed;

    while (vecWorkList.size() > 0) {
        Instruction *I = vecWorkList.back();
        vecWorkList.pop_back();
        setProcessed.insert(I);

        for (Instruction::user_iterator UI = I->user_begin(); UI != I->user_end(); UI++) {
            if (Instruction *pI = dyn_cast<Instruction>(*UI)) {
                if (setAllLoopBlocks.find(pI->getParent()) == setAllLoopBlocks.end()) {
                    continue;
                }

                if (isa<PHINode>(pI)) {
                    if (setProcessed.find(pI) == setProcessed.end()) {
                        vecWorkList.push_back(pI);
                    }
                } else {
                    setUses.insert(pI);
                }
            }
        }
    }
}

void GetAllDefInLoop(Instruction *pInst, Loop *pLoop, set<Instruction *> &setDef) {
    set<BasicBlock *> setAllLoopBlocks;

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        setAllLoopBlocks.insert(BB);
    }

    vector<Instruction *> vecWorkList;
    vecWorkList.push_back(pInst);
    set<Instruction *> setProcessed;

    while (vecWorkList.size() > 0) {
        Instruction *I = vecWorkList.back();
        vecWorkList.pop_back();

        setProcessed.insert(I);

        if (PHINode *pPHI = dyn_cast<PHINode>(I)) {
            unsigned numValues = pPHI->getNumIncomingValues();
            unsigned numIndex = 0;

            for (; numIndex < numValues; numIndex++) {
                if (isa<Constant>(pPHI->getIncomingValue(numIndex))) {
                    setDef.insert(pPHI);
                } else if (Instruction *pDef = dyn_cast<Instruction>(pPHI->getIncomingValue(numIndex))) {
                    if (setAllLoopBlocks.find(pDef->getParent()) != setAllLoopBlocks.end() &&
                        setProcessed.find(pDef) == setProcessed.end()) {
                        vecWorkList.push_back(pDef);
                    }
                }
            }
        } else {
            setDef.insert(I);
        }
    }

}


char SideEffect::ID = 0;

void SideEffect::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

SideEffect::SideEffect() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializePostDominatorTreeWrapperPassPass(Registry);
    initializeDominatorTreeWrapperPassPass(Registry);
    initializeLoopInfoWrapperPassPass(Registry);
}

void SideEffect::print(raw_ostream &O, const Module *M) const {
    return;
}

bool SideEffect::runOnModule(Module &M) {
    Function *pFunction = SearchFunctionByName(M, strFuncName);

    if (pFunction == NULL) {
        errs() << "Cannot find the input function\n";
        return false;
    }


    BasicBlock *pHeader = SearchBlockByIndex(pFunction, numLoopHeader);

    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>(*pFunction).getDomTree();
    LoopInfo *pLoopInfo = &(getAnalysis<LoopInfoWrapperPass>(*pFunction).getLoopInfo());

    if (pHeader == NULL) {
        errs() << "Cannot find the given loop header\n";
        return false;
    }

    Loop *pLoop = pLoopInfo->getLoopFor(pHeader);

    if (pLoop == NULL) {
        errs() << "Cannot find the input loop\n";
        return false;
    }


    set<BasicBlock *> setDominatingBlocks;
    set<BasicBlock *> setAllLoopBlocks;
    set<BasicBlock *> setBackEdgeBlocks;

    for (Loop::block_iterator BI = pLoop->block_begin(); BI != pLoop->block_end(); BI++) {
        BasicBlock *BB = *BI;
        setAllLoopBlocks.insert(BB);
    }

    SearchDominatingBlocks(pLoop, setDominatingBlocks, DT);
    //errs() << setDominatingBlocks.size() << "\n";
    //pLoop->dump();

    /*
    set<BasicBlock *>::iterator itSetBegin = setDominatingBlocks.begin();
    set<BasicBlock *>::iterator itSetEnd   = setDominatingBlocks.end();

    while(itSetBegin != itSetEnd)
    {
        (*itSetBegin)->dump();
        itSetBegin++;
    }
    */

    GetBackEdgeBlocks(pLoop, setBackEdgeBlocks);


    map<BasicBlock *, set<Instruction *> > mapBlockAfter;

    PreciseLiveAnalysis(mapBlockAfter, pLoop);


    set<BasicBlock *>::iterator itSetBegin = setBackEdgeBlocks.begin();
    set<BasicBlock *>::iterator itSetEnd = setBackEdgeBlocks.end();

    /*
    while(itSetBegin != itSetEnd)
    {
        set<Instruction *>::iterator itInstBegin = mapBlockAfter[*itSetBegin].begin();
        set<Instruction *>::iterator itInstEnd   = mapBlockAfter[*itSetBegin].end();

        while(itInstBegin != itInstEnd)
        {
            (*itInstBegin)->dump();
            //printSrcCodeInfo(*itInstBegin);
            itInstBegin++;
        }

        itSetBegin++;
    }
    */


    itSetBegin = setBackEdgeBlocks.begin();
    itSetEnd = setBackEdgeBlocks.end();

    set<Instruction *> setSideEffect;

    while (itSetBegin != itSetEnd) {
        set<Instruction *>::iterator itInstBegin = mapBlockAfter[*itSetBegin].begin();
        set<Instruction *>::iterator itInstEnd = mapBlockAfter[*itSetBegin].end();

        while (itInstBegin != itInstEnd) {
            set<Instruction *> setUses;
            GetAllUsesInLoop(*itInstBegin, pLoop, setUses);


            set<Instruction *>::iterator itUseSetBegin = setUses.begin();
            set<Instruction *>::iterator itUseSetEnd = setUses.end();

            bool flag = false;

            while (itUseSetBegin != itUseSetEnd) {
                if (setDominatingBlocks.find((*itUseSetBegin)->getParent()) == setDominatingBlocks.end()) {
                    flag = true;
                }

                itUseSetBegin++;
            }

            if (flag) {
                itUseSetBegin = setUses.begin();

                flag = false;

                while (itUseSetBegin != itUseSetEnd) {
                    if (isa<CmpInst>(*itUseSetBegin)) {
                        flag = true;
                        break;
                    }

                    itUseSetBegin++;
                }

                if (flag) {
                    setSideEffect.insert(*itInstBegin);
                }
            }

            itInstBegin++;
        }

        itSetBegin++;
    }

    set<Instruction *>::iterator itSEBegin = setSideEffect.begin();
    set<Instruction *>::iterator itSEEnd = setSideEffect.end();

    while (itSEBegin != itSEEnd) {
        set<Instruction *> setDef;
        GetAllDefInLoop(*itSEBegin, pLoop, setDef);

        set<Instruction *>::iterator itDefBegin = setDef.begin();
        set<Instruction *>::iterator itDefEnd = setDef.end();

        while (itDefBegin != itDefEnd) {
            errs() << "Inst " << GetInstructionID(*itDefBegin) << ":";
            (*itDefBegin)->dump();
            errs() << printSrcCodeInfo(*itDefBegin) << "\n";
            itDefBegin++;
        }

        itSEBegin++;
    }

    return false;
}