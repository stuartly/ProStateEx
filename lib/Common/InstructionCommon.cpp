#include "Common/InstructionCommon.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DebugInfo.h"

using namespace llvm;


int GetInstructionID(Instruction *II) {

    MDNode *Node = II->getMetadata("ins_id");
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

    return -1;
}


Instruction *SearchInstByIndex(Function *pFunction, unsigned numIndex) {
    for (Function::iterator BI = pFunction->begin(); BI != pFunction->end(); BI++) {
        BasicBlock *BB = &*BI;

        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {
            Instruction *I = &*II;

            if (GetInstructionID(I) == numIndex) {
                return I;
            }
        }
    }

    return NULL;
}

std::string printSrcCodeInfo(Instruction *pInst) {
    const DILocation *DIL = pInst->getDebugLoc();

    if (!DIL)
        return "";

    char pPath[200];

    std::string sFileName = DIL->getDirectory().str() + "/" + DIL->getFilename().str();
    realpath(sFileName.c_str(), pPath);
    sFileName = std::string(sFileName);
    unsigned int numLine = DIL->getLine();
    return sFileName + ": " + std::to_string(numLine);

}

