#include <fstream>
#include "Common/BasicBlockCommon.h"
#include "NetworkIO/NetworkIO.h"


#define _DEBUG 1
#define Call_System_IO 1
#define Call_Wrapper_IO 2


static RegisterPass<NetworkIO> X(
        "network-io",
        "identify network io",
        false,
        true);


static cl::opt<std::string> SystemIOPath(
        "SystemIOPath",
        cl::desc("Path of file containing system io"), cl::Optional,
        cl::value_desc("SystemIOPath"));


static cl::opt<std::string> strFileName(
        "strFileName",
        cl::desc("The name of File to store candidate function name"), cl::Optional,
        cl::value_desc("strFileName"),
        cl::init(""));


char NetworkIO::ID = 0;
bool isFirst = true;
std::set<std::string> NetworkIOSet;
std::set<std::string> SystemNetWorkIOSet;


void NetworkIO::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}


NetworkIO::NetworkIO() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializePostDominatorTreeWrapperPassPass(Registry);
    initializeDominatorTreeWrapperPassPass(Registry);
    initializeLoopInfoWrapperPassPass(Registry);
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

void NetworkIO::MarkIOFlagToFunction(Module &M, Function *F) {
    _MarkFlagFunction(M, F, 1, IO_FLAG);
}

void _MarkFlagToInstruction(Module &M, Instruction *I, int val, std::string flag) {
    IntegerType *IntType = IntegerType::get(M.getContext(), 32);
    MDBuilder MDHelper(M.getContext());

    Constant *FlagValue = ConstantInt::get(IntType, val);
    SmallVector<Metadata *, 1> Vals;
    Vals.push_back(MDHelper.createConstant(FlagValue));
    I->setMetadata(flag, MDNode::get(M.getContext(), Vals));
};

void MarkFlagToSystemIOCallInst(Module &M, Instruction *I) {
    _MarkFlagToInstruction(M, I, Call_System_IO, IO_CALL_FLAG);
}

void MarkFlagToWrapperIOCallInst(Module &M, Instruction *I) {
    _MarkFlagToInstruction(M, I, Call_Wrapper_IO, IO_CALL_FLAG);
};


std::set<std::string> getSystemIOSet(const string &SystemIOPath) {
    std::set<std::string> system_io_set;
    ifstream file(SystemIOPath);

    if (!file) {
        errs() << "Invalid system io path\n";
    } else {
        string line;
        while (getline(file, line)) {
            system_io_set.insert(line);
        }
    }

    file.close();
    return system_io_set;
}


bool isNetworkIO(string FunName) {

    for (auto &ele: SystemNetWorkIOSet) {
        if (ele == FunName) {
            return true;
        }
    }

    for (auto &ele: NetworkIOSet) {
        if (ele == FunName) {
            return true;
        }
    }

    return false;
}


bool CallNetworkIO(Module &M, Function *Func) {
    vector<Function *> workList;
    set<Function *> AnalyzedFuns;

    if (Func != NULL) {
        workList.push_back(Func);
    }

    while (workList.size() != 0) {

        Function *Fun = workList.back();
        AnalyzedFuns.insert(Fun);
        workList.pop_back();

        if (Fun != NULL) {
            for (Function::iterator BI = Fun->begin(); BI != Fun->end(); BI++) {
                if (BasicBlock *BB = &*BI) {
                    for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {
                        if (Instruction *Inst = &*II) {
                            if (CallInst *FunCallInst = dyn_cast<CallInst>(Inst)) {
                                if (Function *Callee = FunCallInst->getCalledFunction()) {
                                    if (Callee != NULL) {
                                        if (isNetworkIO(Callee->getName())) {
                                            if (isFirst == true) {
                                                MarkFlagToSystemIOCallInst(M, FunCallInst);
                                            } else {
                                                MarkFlagToWrapperIOCallInst(M, FunCallInst);
                                            }
                                            return true;
                                        } else if (AnalyzedFuns.find(Callee) == AnalyzedFuns.end()) {
                                            workList.push_back(Callee);
                                        }
                                    }
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

/*globale varialbe*/
set<Value *> ValueSite;
set<pair<string, unsigned >> StructSite;
set<pair<string, unsigned>> StructInstanceSet;


void CollectStoreIO(Module &M) {
    //check all the functions
    for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {
        Function *Func = &*FI;
        //find if store network IO to a function pointer location
        for (Function::iterator BI = Func->begin(); BI != Func->end(); BI++) {
            if (BasicBlock *BB = &*BI) {
                for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {
                    if (Instruction *Inst = &*II) {
                        if (StoreInst *store = dyn_cast<StoreInst>(Inst)) {

                            Value *storeLeft = store->getPointerOperand();
                            Value *storeRight = store->getValueOperand();

                            if (storeRight != NULL) {

                                Type *tp = storeRight->getType();
                                while (tp->isPointerTy()) {
                                    tp = tp->getContainedType(0);
                                }

                                if (tp->isFunctionTy()) {
                                    if (isNetworkIO(storeRight->getName())) {
                                        //using network io to assignment, then record the set of storeLeft
                                        if (User *vaUser = dyn_cast<User>(storeLeft)) {
                                            if (Instruction *vaInst = dyn_cast<Instruction>(vaUser)) {

                                                //case 1,  compare_value
                                                if (AllocaInst *alloc = dyn_cast<AllocaInst>(storeLeft)) {

                                                    ValueSite.insert(storeLeft);

                                                } else if (GetElementPtrInst *gt = dyn_cast<GetElementPtrInst>(
                                                        storeLeft)) {
                                                    //case2: structure.mem
                                                    Type *tp = gt->getSourceElementType();
                                                    if (tp->isStructTy()) {
                                                        StructType *stp = dyn_cast<StructType>(tp);
                                                        if (stp->isLiteral() == false) {
                                                            string structName = tp->getStructName().str();

                                                            if (ConstantInt *i = dyn_cast<ConstantInt>(
                                                                    gt->getOperand(2))) {
                                                                unsigned offset = i->getZExtValue();
                                                                pair<string, unsigned> pairTemp;
                                                                pairTemp.first = structName;
                                                                pairTemp.second = offset;
                                                                StructSite.insert(pairTemp);


                                                                //handle the situation of structName.123, structName
                                                                string theEnd = structName.substr(
                                                                        structName.find_last_of(".") + 1,
                                                                        structName.length());
                                                                int endInt = -1;
                                                                try {
                                                                    endInt = stoi(theEnd);
                                                                } catch (exception) {

                                                                }

                                                                if (i >= 0) {
                                                                    structName = structName.substr(0,
                                                                                                   structName.find_last_of(
                                                                                                           "."));
                                                                    pair<string, unsigned> pairTemp_same_sturct;
                                                                    pairTemp_same_sturct.first = structName;
                                                                    pairTemp_same_sturct.second = offset;
                                                                    StructSite.insert(pairTemp_same_sturct);
                                                                }
                                                            }


                                                        } else {

                                                            errs() << "isLiteral\n";

                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                } else if (tp->isStructTy()) {

                                    StructType *stp = dyn_cast<StructType>(tp);

                                    if (stp->isLiteral() == false) {

                                        if (User *vaUser = dyn_cast<User>(storeRight)) {
                                            //vaUser->dump();
                                            if (BitCastInst *bt = dyn_cast<BitCastInst>(vaUser)) {
                                                //ignore bitcast inst
                                                // bt->dump();

                                            } else if (ConstantExpr *ci = dyn_cast<ConstantExpr>(vaUser)) {
                                                string instanceName = ci->getOperand(0)->getName().str();

                                                bool find = false;
                                                pair<string, unsigned> pairFound;
                                                for (auto &ele:StructInstanceSet) {
                                                    if (ele.first == instanceName) {
                                                        find = true;
                                                        pairFound = ele;
                                                    }
                                                }

                                                if (find == true) {    //
                                                    //obtain the struct name.
                                                    string structName = tp->getStructName().str();
                                                    //errs()<< "using instance assign: "<< structName<<"\n";
                                                    unsigned offset = pairFound.second;
                                                    pair<string, unsigned> pairTemp;
                                                    pairTemp.first = structName;
                                                    pairTemp.second = offset;
                                                    StructSite.insert(pairTemp);

                                                    //handle the situation of structName.123, structName
                                                    string theEnd = structName.substr(
                                                            structName.find_last_of(".") + 1,
                                                            structName.length());
                                                    int endInt = -1;
                                                    try {
                                                        endInt = stoi(theEnd);
                                                    } catch (exception) {

                                                    }

                                                    if (endInt >= 0) {
                                                        structName = structName.substr(0,
                                                                                       structName.find_last_of(
                                                                                               "."));
                                                        pair<string, unsigned> pairTemp_same_sturct;
                                                        pairTemp_same_sturct.first = structName;
                                                        pairTemp_same_sturct.second = offset;
                                                        StructSite.insert(pairTemp_same_sturct);
                                                    }

                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        } else if (MemCpyInst *memcpy = dyn_cast<MemCpyInst>(Inst)) {
                            // not need any more
                        }
                    }
                }
            }
        }
    }
}


bool isFunPtrToIO(Module &M, Function *Func, CallInst *callfp) {

    if (callfp != NULL) {

        Value *ld_value = callfp->getCalledValue();

        if (ld_value != NULL) {
            if (User *vaUser = dyn_cast<User>(ld_value)) {
                if (Instruction *vaInst = dyn_cast<Instruction>(vaUser)) {
                    if (LoadInst *ld = dyn_cast<LoadInst>(vaInst)) {
                        Value *va = ld->getPointerOperand();
                        if (User *vaUser = dyn_cast<User>(va)) {
                            if (Instruction *vaInst = dyn_cast<Instruction>(vaUser)) {
                                if (AllocaInst *alloc = dyn_cast<AllocaInst>(vaInst)) {
                                    //case1:
                                    for (auto &ele:ValueSite) {
                                        if (ele == va) {
                                            return true;
                                        }
                                    }
                                } else if (GetElementPtrInst *gt = dyn_cast<GetElementPtrInst>(vaInst)) {
                                    //case2:
                                    Type *tp = gt->getSourceElementType();
                                    if (tp->isPointerTy()) {
                                        tp = tp->getContainedType(0);
                                    }

                                    if (tp->isStructTy()) {
                                        StructType *stp = dyn_cast<StructType>(tp);

                                        if (stp->isLiteral() == false) {
                                            string structName = tp->getStructName().str();

                                            //errs()<<"Load from: "<< structName<<"\n";
                                            unsigned offset;

                                            if (ConstantInt *i = dyn_cast<ConstantInt>(gt->getOperand(2))) {
                                                offset = i->getZExtValue();
                                            }

                                            pair<string, unsigned> pairTemp;
                                            pairTemp.first = structName;
                                            pairTemp.second = offset;

                                            for (auto &ele:StructSite) {
                                                if (pairTemp == ele) {
                                                    return true;
                                                }
                                            }


                                            //handle the situation of structName.123, structName
                                            string theEnd = structName.substr(
                                                    structName.find_last_of(".") + 1,
                                                    structName.length());
                                            int endInt = -1;
                                            try {
                                                endInt = stoi(theEnd);
                                            } catch (exception) {

                                            }

                                            if (endInt >= 0) {
                                                structName = structName.substr(0,
                                                                               structName.find_last_of(
                                                                                       "."));
                                                pair<string, unsigned> pairTemp_same_sturct;
                                                pairTemp_same_sturct.first = structName;
                                                pairTemp_same_sturct.second = offset;

                                                for (auto &ele:StructSite) {
                                                    if (pairTemp_same_sturct == ele) {
                                                        return true;
                                                    }
                                                }
                                            }


                                        } else {


                                            errs() << "assign using constant \n";
                                            gt->dump();


                                        }
                                    }
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


bool useFunPtrToIO(Module &M, Function *Func) {

    if (Func != NULL) {
        for (Function::iterator BI = Func->begin(); BI != Func->end(); BI++) {
            if (BasicBlock *BB = &*BI) {
                for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {
                    if (Instruction *Inst = &*II) {
                        if (CallInst *FunCallInst = dyn_cast<CallInst>(Inst)) {
                            CallSite ci(FunCallInst);
                            Function *Callee = dyn_cast<Function>(ci.getCalledValue()->stripPointerCasts());
                            if (!Callee) {
                                if (isFunPtrToIO(M, Func, FunCallInst)) {
                                    if (isFirst == true) {
                                        MarkFlagToSystemIOCallInst(M, FunCallInst);
                                    } else {
                                        MarkFlagToWrapperIOCallInst(M, FunCallInst);
                                    }
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


void upDateStructIOSet(Module &M) {

    for (auto &Global : M.getGlobalList()) {
        Type *type = Global.getValueType();

        if (type->isStructTy()) {
            //Global.dump();
            string instanceName = Global.getName().str();
            if (Global.getNumOperands() < 1) {
                continue;
            }
            Value *va = Global.getOperand(0);
            if (User *usr = dyn_cast<User>(va)) {
                //traverse all the element of struct
                for (unsigned i = 0; i < usr->getNumOperands(); i++) {
                    Value *ele_va = usr->getOperand(i);
                    Type *ele_type = ele_va->getType();

                    if (ele_type->isPointerTy()) {
                        while (ele_type->isPointerTy()) {
                            ele_type = ele_type->getContainedType(0);
                        }
                        if (ele_type->isFunctionTy()) {
                            string funName = ele_va->getName().str();
                            if (isNetworkIO(funName)) {

                                Type *tp = va->getType();
                                StructType *stp = dyn_cast<StructType>(tp);

                                if (stp->isLiteral() == false) {
                                    // //@instantName = constant structName { i32, i32
                                    string structName = tp->getStructName().str();
                                    unsigned offset = i;
                                    pair<string, unsigned> pairTemp;
                                    pairTemp.first = structName;
                                    pairTemp.second = offset;
                                    StructSite.insert(pairTemp);

                                    pair<string, unsigned> pairTemp2;
                                    pairTemp2.first = instanceName;
                                    pairTemp2.second = offset;
                                    StructInstanceSet.insert(pairTemp2);

                                } else {

                                    //@instantName = constant { i32, i32,

                                    unsigned offset = i;
                                    pair<string, unsigned> pairTemp;
                                    pairTemp.first = instanceName;
                                    pairTemp.second = offset;

                                    StructInstanceSet.insert(pairTemp);
                                }
                            }
                        }
                    }
                }
            }
        }

    }
}


bool NetworkIO::runOnModule(Module &M) {

    if (strFileName.empty()) {
        errs() << "Please set output file path and name." << "\n";
        return false;
    }

    //load system io from config file
    SystemNetWorkIOSet = getSystemIOSet(SystemIOPath);
    unsigned set_size1, set_size2;

    ValueSite.clear();
    StructSite.clear();

    do {
        //update NetworkIO set based system io in config file (first)
        for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {
            Function *F = &*FI;
            if (CallNetworkIO(M, F) == true) {
                NetworkIOSet.insert(F->getName().str());
                MarkIOFlagToFunction(M, F);
            }
        }

        set_size1 = NetworkIOSet.size();

        upDateStructIOSet(M); //handle all global struct instance
        CollectStoreIO(M);    //handle all the storeInst

        //handle function pointer and update NetworkIO set.
        for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {
            Function *F = &*FI;

            if (useFunPtrToIO(M, F) == true) {
                NetworkIOSet.insert(F->getName().str());
                MarkIOFlagToFunction(M, F);
            }
        }

        isFirst = false;
        upDateStructIOSet(M);
        CollectStoreIO(M); //handle all the storeInst


        for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {
            Function *F = &*FI;

            if (CallNetworkIO(M, F) == true) {
                NetworkIOSet.insert(F->getName().str());
                MarkIOFlagToFunction(M, F);
            }
        }

        upDateStructIOSet(M);
        CollectStoreIO(M); //handle all the storeInst

        //handle function pointer and update NetworkIO set.
        for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {
            Function *F = &*FI;

            if (useFunPtrToIO(M, F) == true) {
                NetworkIOSet.insert(F->getName().str());
                MarkIOFlagToFunction(M, F);
            }
        }


        set_size2 = NetworkIOSet.size();

    } while (set_size1 != set_size2);



    //ouput to file
    ofstream f_out;
    f_out.open(strFileName, std::ofstream::out | std::ofstream::app);

    for (auto &ele: NetworkIOSet) {
        //errs() << ele << "\n";
        f_out << ele << "\n";
    }

    f_out.close();
    return false;

}
