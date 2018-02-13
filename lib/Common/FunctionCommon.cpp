#include "Common/Constant.h"
#include "Common/FunctionCommon.h"

#include "llvm/IR/Constants.h"


using namespace llvm;


Function *SearchFunctionByName(Module &M, string strFunctionName) {

    for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {

        Function *f = &*FI;

        if (f->getName().str() == strFunctionName) {
            return f;
        }

    }

    return NULL;
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


bool HasIO(Instruction *Inst) {

    MDNode *Node = Inst->getMetadata(IO_CALL_FLAG);
    if (Node) {
        return true;
    }

    return false;
}

bool HasLoop(Function *F) {

    for (Function::iterator BB = F->begin(); BB != F->end(); BB++) {
        if (BB->begin() == BB->end()) {
            return -1;
        }

        Instruction *I = &*(BB->begin());

        MDNode *Node = I->getMetadata(LOOP_FLAG);
        if (Node) {
            return true;
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

/*
 *
 */
std::set<Function *> GetSpecialFunctionList(Module &M, int FlagType) {

    std::set<Function *> TargetFuncSet;

    for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {

        Function *F = &*FI;

        if (HasLooPIO(F) == FlagType) {
            TargetFuncSet.insert(F);
        }
    }

    return TargetFuncSet;
}