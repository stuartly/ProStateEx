#ifndef PROTOCOLEX_GETBASICBLOCKLIST_H
#define PROTOCOLEX_GETBASICBLOCKLIST_H

#include <set>
#include <string>
#include <ostream>
#include <fstream>

#include "llvm/Pass.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"


using namespace llvm;
using namespace std;

struct TreeNode {
    Instruction *callInst;
    Function *function;
    std::vector<TreeNode *> children;
    TreeNode *parent;
};


struct BasicBlockList : public ModulePass
{
    static char ID;
    BasicBlockList();

    unsigned totalPaths = 0;
    std::map<unsigned, std::vector<TreeNode *>> NodePathMap;
    std::map<unsigned, std::vector<int>>BBIdPathMap;

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;
    virtual bool runOnModule(Module& M);
    virtual void print(raw_ostream &O, const Module * M) const;

    std::set<int> InstIdParse(const std::string &FilePath);

    std::vector<int> GetBBIdList(BasicBlock *BS, BasicBlock *BE);
    void GetBBIdList(Instruction *InstF, Instruction *InstE);

    std::vector<int> GetBBPath(Instruction *InstF, Instruction *InstE,
                               std::vector<TreeNode *> NodePath);
    std::vector<int> GetBBPath(Instruction *startInst, Instruction *endInst);

    BasicBlock * nextstep(BasicBlock * x, BasicBlock * y);

    void FindPath(struct TreeNode *root, Function *FE,
                  std::vector<TreeNode *> VisitedNodes);

    std::vector<int> GetFullBBPath(Module& M, std::vector <int> BbPath);

};

#endif //PROTOCOLEX_GETBASICBLOCKLIST_H
