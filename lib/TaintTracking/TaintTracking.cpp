#include <fstream>
#include "Common/BasicBlockCommon.h"
#include "TaintTracking/TaintTracking.h"


#define Call_System_IO 1
#define Call_Wrapper_IO 2
#define Debug

static RegisterPass<TaintTracking> X(
        "taint-track",
        "Taint Tracking Pass (with getAnalysisUsage implemented)",
        false,
        true);


static cl::opt<std::string>
        strFileName("strFileName",
                    cl::desc("The name of File to store candidate state variable"), cl::Optional,
                    cl::value_desc("strFileName"),
                    cl::init(""));

//static cl::opt<bool>
//        ProtectedFunc("protected-functions", cl::ZeroOrMore, cl::init(false), cl::Hidden,
//                      cl::desc("Specify list of functions to protect."));

static cl::opt<bool>
        SourceFunc("user-sources", cl::ZeroOrMore, cl::init(false), cl::Hidden,
                   cl::desc("Specify list of functions to treat as taint sourcs."));


char TaintTracking::ID = 0;


void TaintTracking::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

TaintTracking::TaintTracking() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializePostDominatorTreeWrapperPassPass(Registry);
    initializeDominatorTreeWrapperPassPass(Registry);
    initializeLoopInfoWrapperPassPass(Registry);
}


bool TaintTracking::isSource(CallInst *call) {

    string calleeName=call->getCalledFunction()->getName().str();
    if (SourceFunctions.find(calleeName) == SourceFunctions.end())
        return false;
    return true;
}

void TaintTracking::insertParamTaintLoads(Function *F) {


    Function::arg_iterator args = F->arg_begin();
    FunctionToGlobalVarVecIt = FunctionToParamTaintAddr.find(F);
    assert(FunctionToGlobalVarVecIt != FunctionToParamTaintAddr.end() &&
           "Inserting param taint loads for unalloc function!");

    std::vector<GlobalVariable *> *paramTaintAddrs = FunctionToGlobalVarVecIt->second;
    std::vector<GlobalVariable *>::iterator addrIt = paramTaintAddrs->begin();

    for (int i = 0; i < F->arg_size(); i++) {

        Value *arg = args++;
        GlobalVariable *loadAddr = *addrIt++;

        LoadInst *paramTaintLoad = new LoadInst(loadAddr,
                                                "param_taint_load",
                                                &F->getEntryBlock().front());
        RegToTaintVal[arg] = paramTaintLoad;
    }

}


void TaintTracking::allocFuncParamTaint(Module &M) {

    for (std::set<Function *>::iterator it = DefinedFunctions.begin();
         it != DefinedFunctions.end(); it++) {

        Function *f = *it;

        bool isMain = false;
        if (f->getName() == "main") isMain = true;

        // For each argument for Function f, we need to allocate a bit on the
        // stack to store the argument's taint value.
        std::vector<GlobalVariable *> *paramTaintAddrs =
                new std::vector<GlobalVariable *>();
        for (unsigned int i = 0; i < f->arg_size(); i++) {

            GlobalVariable *pTaint = new GlobalVariable(M,
                                                        TaintIntType,
                                                        false,
                                                        GlobalValue::ExternalLinkage,
                                                        0,
                                                        "param_taint");

            ConstantInt *paramTaintVal = ConstantInt::get(M.getContext(),
                                                          APInt(1, StringRef("0"), 10));

            // Taint program argument values.
            if (isMain)
                paramTaintVal = ConstantInt::get(M.getContext(),
                                                 APInt(1, StringRef("1"), 10));

            pTaint->setInitializer(paramTaintVal);
            paramTaintAddrs->push_back(pTaint);
        }

        FunctionToParamTaintAddr[f] = paramTaintAddrs;
    }

}


void TaintTracking::allocFuncTaintReturn(Module &M) {

    for (std::set<Function *>::iterator it = DefinedFunctions.begin();
         it != DefinedFunctions.end(); it++) {

        Function *f = *it;

        GlobalVariable *funcTaintStore = new GlobalVariable(M,
                                                            TaintIntType,
                                                            false,
                                                            GlobalValue::ExternalLinkage,
                                                            0,
                                                            "return_taint");

        ConstantInt *untaintedVal = ConstantInt::get(M.getContext(),
                                                     APInt(1, StringRef("0"), 10));

        funcTaintStore->setInitializer(untaintedVal);

        FunctionToReturnTaintAddr[f] = funcTaintStore;
    }
}


// Function to find all functions that are actually defined in the source
// code we are instrumenting.
void TaintTracking::findDefinedFunctions(Module &M) {

    for (Module::iterator m = M.begin(); m != M.end(); m++) {
        Function *F = &*m;
        bool foundInst = true;
        if (F->begin() != F->end())
            DefinedFunctions.insert(F);
    }
}

// Returns the taint value for a register (if it has one), or the
// constant '0' otherwise.
Value *TaintTracking::getRegOpTaintVal(Value *regOp) {

    Value *regOpTaintVal;

    ValToValIt = RegToTaintVal.find(regOp);
    if (ValToValIt != RegToTaintVal.end()) {
        regOpTaintVal = ValToValIt->second;
    } else {
        // The regOperand for our current instruction isn't in the taint map.
        // Therefore, it has never been seen before (and can't possible be
        // tainted). It could also be something like a constant that also
        // won't be tainted.
        regOpTaintVal = ConstantInt::get(TaintIntType, 0, false);
    }

    return regOpTaintVal;
}

void TaintTracking::storeRegTaint(Value *reg, Value *taintVal) {

    RegToTaintVal[reg] = taintVal;
    ValToValIt = ArrIdxPtrs.find(reg);
    if (ValToValIt != ArrIdxPtrs.end()) {
        // If reg is an index into an array, taint the array.

        Value *arr = ValToValIt->second;
        RegToTaintVal[arr] = taintVal;
    }
}


// Finds the memory address that holds the taint value for the
// data stored at memAddr. Returns NULL if memAddr has no
// associated taint value in memory.
Value *TaintTracking::getTaintAddrForMemAddr(Value *memAddr) {

    ValToValIt = AddrToTaintAddr.find(memAddr);
    if (ValToValIt == AddrToTaintAddr.end()) {
        return NULL;
    }

    return ValToValIt->second;
}


// If we have seen this memory address before, its taint value
// will be a load from the taint address for this instruction.
// Otherwise, its taint is simple '0'.
Value *TaintTracking::getMemOpTaintVal(Value *addrOp, Instruction &I) {

    Value *taintValAddr = getTaintAddrForMemAddr(addrOp);
    if (taintValAddr == NULL)
        return ConstantInt::get(TaintIntType, 0, false);

    // Create a load instruction to load taint value from memory.
    LoadInst *loadInst = new LoadInst(taintValAddr, "taint_load", &I);
    return loadInst;
}

void TaintTracking::visitLoadInst(LoadInst &I) {

    // Load the taint value for the load's address operand.
    Value *opTaintVal = getMemOpTaintVal(I.getPointerOperand(), I);

    BinaryOperator *orInst = BinaryOperator::Create(Instruction::Or,
                                                    opTaintVal,
                                                    ConstantInt::get(TaintIntType, 0, false),
                                                    "loadT",
                                                    &I);

    //RegToTaintVal[&I] = orInst;
    storeRegTaint(&I, orInst);
}


void TaintTracking::visitStoreInst(StoreInst &I) {

    // Allocate space on the stack to store the source reg's
    // taint value so we can later load it.
    Value *taintStoreAddr =
            getTaintAddrForMemAddr(I.getPointerOperand());

    if (taintStoreAddr == NULL) {
        // Don't have a taint value in memory for this address.
        // Need to allocate memory to store it now.
        taintStoreAddr = new AllocaInst(TaintIntType,
                                        0,
                                        "storeTa",
                                        I.getParent()->getParent()->getEntryBlock().getFirstNonPHI());
        AddrToTaintAddr[I.getPointerOperand()] = taintStoreAddr;
    }

    Value *regTaintVal = getRegOpTaintVal(I.getValueOperand());

    StoreInst *taintStore = new StoreInst(regTaintVal,
                                          taintStoreAddr,
                                          false,
                                          &I);

}


void TaintTracking::visitPHINode(PHINode &I) {

    int numPhiVals = I.getNumIncomingValues();
    PHINode *taintPHI = PHINode::Create(TaintIntType,
                                        I.getNumIncomingValues(),
                                        "taintPHI",
                                        &I);

    for (int i = 0; i < numPhiVals; i++) {
        Value *phiV = I.getIncomingValue(i);
        BasicBlock *srcBB = I.getIncomingBlock(i);
        Value *taintVal = getRegOpTaintVal(phiV);
        taintPHI->addIncoming(taintVal, srcBB);
    }

    //RegToTaintVal[&I] = taintPHI;
    storeRegTaint(&I, taintPHI);
}


void TaintTracking::visitUnaryInstruction(UnaryInstruction &I) {

    Value *opA = I.getOperand(0);
    Value *opTaint = getRegOpTaintVal(opA);

    BinaryOperator *orInst = BinaryOperator::Create(Instruction::Or,
                                                    opTaint,
                                                    ConstantInt::get(TaintIntType, 0, false),
                                                    "unaryT",
                                                    &I);

    //RegToTaintVal[&I] = orInst;
    storeRegTaint(&I, orInst);
}


void TaintTracking::visitCmpInst(CmpInst &I) {
    binaryOp(I);
}


void TaintTracking::visitBinaryOperator(BinaryOperator &I) {
    binaryOp(I);
}

void TaintTracking::binaryOp(Instruction &I) {

    Value *opA = I.getOperand(0);
    Value *opB = I.getOperand(1);

    // Look up where the taint values for these two Value* objects
    // are stored.
    Value *opATaint = getRegOpTaintVal(opA);
    Value *opBTaint = getRegOpTaintVal(opB);

    assert(opATaint->getType() == opBTaint->getType() &&
           "Ops to OR instruction do not match!");

    // Create OR instruction between two taint values.
    BinaryOperator *orInst = BinaryOperator::Create(Instruction::Or,
                                                    opATaint,
                                                    opBTaint,
                                                    "binT",
                                                    &I);

    //RegToTaintVal[&I] = orInst;
    storeRegTaint(&I, orInst);
}


void TaintTracking::visitGetElementPtrInst(GetElementPtrInst &I) {

    // Add entry for index to main array pointer.
    Value *arrPtr = I.getPointerOperand();
    ArrIdxPtrs[&I] = arrPtr;

    // Taint value of this pointer is equal to the taint value of
    // the array being indexed.
    Value *arrTaint = getRegOpTaintVal(arrPtr);
    storeRegTaint(&I, arrTaint);

    Value *taintStoreAddr = getTaintAddrForMemAddr(&I);
    if (taintStoreAddr == NULL) {
        taintStoreAddr = new AllocaInst(TaintIntType,
                                        0,
                                        "storeTb",
                                        &I);
        AddrToTaintAddr[&I] = taintStoreAddr;
    }

    StoreInst *taintStore = new StoreInst(arrTaint,
                                          taintStoreAddr,
                                          false,
                                          &I);
}


// For now, since we only have main(), if we return a tainted
// value, print out an error message.
void TaintTracking::visitReturnInst(ReturnInst &I) {

    BasicBlock *orig = I.getParent();
    Function *f = orig->getParent();

    /* Uncomment this section to treat the return from main() as a sink.
    if(orig->getParent()->getName() == "main") {
        // Returning from main is a sink, so handle that differently.
        handleReturnFromMain(I);
        return;
    }
    */

    // Store the return value's taint value at the correct GlobalVar
    // address for this instruction's function.

    Value *retVal = I.getReturnValue();
    Value *retTaint = getRegOpTaintVal(retVal);

    FunctionToGlobalVarIt = FunctionToReturnTaintAddr.find(f);

    assert(FunctionToGlobalVarIt != FunctionToReturnTaintAddr.end() &&
           "Function doesn't have AllocaInst for it's taint return!");

    GlobalVariable *taintAddr = FunctionToGlobalVarIt->second;

    StoreInst *taintStore = new StoreInst(retTaint,
                                          taintAddr,
                                          false,
                                          &I);
}


void TaintTracking::visitCallInst(CallInst &I) {

    // Add instructions to store the taint values of the function parameters.
    // (Only if it is a function internally defined).
    Function *calledFunc = I.getCalledFunction();
    if (calledFunc == NULL) return;

//    if (ProtectedFunctions.find(calledFunc->getName()) != ProtectedFunctions.end()) {
//        insertFuncProtector(I);
//    }
    if (DefinedFunctions.find(calledFunc) != DefinedFunctions.end()) {
        insertParamTaintStore(I);
        insertReturnTaintLoad(I);
    } else if (calledFunc->getName() == "__isoc99_fscanf") {
        // fscanf is our source; need to taint the pointer address that is used to
        // store the input read from the specified file stream.

        int numParams = I.getNumArgOperands();

        // arg 1 = filestream, arg 2 = format string, arg 3+ = data
        for (int i = 2; i < numParams; i++) {
            Value *dataPtr = I.getOperand(i);

            // Taint register value
            //RegToTaintVal[dataPtr] =  ConstantInt::get(TaintIntType,1,false);
            storeRegTaint(dataPtr, ConstantInt::get(TaintIntType, 1, false));

            // Store taint value into memory
            Value *taintStoreAddr = getTaintAddrForMemAddr(dataPtr);
            if (taintStoreAddr == NULL) {
                taintStoreAddr = new AllocaInst(TaintIntType, 0,
                                                "storeTc",
                                                &I);

                AddrToTaintAddr[dataPtr] = taintStoreAddr;
            }

            StoreInst *taintStore = new StoreInst(ConstantInt::get(TaintIntType, 1, false),
                                                  taintStoreAddr,
                                                  false,
                                                  &I);
        }
    } else if (SourceFunctions.find(calledFunc->getName()) != SourceFunctions.end()) {
        insertReturnTaintLoad(I);
    }
}


void TaintTracking::insertReturnTaintLoad(CallInst &I) {

    Function *calledFunc = I.getCalledFunction();

    // We need to insert the load after the function call returns, so the
    // function can store the return taint value before we load it.


    Value *taintVal;

    if (DefinedFunctions.find(calledFunc) != DefinedFunctions.end()) {
        FunctionToGlobalVarIt = FunctionToReturnTaintAddr.find(calledFunc);
        assert(FunctionToGlobalVarIt != FunctionToReturnTaintAddr.end() &&
               "Function doesn't have AllocaInst for it's taint return!");

        GlobalVariable *taintAddr = FunctionToGlobalVarIt->second;
        taintVal = new LoadInst(taintAddr, "retT_load", I.getNextNode());

    } else {
        // Taint values coming back from external function calls.
        // If function is a source, Taint = 1
        //                    else, Taint = 0
        if (SourceFunctions.find(calledFunc->getName()) != SourceFunctions.end())
            taintVal = ConstantInt::get(TaintIntType, 1, false);
        else
            taintVal = ConstantInt::get(TaintIntType, 0, false);
    }

    //RegToTaintVal[&I] = taintVal;
    storeRegTaint(&I, taintVal);
}


void TaintTracking::insertParamTaintStore(CallInst &I) {

    Function *calledFunc = I.getCalledFunction();
    FunctionToGlobalVarVecIt = FunctionToParamTaintAddr.find(calledFunc);

    assert(FunctionToGlobalVarVecIt != FunctionToParamTaintAddr.end() &&
           "CallInst to function with no param taint memory allocated!");

    std::vector<GlobalVariable *> *paramTaintAddrs = FunctionToGlobalVarVecIt->second;
//    assert(paramTaintAddrs->size() == I.getNumArgOperands()-1 &&
//           "Inconsistent number of parameters for CallInst!");

    for (unsigned int i = 0; i < paramTaintAddrs->size(); i++) {

        GlobalVariable *tAddr = paramTaintAddrs->at(i);
        Value *arg = I.getArgOperand(i);

        Value *taint = getRegOpTaintVal(arg);
        StoreInst *paramTaintStore = new StoreInst(taint,
                                                   tAddr,
                                                   false,
                                                   &I);
    }
}


void TaintTracking::visitBranchInst(BranchInst &I) {

    /*
    if (I.isUnconditional())
        return;

    // If branch condition is tainted, branch to abort BB.
    BinaryOperator *orInst;

    if (I.getNumSuccessors() == 2) {
        Value *opATaint = getRegOpTaintVal(I.getSuccessor(0));
        Value *opBTaint = getRegOpTaintVal(I.getSuccessor(1));

        if (opATaint == ConstantInt::get(TaintIntType, 0, false) &&
            opBTaint == ConstantInt::get(TaintIntType, 0, false))
            return;

        orInst = BinaryOperator::Create(Instruction::Or,
                                        opATaint,
                                        opBTaint,
                                        "branch_check",
                                        &I);
    } else {
        Value *opATaint = getRegOpTaintVal(I.getSuccessor(0));
        if (opATaint == ConstantInt::get(TaintIntType, 0, false))
            return;


        orInst = BinaryOperator::Create(Instruction::Or,
                                        opATaint,
                                        ConstantInt::get(TaintIntType, 0, false),
                                        "branch_check",
                                        &I);
    }

    BasicBlock *orig = I.getParent();
    BasicBlock *continueBB = SplitBlock(orig, &I, taintPass);
    continueBB->setName("cont_BB");

    BranchInst *abortBr = BranchInst::Create(abortBB,
                                             continueBB,
                                             orInst,
                                             orig->getTerminator());

    orig->getTerminator()->eraseFromParent();

    assert(orig->getTerminator() != NULL &&
           "OrigBB has no terminator!");

    assert(continueBB->getTerminator() != NULL &&
           "ContinueBB has no terminator!");

 */
}


void TaintTracking::visitInstruction(Instruction *Inst) {


    if (CallInst *call = dyn_cast<CallInst>(Inst)) {
        visitCallInst(*call);
    }


    if (AtomicCmpXchgInst *atomCmpchg = dyn_cast<AtomicCmpXchgInst>(Inst)) {

    }


    if (AtomicRMWInst *atomRMW = dyn_cast<AtomicRMWInst>(Inst)) {

    }


    if (BinaryOperator *binOP = dyn_cast<BinaryOperator>(Inst)) {
        visitBinaryOperator(*binOP);
    }


    if (CmpInst *cmp = dyn_cast<CmpInst>(Inst)) {
        visitCmpInst(*cmp);
    }

    if (ExtractElementInst *extrEle = dyn_cast<ExtractElementInst>(Inst)) {

    }


    if (FenceInst *fence = dyn_cast<FenceInst>(Inst)) {

    }


    //FuncletPadInst, including CleanupPadInst, CatchPadInst
    if (CleanupPadInst *clrPad = dyn_cast<CleanupPadInst>(Inst)) {

    }


    if (CatchPadInst *catchPad = dyn_cast<CatchPadInst>(Inst)) {

    }


    if (GetElementPtrInst *getPtr = dyn_cast<GetElementPtrInst>(Inst)) {
        visitGetElementPtrInst(*getPtr);
    }


    if (InsertElementInst *insertEle = dyn_cast<InsertElementInst>(Inst)) {

    }

    if (InsertValueInst *insertVa = dyn_cast<InsertValueInst>(Inst)) {

    }


    if (LandingPadInst *landPad = dyn_cast<LandingPadInst>(Inst)) {

    }


    if (PHINode *phi = dyn_cast<PHINode>(Inst)) {
        visitPHINode(*phi);
    }

    if (SelectInst *select = dyn_cast<SelectInst>(Inst)) {

    }


    if (ShuffleVectorInst *shuff = dyn_cast<ShuffleVectorInst>(Inst)) {

    }


    if (StoreInst *store = dyn_cast<StoreInst>(Inst)) {

        visitStoreInst(*store);
    }


    //UnaryInstruction
    if (AllocaInst *alloc = dyn_cast<AllocaInst>(Inst)) {

    }


    if (LoadInst *load = dyn_cast<LoadInst>(Inst)) {
        visitLoadInst(*load);
    }

    //ExtractValueInst
    if (ExtractValueInst *extrValue = dyn_cast<ExtractValueInst>(Inst)) {

    }


    if (VAArgInst *VAA = dyn_cast<VAArgInst>(Inst)) {

    }


    if (CastInst *cast = dyn_cast<CastInst>(Inst)) {

    }


    //TerminatorInst in the following:

    if (ReturnInst *ret = dyn_cast<ReturnInst>(Inst)) {
        visitReturnInst(*ret);
    }

    if (InvokeInst *invoke = dyn_cast<InvokeInst>(Inst)) {

    }

    if (ResumeInst *resume = dyn_cast<ResumeInst>(Inst)) {

    }

    if (SwitchInst *swit = dyn_cast<SwitchInst>(Inst)) {

    }

    if (UnreachableInst *unreach = dyn_cast<UnreachableInst>(Inst)) {

    }

    if (BranchInst *br = dyn_cast<BranchInst>(Inst)) {
        visitBranchInst(*br);
    }

    if (CatchReturnInst *catchRet = dyn_cast<CatchReturnInst>(Inst)) {

    }

    if (CatchSwitchInst *catchSwit = dyn_cast<CatchSwitchInst>(Inst)) {

    }

    if (CleanupReturnInst *cleanRet = dyn_cast<CleanupReturnInst>(Inst)) {

    }

    if (IndirectBrInst *indirectRr = dyn_cast<IndirectBrInst>(Inst)) {

    }


}


// Function to create a BasicBlock that is used to warn user when a
// tainted value is being used for output, then exits the program.
void TaintTracking::createAbortBB(Module &M, Function &F) {

    // This will be the block that is branched to if a taint value
    // is being used for some type of output.
    abortBB = BasicBlock::Create(F.getContext(),
                                 "abortBB",
                                 &F,
                                 NULL);


    /* TODO: seems like exit should take 1 parameter, but it wants none.
     * Figure out what's going on and make sure we are calling it correctly.
     Function::ArgumentListType* argList = &exitFunc->getArgumentList();
     errs() << "Num params for exit = " << argList->size() << "\n";
     args.push_back(ConstantInt::get(IntegerType::get(M.getContext(),32),
     -1,
     true));
     */

    // Insert a dummy branch to be the terminator of abortBB.
    BranchInst *br = BranchInst::Create(abortBB,abortBB);


    // Call exit
    Function* func_exit = M.getFunction("exit");
    if (!func_exit) {
        std::vector<Type*>FuncTy_args;
        FuncTy_args.push_back(IntegerType::get(M.getContext(), 32));
        FunctionType* FuncTy_3 = FunctionType::get(
                Type::getVoidTy(M.getContext()),
                FuncTy_args,
                false);

        func_exit = Function::Create(FuncTy_3,
                                     GlobalValue::ExternalLinkage,
                                     "exit",
                                     &M);
        func_exit->setCallingConv(CallingConv::C);
    }


    CallInst* callExit = CallInst::Create(func_exit,
                                          ConstantInt::get(M.getContext(), APInt(32, StringRef("1"), 10)),
                                          "",
                                          abortBB->getTerminator());

    assert(abortBB->getTerminator() != NULL &&
           "AbortBB has no terminator!");
}



bool TaintTracking::runOnModule(Module &M) {

    // taintPass = this;

    TaintIntType = IntegerType::get(M.getContext(), 1);

    if (SourceFunc) {
        std::ifstream infile("source_functions.tt");
        std::string fName;
        while (getline(infile, fName))
            SourceFunctions.insert(fName);
    }


    findDefinedFunctions(M);
    allocFuncTaintReturn(M);
    allocFuncParamTaint(M);

    // For each instruction in each basic block of this function,
    // insert taint checking instruction.
    for (Module::iterator m = M.begin(); m != M.end(); m++) {

        Function *F = &*m;
        // We only want to insert the abortBB into functions that are actually
        // defined in the source code we are instrumenting. Since we are iterating
        // over all function calls in the source, only add abortBB to a function
        // once we have seen an instruction for that function.
        bool foundInst = false;
        bool hasSource = false;

        std::vector<Instruction *> funcInstList;

        for (Function::iterator b = F->begin(); b != F->end(); b++) {
            //errs() << "\tfound instruction!\n";
            if (!foundInst) {
                foundInst = true;

                // TODO: only call this if F has a sink.
                createAbortBB(M, *F);
            }

            if (b->getName() == "abortBB") continue;
            for (BasicBlock::iterator i = b->begin(); i != b->end(); i++) {
                Instruction *Inst=&*i;
                if(CallInst* call=dyn_cast<CallInst>(Inst)) {
                    if (isSource(call)) hasSource = true;
                    funcInstList.push_back(Inst);
                }
            }
        }

        if (foundInst)
            insertParamTaintLoads(F);


        for (std::vector<Instruction *>::iterator instIt = funcInstList.begin();
             instIt != funcInstList.end(); instIt++) {
            Instruction *inst = *instIt;
            //errs() << "visitng: " << *inst << "\n";
            visitInstruction(inst);

        }
    }

    return true;
}



