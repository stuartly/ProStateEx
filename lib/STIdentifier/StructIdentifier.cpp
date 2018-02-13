//===----------------------------------------------------------------------===//
//
// This file implements a versions of the LLVM "GlobalStructIdentifier" pass
//
//===----------------------------------------------------------------------===//


#include "Common/InstructionCommon.h"
#include <STIdentifier/StructIdentifier.h>

#include "Common/BasicBlockCommon.h"

#define _DEBUG 1


static RegisterPass<StructIdentifier> X(
        "identify-struct",
        "Identify data structure of interest",
        false, true);


static cl::opt<bool> boolPrintElements(
        "print-elements",
        cl::desc("Print elements contained in each data structure"),
        cl::init(false),
        cl::value_desc("boolPrintElements"));

char StructIdentifier::ID = 0;

//set<FunStructStore> FunStructSet;    //Pair(funcName,StructName)
set<LogicOp> LogicOpSet;        //LogicOp(Block*,Instruction*)



StructIdentifier::StructIdentifier() : ModulePass(ID) {
    initializePostDominatorTreeWrapperPassPass(*PassRegistry::getPassRegistry());
    //initializeDependenceAnalysisPass(*PassRegistry::getPassRegistry());

}

void StructIdentifier::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesCFG();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequiredTransitive<MemoryDependenceWrapperPass>();
    AU.addRequired<AAResultsWrapperPass>();
    AU.setPreservesAll();
}

bool StructIdentifier::isLogicalOperator(unsigned numOpCode) {
    if (numOpCode == 23) {//Shl
        return true;
    } else if (numOpCode == 24) {//LShr
        return true;
    } else if (numOpCode == 25) {//AShr
        return true;
    } else if (numOpCode == 26) {//And
        return true;
    } else if (numOpCode == 27) {//Or
        return true;
    } else if (numOpCode == 28) {//Xor
        return true;
    }

    return false;
}

set<Instruction *> GetDepInstSet(Function *F, BasicBlock *B, Instruction *I) {
    vector<Instruction *> workList;
    set<Instruction *> InstSet;

    if (I != NULL) {
        workList.push_back(I);
    }

    while (workList.size() != 0) {
        Instruction *Inst = workList.back();
        InstSet.insert(Inst);
        workList.pop_back();
        if (Inst != NULL) {
            for (unsigned i = 0; i < Inst->getNumOperands(); i++) {
                Value *va = Inst->getOperand(i);
                if (va != NULL) {
                    if (User *vaUser = dyn_cast<User>(va)) {
                        if (Instruction *vaInst = dyn_cast<Instruction>(vaUser)) {
                            if (vaInst->getParent() == B) {
                                if (InstSet.find(vaInst) == InstSet.end()) {
                                    workList.push_back(vaInst);
                                }

                            }
                        }
                    }
                }
            }
        }
    }

    return InstSet;
}


bool StructIdentifier::isDataDep_LogicOp(Function *F, BasicBlock *B, Value *V) {

    //Collect Dependence Instruction
    set<Instruction *> InstSet;

    if (V != NULL) {
        if (User *ValueUser = dyn_cast<User>(V)) {
            if (Instruction *ValueInst = dyn_cast<Instruction>(ValueUser)) {
                InstSet = GetDepInstSet(F, B, ValueInst);
            }
        }
    }

    //check if data dependence with logicOp
    for (auto I: InstSet) {
        if (isLogicalOperator(I->getOpcode())) {
            return true;
        }
    }

    return false;

}


bool StructIdentifier::isControlDep_LogicOp(Function *F, BasicBlock *B) {

    ControlDependenceGraphBase CDG;
    PostDominatorTree *PDT = &getAnalysis<PostDominatorTreeWrapperPass>(*F).getPostDomTree();
    CDG.graphForFunction(*F, *PDT);

    for (auto &ele: LogicOpSet) {
        if (CDG.influences(ele.B, B)) {
            return true;
        }
    }

    return false;
}

bool StructIdentifier::isFromConsecutiveMem(Function *F, BasicBlock *B, Value *V) {
    //Collect Dependence Instruction
    set<Instruction *> InstSet;

    if (V != NULL) {
        if (User *ValueUser = dyn_cast<User>(V)) {
            if (Instruction *ValueInst = dyn_cast<Instruction>(ValueUser)) {
                InstSet = GetDepInstSet(F, B, ValueInst);
            }
        }
    }

    //check if data dependence with logicOp
    for (auto &I: InstSet) {
        if (GetElementPtrInst *GI = dyn_cast<GetElementPtrInst>(I)) {
            return true;
        }

//        if (CallInst *Call = dyn_cast<CallInst>(I)) {
//            Function* callee=Call->getCalledFunction();
//            if(callee->getName()=="malloc"){
//                return true;
//            }
//        }
    }

    return false;


}

unsigned getStructFiledNum(Type *st) {

    unsigned fieldNum = 0;

    //return st->getStructNumElements();
    vector<Type *> workList;
    if (st->isStructTy()) {
        workList.push_back(st);
    }

    while (workList.size() != 0) {
        Type *t = workList.back();
        workList.pop_back();
        if (t->isStructTy()) {
            fieldNum += t->getStructNumElements();
            for (unsigned i = 0; i < t->getStructNumElements(); i++) {
                Type *subT = t->getStructElementType(i);
                workList.push_back(subT);

            }
        } else if (t->isArrayTy()) {
            fieldNum += t->getArrayNumElements();
        } else {
            fieldNum++;
        }
    }

    return fieldNum;

}

unsigned getRelatedStoreNum(StructType *st, set<FunStructStore> FunStructStoreSet) {
    unsigned Num = 0;
    for (auto &ele: FunStructStoreSet) {
        if (ele.Struct == st) {
            Num++;
        }
    }
    return Num;
}

unsigned StructIdentifier::getDataDepSotreNum(StructType *st, set<FunStructStore> FunStructStoreSet) {
    unsigned Num = 0;
    for (auto &ele: FunStructStoreSet) {
        if (ele.Struct == st) {
            Function *F = ele.Store->getParent()->getParent();
            BasicBlock *B = ele.Store->getParent();
            Value *V = ele.Store->getValueOperand();
            if (isDataDep_LogicOp(F, B, V)) {
                Num++;
            }
        }
    }
    return Num;
}

bool StructIdentifier::runOnModule(Module &M) {

    set<FunStructStore> FunStructStoreSet;
    for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {
        Function *F = &*FI;

        // collect LogicOpSet
        LogicOpSet.clear();
        for (Function::iterator BI = FI->begin(); BI != FI->end(); BI++) {
            BasicBlock *B = &*BI;
            for (BasicBlock::iterator II = BI->begin(); II != BI->end(); II++) {
                Instruction *I = &*II;

                //if it is a logical instruction, put it into global LogicOpSet
                if (isLogicalOperator(I->getOpcode())) {
                    LogicOp lp;
                    lp.I = I;
                    lp.B = B;
                    LogicOpSet.insert(lp);
                }
            }
        }



        //collect Pair(FunName,StructName)
        for (Function::iterator BI = FI->begin(); BI != FI->end(); BI++) {
            BasicBlock *B = &*BI;
            for (BasicBlock::reverse_iterator II = BI->rbegin(); II != BI->rend(); II++) {
                Instruction *I = &*II;

                if (StoreInst *SI = dyn_cast<StoreInst>(I)) {

                    Value *va = SI->getValueOperand();

                    //filter, if No control-dependence and date dependence with logicInst .
                    if (isControlDep_LogicOp(F, B) == false && isDataDep_LogicOp(F, B, va) == false) {
                        continue;
                    }

                    //filter, if value operand is not from a consecutive memeory.
//                   if(isFromConsecutiveMem(B,va)==false)
//                   {
//                        continue;
//                   }


                    //if the final GetElementPtrInst's getSourceElementType is a struct type
                    Value *Pva = SI->getPointerOperand();

                    vector<Value *> workList;
                    if (Pva != NULL) {
                        workList.push_back(Pva);
                    }


                    Type *type = NULL;
                    while (workList.size() != 0) {
                        Value *va = workList.back();
                        workList.pop_back();
                        if (User *usr = dyn_cast<User>(va)) {
                            if (Instruction *inst = dyn_cast<Instruction>(usr)) {
                                if (GetElementPtrInst *GI = dyn_cast<GetElementPtrInst>(inst)) {
                                    type = GI->getSourceElementType();
                                    Value *subVa = GI->getPointerOperand();
                                    workList.push_back(subVa);
                                } else if (BitCastInst *BI = dyn_cast<BitCastInst>(inst)) {
                                    Value *subVa = BI->getOperand(0);
                                    workList.push_back(subVa);
                                } else {
                                    break;
                                }
                            }
                        }
                    }

                    //check the type whether it is a struct type, if yes, then store it
                    if (type != NULL && type->isStructTy()) {
                        StructType *stype = dyn_cast<StructType>(type);
                        FunStructStore FSS;
                        FSS.Fun = F;
                        FSS.Struct = stype;
                        FSS.Store = SI;
                        FunStructStoreSet.insert(FSS);
                    }

                }
            }
        }
    }



    /*
     * Calculate the ratio of fields having data or control dependence with bitwise
     * operations over the total number of fields.
     */

#if _DEBUG == 1
    set<StructStoreInfo> structSet;
    if (!FunStructStoreSet.empty()) {
        for (auto &ele:FunStructStoreSet) {

            errs() << ele.Fun->getName() << "," << ele.Struct->getName() << "\n";
            errs() << printSrcCodeInfo(ele.Store) << "\n";

            //get the field num
            StructStoreInfo SSI;
            SSI.Struct = ele.Struct;
            if (structSet.find(SSI) != structSet.end()) {
                continue;
            }
            SSI.FieldNum = getStructFiledNum(ele.Struct);
            SSI.RelatedStoreNum = getRelatedStoreNum(ele.Struct, FunStructStoreSet);
            SSI.DateDepStoreNum = getDataDepSotreNum(ele.Struct, FunStructStoreSet);
            structSet.insert(SSI);
        }

    }


    for (auto &ele:structSet) {
        errs() << "Struct: " << ele.Struct->getName()
               << ", FieldNum:  " << ele.FieldNum
               << ", DataDepStoreNum: " << ele.DateDepStoreNum
               << ", TotalStoreNum(ControlDep or DataDep): " << ele.RelatedStoreNum << "\n";
    }
#endif


    /*
     * Calculate the ratio of fields whose values come from a consecutive memory
     * range over the total number of fields.
     */
#if _DEBUG == 1

#endif




    /*
     * check the value operand has data dependence with a getElementPtr instruction
     * output the result
     */

    vector<FunStruct> FunStructVector;
    if (!FunStructStoreSet.empty()) {
        for (auto &ele:FunStructStoreSet) {
            StoreInst *st = ele.Store;
            Value *va = st->getValueOperand();
            // if(isFromConsecutiveMem(ele.Fun,st->getParent(), va)) {

            FunStruct FS;
            FS.Fun = ele.Fun;
            FS.Struct = ele.Struct;
            int Flag = 0;
            for (auto &v:FunStructVector) {
                if (v.Fun == ele.Fun && v.Struct == ele.Struct) {
                    Flag = 1;
                    break;
                }
            }

            if (Flag == 0) {
                FunStructVector.push_back(FS);
            }
            // }
        }
    }



    //output result to struct-of-interest file
    if (!FunStructVector.empty()) {
        //output to file
        std::error_code ErrorInfo;
        llvm::sys::fs::OpenFlags F_None;
        raw_fd_ostream fout("struct-of-interest", ErrorInfo, F_None);
        for (auto &setEle:FunStructVector) {
            fout << "(" << setEle.Fun->getName() << "," << setEle.Struct->getName() << ")\n";

#if _DEBUG == 1
            errs() << "(" << setEle.Fun->getName() << "," << setEle.Struct->getName() << ")\n";
#endif

        }

        fout.close();
    } else {
        errs() << "No Valid Record !\n";
    }

    return false;
}








