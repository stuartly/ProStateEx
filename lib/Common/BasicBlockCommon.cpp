#include <set>
#include <map>
#include <vector>

#include "Common/BasicBlockCommon.h"

#include "llvm/IR/Constants.h"


using namespace llvm;
using namespace std;


int GetBasicBlockID(BasicBlock *BB) {
    if (BB->begin() == BB->end()) {
        return -1;
    }

    Instruction *I = &*(BB->begin());

    MDNode *Node = I->getMetadata("bb_id");
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

BasicBlock *SearchBlockByIndex(Function *pFunction, unsigned numIndex) {
    for (Function::iterator BI = pFunction->begin(); BI != pFunction->end(); BI++) {
        BasicBlock *BB = &*BI;

        if (GetBasicBlockID(BB) == numIndex) {
            return BB;
        }
    }

    return NULL;
}
