#include <set>
#include <stack>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "Common/BasicBlockCommon.h"
#include "Common/InstructionCommon.h"
#include "GetBasicBlockList/GetBasicBlockList.h"

#include "llvm/Analysis/CallGraph.h"


using namespace std;
using namespace llvm;

static RegisterPass<BasicBlockList> X(
        "basic-block-list",
        "get basic block list from inst id list",
        false,
        true);

static cl::opt<std::string> instIdsFile(
        "instIdsFile",
        cl::desc("The name of File to store instruction ids"), cl::Optional,
        cl::value_desc("instIdsFile"));


static cl::opt<std::string> strFileName(
        "strFileName",
        cl::desc("The name of File to store output"), cl::Optional,
        cl::value_desc("strFileName"));


char BasicBlockList::ID = 0;

/*
 *  Local functions start
 */

void printInstructionID(Module &M, const std::string FuncName) {
    Function *targetFunc = M.getFunction(FuncName);
    if (targetFunc) {
        for (Function::iterator FB = targetFunc->begin(); FB != targetFunc->end(); FB++) {
            for (BasicBlock::iterator BI = FB->begin(); BI != FB->end(); BI++) {
                Instruction *II = &*BI;
                errs() << *II << "======" << GetInstructionID(II) << "\n";
            }
        }
    }

    for (Module::iterator F = M.begin(); F != M.end(); F++) {
        Function *Func = &*F;

        for (Function::iterator BI = Func->begin(); BI != Func->end(); BI++) {
            BasicBlock *B = &*BI;

            for (BasicBlock::iterator II = B->begin(); II != B->end(); II++) {

                Instruction *inst = &*II;
                errs() << *inst << "===" << GetBasicBlockID(B) << "====" << GetInstructionID(inst) << "\n";
            }
        }
    }
}


Function *getCallFunction(Instruction *Inst) {
    if (Inst->getOpcode() == Instruction::Call) {

        CallSite ci(Inst);
        Function *Callee = dyn_cast<Function>(ci.getCalledValue()->stripPointerCasts());

        if (Callee) {
            if (Callee->begin() != Callee->end()) {
                return Callee;
            }
        }
    }

    return NULL;
}


std::map<Instruction *, Function *> getCalleeVec(Function *F) {

    std::map<Instruction *, Function *> Callee;

    for (Function::iterator BI = F->begin(); BI != F->end(); BI++) {

        BasicBlock *BB = &*BI;
        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {

            Instruction *Inst = &*II;
            Function *Func = getCallFunction(Inst);

            if (Func) {
                Callee[Inst] = Func;
            }
        }
    }

    return Callee;
}


void CollectTreeChildren(struct TreeNode *Node, Function *FE,
                         std::set<Function *> VisitedFunc) {

    std::map<Instruction *, Function *> CallMap = getCalleeVec(Node->function);

    map<Instruction *, Function *>::iterator
            itMapBegin = CallMap.begin();
    map<Instruction *, Function *>::iterator
            itMapEnd = CallMap.end();

    if (!CallMap.empty()) {

        while (itMapBegin != itMapEnd) {

            if (VisitedFunc.find(itMapBegin->second) == VisitedFunc.end()) {

                struct TreeNode *node = new TreeNode;
                node->callInst = itMapBegin->first;
                node->function = itMapBegin->second;
                node->parent = Node;
                Node->children.push_back(node);
                VisitedFunc.insert(itMapBegin->second);

                if (itMapBegin->second != FE) {
                    CollectTreeChildren(node, FE, VisitedFunc);
                }
            }

            VisitedFunc.clear();
            itMapBegin++;
        }
    }
}


struct TreeNode *GetFFTree(Function *FS, Function *FE) {

    std::map<int, std::vector<Function *>> PathMap;
    std::vector<Function *> FuncPath;
    struct TreeNode *root = new TreeNode;

    if (FS == FE) {
        root->function = FS;
        root->children = {};
        return root;
    }

    root->function = FS;
    std::set<Function *> VisitedFunc = {};
    CollectTreeChildren(root, FE, VisitedFunc);
    return root;
}


BasicBlock *SearchBlock(Module *M, int numIndex) {

    for (Module::iterator FI = M->begin(); FI != M->end(); FI++) {

        Function *Func = &*FI;
        for (Function::iterator BI = Func->begin(); BI != Func->end(); BI++) {

            BasicBlock *B = &*BI;

            if (GetBasicBlockID(B) == numIndex) {
                return B;
            }
        }
    }

    return NULL;
}


/*
 * Local functions end
 */

void BasicBlockList::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<CallGraphWrapperPass>();
}

void BasicBlockList::print(raw_ostream &O, const Module *M) const {
    return;
}

BasicBlockList::BasicBlockList() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializeLoopInfoWrapperPassPass(Registry);
    initializeCallGraphWrapperPassPass(Registry);
}

/**
 *  The parser for parsing instruction id in instIdsFile
 * @param FilePath
 * @return array of instructions'id or NULL
 */
std::set<int> BasicBlockList::InstIdParse(const std::string &FilePath) {
    std::set<std::string> InstIds_str;
    std::set<int> InstIds;
    std::ifstream file(FilePath);

    char delim = '-';

    if (file) {
        std::string item;

        while (getline(file, item, delim)) {
            InstIds_str.insert(item);
        }

        for (auto id = InstIds_str.begin(); id != InstIds_str.end(); id++) {
            InstIds.insert(stoi(*id));
        }

        return InstIds;

    } else {
        // TODO: exception,tips: file is not found, please check file exist?
        errs() << "//--PATH-Constraints:" << "Path file is not found! \n";
        return InstIds;
    }
}

/**
 * Find nearest basic block between input basic blocks.
 * @param x
 * @param y
 * @return basic block or NULL
 */
BasicBlock *BasicBlockList::nextstep(BasicBlock *x, BasicBlock *y) {

    map<BasicBlock *, vector<BasicBlock *> > mapBlockSucc;
    set<BasicBlock *> setVisited;

    for (succ_iterator si = succ_begin(x); si != succ_end(x); si++) {
        if (*si == y) {
            return *si;
        }

        setVisited.insert(*si);
    }

    for (succ_iterator si = succ_begin(x); si != succ_end(x); si++) {
        vector<BasicBlock *> vecSucc;

        for (succ_iterator ssi = succ_begin(*si); ssi != succ_end(*si); ssi++) {
            if (setVisited.find(*ssi) == setVisited.end()) {
                vecSucc.push_back(*ssi);
            }
        }

        mapBlockSucc[*si] = vecSucc;
    }


    while (true) {
        unsigned numTotal = 0;

        map<BasicBlock *, vector<BasicBlock *> >::iterator
                itMapBegin = mapBlockSucc.begin();
        map<BasicBlock *, vector<BasicBlock *> >::iterator
                itMapEnd = mapBlockSucc.end();

        while (itMapBegin != itMapEnd) {
            numTotal += itMapBegin->second.size();
            itMapBegin++;
        }

        if (numTotal == 0) {
            break;
        }


        itMapBegin = mapBlockSucc.begin();

        while (itMapBegin != itMapEnd) {
            for (unsigned i = 0; i < itMapBegin->second.size(); i++) {
                if (itMapBegin->second[i] == y) {
                    return itMapBegin->first;
                }

                setVisited.insert(itMapBegin->second[i]);
            }

            itMapBegin++;
        }

        map<BasicBlock *, vector<BasicBlock *> > mapNewBlockSucc;

        itMapBegin = mapBlockSucc.begin();

        while (itMapBegin != itMapEnd) {
            vector<BasicBlock *> vecTmp;

            for (unsigned i = 0; i < itMapBegin->second.size(); i++) {
                for (succ_iterator ssi = succ_begin(itMapBegin->second[i]);
                     ssi != succ_end(itMapBegin->second[i]); ssi++) {
                    if (setVisited.find(*ssi) == setVisited.end()) {
                        vecTmp.push_back(*ssi);
                    }
                }
            }

            mapNewBlockSucc[itMapBegin->first] = vecTmp;

            itMapBegin++;
        }


        mapBlockSucc = mapNewBlockSucc;
    }

    return NULL;
}

/**
 * find the path to FE and record the result to BasicBlockList::NodePathMap
 *
 * @param root The root of functions' call tree
 * @param FE  The destination function
 * @param VisitedNodes  default is {}
 */
void BasicBlockList::FindPath(struct TreeNode *root, Function *FE,
                              std::vector<TreeNode *> VisitedNodes) {

    VisitedNodes.push_back(root);

    if (root->function == FE) {
        totalPaths++;
        std::vector<TreeNode *> tempVec = VisitedNodes;
        NodePathMap[totalPaths] = tempVec;

    } else {

        if (root->children.size() > 0) {
            for (auto *node: root->children) {
                FindPath(node, FE, VisitedNodes);
            }
        }

    }

    VisitedNodes.pop_back();
}

/**
 * Get the Basic Block ID list from BS to BE
 * @param BS The start basic block
 * @param BE The end basic block
 * @return the array of store all bb ids
 */
std::vector<int> BasicBlockList::GetBBIdList(BasicBlock *BS, BasicBlock *BE) {
    std::vector<int> path;
    bool find_path = false;

    auto _next_step = nextstep(BS, BE);
    if (_next_step) {
        find_path = true;
        path.push_back(GetBasicBlockID(BS));
    }

    while (_next_step && _next_step != BE) {
        path.push_back(GetBasicBlockID(_next_step));
        _next_step = nextstep(_next_step, BE);
    }

    if (find_path) {
        path.push_back(GetBasicBlockID(BE));
    }

    return path;
}

/**
 * Get the Basic Block ID list from InstF to InstE,
 * InstF and InstE are in different functions.
 * @param InstF
 * @param InstE
 */
void BasicBlockList::GetBBIdList(Instruction *InstF, Instruction *InstE) {
    std::map<Instruction *, Function *> CandidateMap;

    Function *Func = InstF->getFunction();
    Function *FE = InstE->getFunction();
    LoopInfo &LoopInfo = getAnalysis<LoopInfoWrapperPass>(*Func).getLoopInfo();

    for (Function::iterator BI = Func->begin(); BI != Func->end(); BI++) {

        BasicBlock *BBL = &*BI;
        for (BasicBlock::iterator II = BBL->begin(); II != BBL->end(); II++) {

            Instruction *inst = &*II;
            Function *callFunc = getCallFunction(inst);

            if (callFunc) {
                for (auto &loop:LoopInfo) {
                    if (loop->contains(InstF) && loop->contains(inst)) {
                        CandidateMap[inst] = callFunc;
                    }
                }
            }
        }
    }

    map<Instruction *, Function *>::iterator
            itMapBegin = CandidateMap.begin();
    map<Instruction *, Function *>::iterator
            itMapEnd = CandidateMap.end();

    while (itMapBegin != itMapEnd) {
        struct TreeNode *root = GetFFTree(itMapBegin->second, FE);
        root->callInst = itMapBegin->first;
        FindPath(root, FE, {});
        itMapBegin++;
    }

    // get all node path!

    map<unsigned, std::vector<TreeNode *>>::iterator
            itBegin = NodePathMap.begin();
    map<unsigned, std::vector<TreeNode *>>::iterator
            itEnd = NodePathMap.end();

    unsigned counter = 0;
    while (itBegin != itEnd) {
        std::vector<int> tempVec = GetBBPath(InstF, InstE, itBegin->second);
        counter++;
        BBIdPathMap[counter] = tempVec;
        itBegin++;
    }
}

/**
 * Get the Basic Block ID list from startInst to endInst,
 * startInst and endInst in same function.
 * @param startInst
 * @param endInst
 * @return
 */
std::vector<int> BasicBlockList::GetBBPath(Instruction *startInst, Instruction *endInst) {
    std::vector<int> Paths;

    if (startInst->getParent() == endInst->getParent()) {
        int bb_id = GetBasicBlockID(startInst->getParent());
        Paths.push_back(bb_id);

    } else {
        std::vector<int> tempVec = GetBBIdList(startInst->getParent(),
                                               endInst->getParent());
        Paths.insert(Paths.end(), tempVec.begin(), tempVec.end());
    };

    return Paths;
}

/**
 * Get BB ID from NodePath、InstF and InstE
 * @param InstF  start instruction
 * @param InstE   end instruction
 * @param NodePath the array contains many TreeNode
 * @return the array of bb id
 */
std::vector<int> BasicBlockList::GetBBPath(Instruction *InstF, Instruction *InstE,
                                           std::vector<TreeNode *> NodePath) {
    std::vector<int> BbIDList;

    if (NodePath.empty()) {
        return BbIDList;
    }

    TreeNode *firstNode = *(NodePath.begin());
    Instruction *FirstCallInst = firstNode->callInst;

    // first inst to first function
    if ((GetInstructionID(FirstCallInst) > GetInstructionID(InstF)) &&
        (FirstCallInst->getParent() == InstF->getParent())) {
        BbIDList.push_back(GetBasicBlockID(InstF->getParent()));
    } else {
        std::vector<int> temVec = GetBBIdList(InstF->getParent(), FirstCallInst->getParent());
        BbIDList.insert(BbIDList.end(), temVec.begin(), temVec.end());
    }

    // FIXME:: process function path, TOO HACK!
    TreeNode *nextNode;
    for (std::vector<TreeNode *>::iterator NodeIt = NodePath.begin(); NodeIt != (--NodePath.end());) {

        TreeNode *Node = *NodeIt;
        auto temp_next = ++NodeIt;
        nextNode = *temp_next;

        Instruction *startInst = &*(Node->function->begin()->begin());
        Instruction *endInst = nextNode->callInst;
        std::vector<int> tempVec = GetBBPath(startInst, endInst);
        BbIDList.insert(BbIDList.end(), tempVec.begin(), tempVec.end());
    }

    // FIXME:: last function to last instruction, TOO HACK!
    if (nextNode) {
        Instruction *startInst = &*(nextNode->function->begin()->begin());
        std::vector<int> tempVec = GetBBPath(startInst, InstE);
        BbIDList.insert(BbIDList.end(), tempVec.begin(), tempVec.end());
    }

    return BbIDList;
}

/**
 *
 * @param BbPath
 * @return
 */
std::vector<int> BasicBlockList::GetFullBBPath(Module &M, std::vector<int> BbPath) {

    if (BbPath.size() == 0) {
        return BbPath;
    }

    // copy to a new vector
    std::vector<int> NewBbPath;

    for (std::vector<int>::iterator BbI = BbPath.begin(); BbI != BbPath.end(); BbI++) {

        int i = *BbI;
        NewBbPath.insert(NewBbPath.end(), i);
        BasicBlock *BB = SearchBlock(&M, i);

        if (BB) {

            for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {

                Instruction *Inst = &*II;
                if (Inst->getOpcode() == Instruction::Call) {

                    CallSite ci(Inst);
                    Function *Callee = dyn_cast<Function>(ci.getCalledValue()->stripPointerCasts());

                    if (Callee && !Callee->isIntrinsic()) {

                        BasicBlock *firstBB = &*(Callee->begin());
                        BasicBlock *lastBB;
                        for (Function::iterator BI = Callee->begin(); BI != Callee->end(); BI++) {
                            lastBB = &*BI;
                        }
                        std::vector<int> BBPath = GetBBIdList(firstBB, lastBB);
                        unsigned long last_size = 0;

                        while (BBPath.size() > last_size) {
                            last_size = BBPath.size();
                            BBPath = GetFullBBPath(M, BBPath);
                        }

                        NewBbPath.insert(NewBbPath.end(), BBPath.begin(), BBPath.end());
                    }
                }
             }
        }
    }

    return NewBbPath;
}

bool BasicBlockList::runOnModule(Module &M) {

//    printInstructionID(M, "");

    if (instIdsFile.empty()) {
        errs() << "Please set input file name!" << "\n";
        return false;
    }

    if (strFileName.empty()) {
        errs() << "Please set output file name!" << "\n";
        return false;
    }

    ofstream outFile;
    outFile.open(strFileName, std::ofstream::out);

    std::set<int> InstIds = InstIdParse(instIdsFile);

    if (!InstIds.empty()) {
        std::set<Instruction *> InstSet;

        for (Module::iterator F = M.begin(); F != M.end(); F++) {
            Function *Func = &*F;

            for (Function::iterator BI = Func->begin(); BI != Func->end(); BI++) {
                BasicBlock *B = &*BI;

                for (BasicBlock::iterator II = B->begin(); II != B->end(); II++) {

                    Instruction *inst = &*II;
                    int current_inst_id = GetInstructionID(inst);

                    if (current_inst_id > 0) {
                        if (InstIds.count(current_inst_id) != 0) {
                            InstSet.insert(inst);
                        }
                    }
                }
            }
        }

        if (InstSet.empty()) {
            errs() << "Instruction Set is empty!" << "\n";
            return false;
        }

        // copy InstSet to a NewSet!
        std::set<Instruction *> NewInstSet;
        NewInstSet.insert(InstSet.begin(), InstSet.end());

        for (auto *inst_f: InstSet) {

            for (auto *inst_b: NewInstSet) {

                std::vector<int> BbPath;
                BbPath.clear();

                Function *f_f = inst_f->getFunction();
                Function *f_b = inst_b->getFunction();

                // inst_f and inst_b in same function!
                if (f_f == f_b) {

                    BbPath = GetBBIdList(inst_f->getParent(),
                                         inst_b->getParent());

                    // FIXME: If we don't need all path, we can comment this code!
                    BbPath = GetFullBBPath(M, BbPath);

                    if (!BbPath.empty()) {
                        // store to out file
                        for (int i = 0; i < BbPath.size(); i++) {
                            if (i != BbPath.size() - 1)
                                outFile << BbPath[i] << "-";
                            else
                                outFile << BbPath[i];
                        }
                        outFile << "\n";

                    } else {
                        errs() << "there is no path" << "\n";
                    }

                } else {
                    // inst_f and inst_b in different functions.
                    // the result store to BBIdPathMap
                    GetBBIdList(inst_f, inst_b);
                    map<unsigned, std::vector<int>>::iterator
                            itBegin = BBIdPathMap.begin();
                    map<unsigned, std::vector<int>>::iterator
                            itEnd = BBIdPathMap.end();

                    while (itBegin != itEnd) {

                        BbPath = itBegin->second;

                        // FIXME: If we don't need all path, we can comment this code!
                        BbPath = GetFullBBPath(M, BbPath);

                        if (!BbPath.empty()) {
                            // store to out file
                            for (int i = 0; i < BbPath.size(); i++) {
                                if (i != BbPath.size() - 1)
                                    outFile << BbPath[i] << "-";
                                else
                                    outFile << BbPath[i];
                            }
                            outFile << "\n";

                        } else {
                            errs() << "there is no path" << "\n";
                        }

                        itBegin++;
                    }
                }
            }
        }

    } else {
        errs() << "Instruction Ids list is empty!" << "\n";
    }

    outFile.close();
    return false;
}
