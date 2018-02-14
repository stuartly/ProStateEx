#include <fstream>
#include "Common/BasicBlockCommon.h"
#include "DynTaintAnalyzer/DynTaintAnalyzer.h"


#define Call_System_IO 1
#define Call_Wrapper_IO 2
#define Debug

static RegisterPass<DynTaintAnalyzer> X(
        "analysis-taint-var",
        "analysis taint variable",
        false,
        true);


static cl::opt<std::string> NetworkIOPath("NetworkIOPath",
                                          cl::desc("Path of file containing network io function and taint argument"),
                                          cl::Optional,
                                          cl::value_desc("NetworkIOPath"),
                                          cl::init(""));

static cl::opt<std::string> strFileName("strFileName",
                                        cl::desc("The name of File to store candidate state variable"), cl::Optional,
                                        cl::value_desc("strFileName"),
                                        cl::init(""));


char DynTaintAnalyzer::ID = 0;

void DynTaintAnalyzer::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

DynTaintAnalyzer::DynTaintAnalyzer() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializePostDominatorTreeWrapperPassPass(Registry);
    initializeDominatorTreeWrapperPassPass(Registry);
    initializeLoopInfoWrapperPassPass(Registry);
}

int DynTaintAnalyzer::printSrcCodeInfo(Instruction *pInst) {
    const DILocation *DIL = pInst->getDebugLoc();

    if (!DIL) {
        //errs()<< "failed to get DIL\n";
        return -1;
    }

    char pPath[200];

    string sFileName = DIL->getFilename().str();
    realpath(sFileName.c_str(), pPath);
    sFileName = string(sFileName);
    unsigned int numLine = DIL->getLine();
    errs() << "         " << sFileName << ": " << numLine << "\n";

    return 1;

}

void DynTaintAnalyzer::Init_Instrument_Type() {

    this->VoidType = Type::getVoidTy(this->pModule->getContext());
    this->IntType = IntegerType::get(this->pModule->getContext(), 32);
    this->LongType = IntegerType::get(this->pModule->getContext(), 64);
    this->arrayType = ArrayType::get(IntegerType::get(this->pModule->getContext(), 32), 10);
    this->VoidPointerType = PointerType::get(IntegerType::get(this->pModule->getContext(), 8), 0);

}

void DynTaintAnalyzer::Init_Instrument_Var() {

    this->gvar_int32_global_arg = new GlobalVariable(/*Module=*/*this->pModule,
            /*Type=*/this->IntType,
            /*isConstant=*/false,
            /*Linkage=*/GlobalValue::CommonLinkage,
            /*Initializer=*/0, // has initializer, specified below
            /*Name=*/"global_arg");
    this->gvar_int32_global_arg->setAlignment(4);

    ConstantInt *const_int32_14 = ConstantInt::get(this->pModule->getContext(), APInt(32, StringRef("0"), 10));
    this->gvar_int32_global_arg->setInitializer(const_int32_14);


    this->gvar_array_global_arg = new GlobalVariable(/*Module=*/*this->pModule,
            /*Type=*/this->arrayType,
            /*isConstant=*/false,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Initializer=*/0, // has initializer, specified below
            /*Name=*/"global_arg");
    this->gvar_array_global_arg->setAlignment(16);

    ConstantAggregateZero *const_array_20 = ConstantAggregateZero::get(this->arrayType);
    this->gvar_array_global_arg->setInitializer(const_array_20);


}

void DynTaintAnalyzer::Init_Instrument_Function() {

    //MarkTaintAddr(Addr)
    std::vector<Type *> FuncTy_13_args;
    FuncTy_13_args.push_back(this->VoidPointerType);
    FuncTy_13_args.push_back(IntegerType::get(this->pModule->getContext(), 32));
    FunctionType *FuncTy_13 = FunctionType::get(
            /*Result=*/Type::getVoidTy(this->pModule->getContext()),
            /*Params=*/FuncTy_13_args,
            /*isVarArg=*/false);
    this->func_MarkTaintAddr = Function::Create(
            /*Type=*/FuncTy_13,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Name=*/"MarkTaintAddr", this->pModule);


    //RemoveTaintAddr(Addr)
    std::vector<Type *> FuncTy_14_args;
    FuncTy_14_args.push_back(this->VoidPointerType);
    FuncTy_14_args.push_back(IntegerType::get(this->pModule->getContext(), 32));
    FunctionType *FuncTy_14 = FunctionType::get(
            /*Result=*/Type::getVoidTy(this->pModule->getContext()),
            /*Params=*/FuncTy_14_args,
            /*isVarArg=*/false);
    this->func_RemoveTaintAddr = Function::Create(
            /*Type=*/FuncTy_14,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Name=*/"RemoveTaintAddr", this->pModule);


    //MarkTaint(Inst)
    std::vector<Type *> FuncTy_15_args;
    FuncTy_15_args.push_back(this->IntType);
    FunctionType *FuncTy_15 = FunctionType::get(
            /*Result=*/Type::getVoidTy(this->pModule->getContext()),
            /*Params=*/FuncTy_15_args,
            /*isVarArg=*/false);
    this->func_MarkTaint = Function::Create(
            /*Type=*/FuncTy_15,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Name=*/"MarkTaint", this->pModule);


    //RemoveTaint(Inst)
    std::vector<Type *> FuncTy_16_args;
    FuncTy_16_args.push_back(this->IntType);
    FunctionType *FuncTy_16 = FunctionType::get(
            /*Result=*/Type::getVoidTy(this->pModule->getContext()),
            /*Params=*/FuncTy_16_args,
            /*isVarArg=*/false);
    this->func_RemoveTaint = Function::Create(
            /*Type=*/FuncTy_16,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Name=*/"RemoveTaint", this->pModule);


    //printTaint(Inst)
    std::vector<Type *> FuncTy_17_args;
    FuncTy_17_args.push_back(this->IntType);
    FunctionType *FuncTy_17 = FunctionType::get(
            /*Result=*/Type::getVoidTy(this->pModule->getContext()),
            /*Params=*/FuncTy_17_args,
            /*isVarArg=*/false);
    this->func_printTaint = Function::Create(
            /*Type=*/FuncTy_17,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Name=*/"printTaint", this->pModule);


    //PropagateTaintRegToReg(Inst, Inst)
    std::vector<Type *> FuncTy_18_args;
    FuncTy_18_args.push_back(this->IntType);
    FuncTy_18_args.push_back(this->IntType);
    FunctionType *FuncTy_18 = FunctionType::get(
            /*Result=*/Type::getVoidTy(this->pModule->getContext()),
            /*Params=*/FuncTy_18_args,
            /*isVarArg=*/false);
    this->func_PropagateTaintRegToReg = Function::Create(
            /*Type=*/FuncTy_18,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Name=*/"PropagateTaintRegToReg", this->pModule);

    //PropagateTaintRegToAddr
    std::vector<Type *> FuncTy_19_args;
    FuncTy_19_args.push_back(this->IntType);
    FuncTy_19_args.push_back(this->VoidPointerType);
    FuncTy_19_args.push_back(this->IntType);
    FunctionType *FuncTy_19 = FunctionType::get(
            /*Result=*/Type::getVoidTy(this->pModule->getContext()),
            /*Params=*/FuncTy_19_args,
            /*isVarArg=*/false);
    this->func_PropagateTaintRegToAddr = Function::Create(
            /*Type=*/FuncTy_19,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Name=*/"PropagateTaintRegToAddr", this->pModule);


    //PropagateTaintAddrToAddr
    std::vector<Type *> FuncTy_20_args;
    FuncTy_20_args.push_back(this->VoidPointerType);
    FuncTy_20_args.push_back(this->IntType);
    FuncTy_20_args.push_back(this->VoidPointerType);
    FuncTy_20_args.push_back(this->IntType);
    FunctionType *FuncTy_20 = FunctionType::get(
            /*Result=*/Type::getVoidTy(this->pModule->getContext()),
            /*Params=*/FuncTy_20_args,
            /*isVarArg=*/false);
    this->func_PropagateTaintAddrToAddr = Function::Create(
            /*Type=*/FuncTy_20,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Name=*/"PropagateTaintAddrToAddr", this->pModule);


    //PropagateTaintAddrToReg
    std::vector<Type *> FuncTy_21_args;
    FuncTy_21_args.push_back(this->VoidPointerType);
    FuncTy_21_args.push_back(this->IntType);
    FuncTy_21_args.push_back(this->IntType);
    FunctionType *FuncTy_21 = FunctionType::get(
            /*Result=*/Type::getVoidTy(this->pModule->getContext()),
            /*Params=*/FuncTy_21_args,
            /*isVarArg=*/false);
    this->func_PropagateTaintAddrToReg = Function::Create(
            /*Type=*/FuncTy_21,
            /*Linkage=*/GlobalValue::ExternalLinkage,
            /*Name=*/"PropagateTaintAddrToReg", this->pModule);

}

void DynTaintAnalyzer::InitForInstument() {

    Init_Instrument_Type();
    Init_Instrument_Var();
    Init_Instrument_Function();
}


int DynTaintAnalyzer::insertMarkTaint(Instruction *Inst, Instruction *location) {
    int inst_id = GetInstructionID(Inst);
    if (inst_id == -1) {
        errs() << "Inst_id is -1\n";
        return -1;
    }
    string inst_id_str = std::to_string(inst_id);
    ConstantInt *const_int32_16 = ConstantInt::get(this->pModule->getContext(),
                                                   APInt(32, StringRef(inst_id_str), 10));
    CallInst *void_63 = CallInst::Create(this->func_MarkTaint, const_int32_16, "", location);

    return 0;
}

int DynTaintAnalyzer::insertRemoveTaint(Instruction *Inst, Instruction *location) {

    int inst_id = GetInstructionID(Inst);
    if (inst_id == -1) {
        errs() << "Inst_id is -1\n";
        return -1;
    }
    string inst_id_str = std::to_string(inst_id);
    ConstantInt *const_int32_16 = ConstantInt::get(this->pModule->getContext(),
                                                   APInt(32, StringRef(inst_id_str), 10));
    CallInst *void_63 = CallInst::Create(this->func_RemoveTaint, const_int32_16, "", location);


    return 0;
}

int DynTaintAnalyzer::insertMarkTaintAddr(Value *va, Instruction *location) {

    DataLayout *dl = new DataLayout(this->pModule);
    Type *vaType = va->getType()->getContainedType(0);

    while (isa<PointerType>(vaType)) {
        vaType = vaType->getContainedType(0);
    }

    if (vaType->isSized()) {
        ConstantInt *va_offset = ConstantInt::get(
                this->pModule->getContext(),
                APInt(32, StringRef(std::to_string(dl->getTypeAllocSize(vaType))), 10));

        CastInst *ptr_80 = new BitCastInst(va, this->VoidPointerType, "", location);

        std::vector<Value *> void_81_params;
        void_81_params.push_back(ptr_80);
        void_81_params.push_back(va_offset);
        CallInst *callMarkTaint = CallInst::Create(this->func_MarkTaintAddr, void_81_params, "", location);
        callMarkTaint->setCallingConv(CallingConv::C);
        callMarkTaint->setTailCall(false);
        AttributeList void_PAL;
        callMarkTaint->setAttributes(void_PAL);
    }
    return 0;
}

int DynTaintAnalyzer::insertRemoveTaintAddr(Value *va, Instruction *location) {

    DataLayout *dl = new DataLayout(this->pModule);
    Type *vaType = va->getType();
    while (isa<PointerType>(vaType))
        vaType = vaType->getContainedType(0);

    if (vaType->isSized()) {
        ConstantInt *dest_offset = ConstantInt::get(
                this->pModule->getContext(),
                APInt(32, StringRef(std::to_string(dl->getTypeAllocSize(vaType))), 10));

        CastInst *ptr_50 = new BitCastInst(va, this->VoidPointerType, "", location);
        //Instrument func_UpdateTaintSet

        std::vector<Value *> void_78_params;
        void_78_params.push_back(ptr_50);
        void_78_params.push_back(dest_offset);
        CallInst *void_78 = CallInst::Create(func_RemoveTaintAddr, void_78_params, "", location);
        void_78->setCallingConv(CallingConv::C);
        void_78->setTailCall(false);
        AttributeList void_78_PAL;
        void_78->setAttributes(void_78_PAL);
    }

    return 0;
}

int DynTaintAnalyzer::insertPropagateTaintAddrToAddr(Value *memSrcValue, Value *memDestValue, Instruction *location) {

    DataLayout *dl = new DataLayout(this->pModule);

    //obtain the src address and offset
    Type *srcType = memSrcValue->getType();
    while (isa<PointerType>(srcType))
        srcType = srcType->getContainedType(0);
    CastInst *ptr_50 = NULL;
    ConstantInt *src_offset = NULL;
    if (srcType->isSized()) {
        src_offset = ConstantInt::get(
                this->pModule->getContext(),
                APInt(32, StringRef(std::to_string(dl->getTypeAllocSize(srcType))), 10));
        ptr_50 = new BitCastInst(memSrcValue, this->VoidPointerType, "", location);
    }

    // obtain the dest address and offset
    Type *destType = memDestValue->getType();;
    while (isa<PointerType>(destType))
        destType = destType->getContainedType(0);

    CastInst *ptr_51 = NULL;
    ConstantInt *dest_offset = NULL;
    if (destType->isSized()) {
        dest_offset = ConstantInt::get(
                this->pModule->getContext(),
                APInt(32, StringRef(std::to_string(dl->getTypeAllocSize(destType))), 10));
        ptr_51 = new BitCastInst(memDestValue, this->VoidPointerType, "", location);
    }


    //Instrument func_UpdateTaintSet
    std::vector<Value *> void_105_params;
    void_105_params.push_back(ptr_50);          //src address
    void_105_params.push_back(dest_offset);     //src offset
    void_105_params.push_back(ptr_51);          //dest address
    void_105_params.push_back(src_offset);      //dest offset
    CallInst *callUpdateTaintSet = CallInst::Create(this->func_PropagateTaintAddrToAddr, void_105_params, "",
                                                    location);
    callUpdateTaintSet->setCallingConv(CallingConv::C);
    callUpdateTaintSet->setTailCall(false);
    AttributeList void_PAL;
    callUpdateTaintSet->setAttributes(void_PAL);

    return 0;
}

int DynTaintAnalyzer::insertPropagateTaintRegToReg(Instruction *src, Instruction *dest, Instruction *location) {

    int src_id = GetInstructionID(src);
    string src_id_str = std::to_string(src_id);
    if (src_id == -1) {
        errs() << "src_id == -1\n";
        return -1;
    }

    int dest_id = GetInstructionID(dest);
    string dest_id_str = std::to_string(dest_id);
    if (dest_id == -1) {
        errs() << "dest_id == -1\n";
        return -1;
    }

    ConstantInt *int32_92 = ConstantInt::get(this->pModule->getContext(),
                                             APInt(32, StringRef(src_id_str), 10));
    ConstantInt *int32_93 = ConstantInt::get(this->pModule->getContext(),
                                             APInt(32, StringRef(dest_id_str), 10));

    std::vector<Value *> void_94_params;
    void_94_params.push_back(int32_92);
    void_94_params.push_back(int32_93);
    CallInst *void_94 = CallInst::Create(this->func_PropagateTaintRegToReg, void_94_params, "", location);
    void_94->setCallingConv(CallingConv::C);
    void_94->setTailCall(false);
    AttributeList void_94_PAL;
    void_94->setAttributes(void_94_PAL);

    return 0;
}

int DynTaintAnalyzer::insertPropagateTaintRegToAddr(Instruction *src, Value *memDestValue, Instruction *location) {

    int src_id = GetInstructionID(src);
    string src_id_str = std::to_string(src_id);
    if (src_id == -1) {
        errs() << "src_id == -1\n";
        return -1;
    }

    ConstantInt *src_Inst_id = ConstantInt::get(this->pModule->getContext(),
                                                APInt(32, StringRef(src_id_str), 10));

    // obtain the dest address and offset
    DataLayout *dl = new DataLayout(this->pModule);
    Type *destType = memDestValue->getType();;
    while (isa<PointerType>(destType))
        destType = destType->getContainedType(0);

    CastInst *ptr_51 = NULL;
    ConstantInt *dest_offset = NULL;
    if (destType->isSized()) {
        dest_offset = ConstantInt::get(
                this->pModule->getContext(),
                APInt(32, StringRef(std::to_string(dl->getTypeAllocSize(destType))), 10));
        ptr_51 = new BitCastInst(memDestValue, this->VoidPointerType, "", location);
    }


    if (ptr_51 != NULL && dest_offset != NULL) {

        //instrument func_PropagateTaintAddrToReg
        std::vector<Value *> void_70_params;
        void_70_params.push_back(src_Inst_id);
        void_70_params.push_back(ptr_51);
        void_70_params.push_back(dest_offset);
        CallInst *void_70 = CallInst::Create(func_PropagateTaintRegToAddr, void_70_params, "", location);
        void_70->setCallingConv(CallingConv::C);
        void_70->setTailCall(false);
        AttributeList void_70_PAL;
        void_70->setAttributes(void_70_PAL);
    }

    return 0;
}

int DynTaintAnalyzer::insertPropagateTaintAddrToReg(Value *memSrcValue, Instruction *dest, Instruction *location) {

    // obtain the dest address and offset
    DataLayout *dl = new DataLayout(this->pModule);
    Type *srcType = memSrcValue->getType();

    while (isa<PointerType>(srcType))
        srcType = srcType->getContainedType(0);

    CastInst *ptr_51 = NULL;
    ConstantInt *src_offset = NULL;
    if (srcType->isSized()) {
        src_offset = ConstantInt::get(
                this->pModule->getContext(),
                APInt(32, StringRef(std::to_string(dl->getTypeAllocSize(srcType))), 10));
        ptr_51 = new BitCastInst(memSrcValue, this->VoidPointerType, "", location);
    }


    int dest_id = GetInstructionID(dest);
    string dest_id_str = std::to_string(dest_id);
    if (dest_id == -1) {
        errs() << "src_id == -1\n";
        return -1;
    }

    ConstantInt *dest_Inst_id = ConstantInt::get(this->pModule->getContext(),
                                                 APInt(32, StringRef(dest_id_str), 10));

    //instrument func_PropagateTaintAddrToReg
    if (ptr_51 != NULL && src_offset != NULL) {
        std::vector<Value *> void_73_params;
        void_73_params.push_back(ptr_51);
        void_73_params.push_back(src_offset);
        void_73_params.push_back(dest_Inst_id);
        CallInst *void_73 = CallInst::Create(this->func_PropagateTaintAddrToReg, void_73_params, "", location);
        void_73->setCallingConv(CallingConv::C);
        void_73->setTailCall(false);
        AttributeList void_73_PAL;
        void_73->setAttributes(void_73_PAL);
    }

    return 0;
}

int DynTaintAnalyzer::insertPropagateTaint(Value *src, Value *dest, Instruction *location) {

    if ((isa<ConstantInt>(src) || isa<ConstantFP>(src)) && (isa<Instruction>(dest))) {

        Instruction *destInst = dyn_cast<Instruction>(dest);
        if (insertRemoveTaint(destInst, location) == -1) {
            errs() << "fail to insertRemoveTaint in: \n";
            printSrcCodeInfo(location);
            return -1;
        }
    }


    //constant->addr;
    if ((isa<ConstantInt>(src) || isa<ConstantFP>(src)) && isa<GlobalVariable>(dest)) {
        if (insertRemoveTaintAddr(dest, location) == -1) {
            errs() << "fail to insertRemoveTaintAddr in: \n";
            printSrcCodeInfo(location);
            return -1;
        }
    }


    //Reg->Reg
    if (isa<Instruction>(src) && isa<Instruction>(dest)) {
        Instruction *srcInst = dyn_cast<Instruction>(src);
        Instruction *destInst = dyn_cast<Instruction>(dest);
        if (insertPropagateTaintRegToReg(srcInst, destInst, location) == -1) {
            errs() << "fail to insertPropagateTaintRegToReg in: \n";
            printSrcCodeInfo(location);
            return -1;
        }
    }

    //Reg->Addr
    if (isa<Instruction>(src) && isa<GlobalVariable>(dest)) {
        Instruction *srcInst = dyn_cast<Instruction>(src);
        if (insertPropagateTaintRegToAddr(srcInst, dest, location) == -1) {
            errs() << "fail to insertPropagateTaintRegToAddr in: \n";
            printSrcCodeInfo(location);
            return -1;
        }
    }


    //Addr->Reg
    if (isa<GlobalVariable>(src) && isa<Instruction>(dest)) {
        Instruction *destInst = dyn_cast<Instruction>(dest);
        if (insertPropagateTaintAddrToReg(src, destInst, location) == -1) {
            errs() << "fail to insertPropagateTaintAddrToReg in: \n";
            printSrcCodeInfo(location);
            return -1;
        }
    }



    //Addr->Addr
    if (isa<GlobalVariable>(src) && isa<GlobalVariable>(dest)) {
        if (insertPropagateTaintAddrToAddr(src, dest, location) == -1) {
            errs() << "fail to insertPropagateTaintAddrToAddr in: \n";
            printSrcCodeInfo(location);
            return -1;
        }
    }

    return 0;
}

int DynTaintAnalyzer::PrintTaintInst(Instruction *Inst) {

    int inst_id = GetInstructionID(Inst);
    if (inst_id == -1) {

        return -1;
    }

    if (isa<TerminatorInst>(Inst)) {

        string inst_id_str = std::to_string(inst_id);
        ConstantInt *const_int32_16 = ConstantInt::get(this->pModule->getContext(),
                                                       APInt(32, StringRef(inst_id_str), 10));
        CallInst *void_63 = CallInst::Create(this->func_printTaint, const_int32_16, "", Inst);

    } else if (CallInst *call = dyn_cast<CallInst>(Inst)) {

        CallSite ci(call);
        Function *callee = dyn_cast<Function>(ci.getCalledValue()->stripPointerCasts());

        if (callee && !callee->getReturnType()->isVoidTy() && callee->isDeclaration() == false) {

            string inst_id_str = std::to_string(inst_id);
            ConstantInt *const_int32_16 = ConstantInt::get(this->pModule->getContext(),
                                                           APInt(32, StringRef(inst_id_str), 10));
            CallInst *void_63 = CallInst::Create(this->func_printTaint, const_int32_16, "",
                                                 Inst->getNextNode()->getNextNode());
        } else {
            string inst_id_str = std::to_string(inst_id);
            ConstantInt *const_int32_16 = ConstantInt::get(this->pModule->getContext(),
                                                           APInt(32, StringRef(inst_id_str), 10));
            CallInst *void_63 = CallInst::Create(this->func_printTaint, const_int32_16, "",
                                                 Inst->getNextNode());
        }
    } else if (PHINode *phi = dyn_cast<PHINode>(Inst)) {

//        string inst_id_str = std::to_string(inst_id);
//        ConstantInt *const_int32_16 = ConstantInt::get(this->pModule->getContext(),
//                                                       APInt(32, StringRef(inst_id_str), 10));
//
//        for (int i = 0; i < phi->getNumIncomingValues(); i++) {
//
//            Inst = Inst->getNextNode();
//            Inst->dump();
//        }
//
//        if (isa<TerminatorInst>(Inst)) {
//            CallInst *void_63 = CallInst::Create(this->func_printTaint, const_int32_16, "",
//                                                 Inst);
//        } else {
//            CallInst *void_63 = CallInst::Create(this->func_printTaint, const_int32_16, "",
//                                                 Inst->getNextNode());
//        }


    } else {

        string inst_id_str = std::to_string(inst_id);
        ConstantInt *const_int32_16 = ConstantInt::get(this->pModule->getContext(),
                                                       APInt(32, StringRef(inst_id_str), 10));
        CallInst *void_63 = CallInst::Create(this->func_printTaint, const_int32_16, "", Inst->getNextNode());

    }


    return 0;
}


int DynTaintAnalyzer::TaintSeed(Instruction *Inst) {

    if (CallInst *call = dyn_cast<CallInst>(Inst)) {

        CallSite ci(call);
        Function *callee = dyn_cast<Function>(ci.getCalledValue()->stripPointerCasts());
        if (!callee) {
            //is function pointer
            return 0;
        }

        /* Mark arguments of network IO as taint source with config. */
        for (auto &ele: this->funArgListSet) {
            if (ele.funName == callee->getName().str()) {
                for (auto &arg:ele.argList) {
                    Value *buf = call->getOperand(arg);
                    if (Instruction *ist = dyn_cast<Instruction>(buf)) {
                        if (insertMarkTaint(ist, Inst) == -1) {
                            errs() << "Error: Failed to insertMark in:\n";
                            printSrcCodeInfo(Inst);
                            return -1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int DynTaintAnalyzer::TaintPropagate(Instruction *Inst) {

    if (GetInstructionID(Inst) == -1) {
        return 0;
    }

    if (AtomicCmpXchgInst *atomCmpchg = dyn_cast<AtomicCmpXchgInst>(Inst)) {

        errs() << "AtomicCmpXchgInst" << "\n";
    }

    if (AtomicRMWInst *atomRMW = dyn_cast<AtomicRMWInst>(Inst)) {

        errs() << "AtomicRMWInst" << "\n";
    }


    if (BinaryOperator *binOP = dyn_cast<BinaryOperator>(Inst)) {
        for (int i = 0; i < binOP->getNumOperands(); i++) {
            Value *src = binOP->getOperand(i);
            Value *dest = binOP;
            if (insertPropagateTaint(src, dest, Inst) == -1) {
                errs() << "fail to insertPropagateTaint in: \n";
                printSrcCodeInfo(Inst);
                return -1;
            };
        }
    }


    if (CallInst *call = dyn_cast<CallInst>(Inst)) {

        //pass taint real argument to formal argument
        CallSite ci(call);
        Function *callee = dyn_cast<Function>(ci.getCalledValue()->stripPointerCasts());
        if (callee && callee->isDeclaration() == false && callee->arg_size() != 0) {

            //parameter passing for function call excluding Instrumented function
            Function::arg_iterator args = callee->arg_begin();

            for (int i = 0; i < call->getNumOperands() - 1; i++) {
                Value *RealArg = call->getOperand(i);

                Value *formArg = args++;
                Value *formArgReal = NULL;
                for (Function::iterator BI = callee->begin(); BI != callee->end(); BI++) {
                    if (BasicBlock *BB = &*BI) {
                        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {
                            if (Instruction *Inst = &*II) {
                                if (StoreInst *store = dyn_cast<StoreInst>(Inst)) {
                                    if (store->getValueOperand() == formArg) {
                                        formArgReal = store->getPointerOperand();
                                    }
                                }
                            }
                        }
                    }
                }

                if (formArgReal != NULL) {
                    Value *src = RealArg;
                    Value *dest = formArgReal;
                    if (insertPropagateTaint(src, dest, Inst) == -1) {
                        errs() << "fail to insertPropagateTaint in: \n";
                        printSrcCodeInfo(Inst);
                        return -1;
                    };
                }
            }

        }


        if (callee && callee->isDeclaration() == false && !callee->getReturnType()->isVoidTy()) {

            Value *src = NULL;

            for (Function::iterator BI = callee->begin(); BI != callee->end(); BI++) {
                if (BasicBlock *BB = &*BI) {
                    for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {
                        if (Instruction *Inst = &*II) {
                            if (ReturnInst *ret = dyn_cast<ReturnInst>(Inst)) {
                                src = ret;
                            }
                        }
                    }
                }
            }

            Value *dest = call;

            if (src != NULL) {
                if (insertPropagateTaint(src, dest, Inst->getNextNode()) == -1) {
                    errs() << "fail to insertPropagateTaint in: \n";
                    printSrcCodeInfo(Inst);
                    return -1;
                };
            }

        }


        if (callee && callee->isDeclaration() == true) {
            /* memory function, if the src is tainted, then the dest is tainted */

            if (callee->getName() == "llvm.memcpy.p0i8.p0i8.i64" ||
                callee->getName() == "llvm.memmove.p0i8.p0i8.i64" ||
                callee->getName() == "llvm.memset.p0i8.i64" ||
                callee->getName() == "llvm.memset.p0i8.i64" ||
                callee->getName() == "strcpy" ||
                callee->getName() == "strncpy" ||
                callee->getName() == "strcat" ||
                callee->getName() == "strncat" ||
                callee->getName() == "strxfrm") {
                Value *src = call->getOperand(1);
                while (BitCastInst *bitcast = dyn_cast<BitCastInst>(src)) {
                    src = bitcast->getOperand(0);
                }

                Value *dest = call->getOperand(0);
                while (BitCastInst *bitcast = dyn_cast<BitCastInst>(dest)) {
                    dest = bitcast->getOperand(0);
                }

                if (insertPropagateTaint(src, dest, Inst) == -1) {
                    errs() << "fail to insertPropagateTaint in: \n";
                    printSrcCodeInfo(Inst);
                    return -1;
                };
            }
        }
    }

    if (CmpInst *cmp = dyn_cast<CmpInst>(Inst)) {
        for (int i = 0; i < cmp->getNumOperands(); i++) {
            Value *src = cmp->getOperand(i);
            Value *dest = cmp;
            if (insertPropagateTaint(src, dest, Inst) == -1) {
                errs() << "fail to insertPropagateTaint in: \n";
                printSrcCodeInfo(Inst);
                return -1;
            };
        }
    }

    if (ExtractElementInst *extrEle = dyn_cast<ExtractElementInst>(Inst)) {
        Value *src = extrEle->getOperand(0);
        Value *dest = extrEle;
        if (insertPropagateTaint(src, dest, Inst) == -1) {
            errs() << "fail to insertPropagateTaint in: \n";
            printSrcCodeInfo(Inst);
            return -1;
        };
    }


    if (FenceInst *fence = dyn_cast<FenceInst>(Inst)) {

    }


    //FuncletPadInst, including CleanupPadInst, CatchPadInst
    if (CleanupPadInst *clrPad = dyn_cast<CleanupPadInst>(Inst)) {

    }


    if (CatchPadInst *catchPad = dyn_cast<CatchPadInst>(Inst)) {

    }


    if (GetElementPtrInst *getPtr = dyn_cast<GetElementPtrInst>(Inst)) {
        Value *src = getPtr->getOperand(0);
        Value *dest = getPtr;
        if (insertPropagateTaint(src, dest, Inst) == -1) {
            errs() << "fail to insertPropagateTaint in: \n";
            printSrcCodeInfo(Inst);
            return -1;
        };
    }


    if (InsertElementInst *insertEle = dyn_cast<InsertElementInst>(Inst)) {
        Value *src = insertEle->getOperand(1);
        Value *dest = insertEle;
        if (insertPropagateTaint(src, dest, Inst) == -1) {
            errs() << "fail to insertPropagateTaint in: \n";
            printSrcCodeInfo(Inst);
            return -1;
        };

    }

    if (InsertValueInst *insertVa = dyn_cast<InsertValueInst>(Inst)) {

        Value *src = insertVa->getOperand(1);
        Value *dest = insertVa;
        if (insertPropagateTaint(src, dest, Inst) == -1) {
            errs() << "fail to insertPropagateTaint in: \n";
            printSrcCodeInfo(Inst);
            return -1;
        };

    }


    if (LandingPadInst *landPad = dyn_cast<LandingPadInst>(Inst)) {

    }


    if (PHINode *phi = dyn_cast<PHINode>(Inst)) {
//        for (int i = 0; i < phi->getNumIncomingValues(); i++) {
//            Value *src = phi->getIncomingValue(i);
//            Value *dest = phi;
//            if (insertPropagateTaint(src, dest, Inst->getNextNode()) == -1) {
//                errs() << "fail to insertPropagateTaint in: \n";
//                printSrcCodeInfo(Inst);
//                return -1;
//            };
//        }
    }

    if (SelectInst *select = dyn_cast<SelectInst>(Inst)) {
//        for(int i=1;i<select->getNumOperands();i++) {
//            Value *src = select->getOperand(i);
//            Value *dest = select;
//            if (insertPropagateTaint(src, dest, Inst) == -1) {
//                errs() << "fail to insertPropagateTaint in: \n";
//                printSrcCodeInfo(Inst);
//                return -1;
//            };
//        }


    }


    if (ShuffleVectorInst *shuff = dyn_cast<ShuffleVectorInst>(Inst)) {

    }


    if (StoreInst *store = dyn_cast<StoreInst>(Inst)) {
        Value *src = store->getValueOperand();
        Value *dest = store->getPointerOperand();
        //taint: src->dest
        if (insertPropagateTaint(src, dest, Inst) == -1) {
            errs() << "fail to insertPropagateTaint in: \n";
            printSrcCodeInfo(Inst);
            return -1;
        };

        //taint: dest->storeInst
        if (insertPropagateTaint(dest, Inst, Inst) == -1) {
            errs() << "fail to insertPropagateTaint in: \n";
            printSrcCodeInfo(Inst);
            return -1;
        };

    }


    //UnaryInstruction
    if (AllocaInst *alloc = dyn_cast<AllocaInst>(Inst)) {

    }


    if (LoadInst *load = dyn_cast<LoadInst>(Inst)) {
        Value *src = load->getOperand(0);
        Value *dest = load;
        if (insertPropagateTaint(src, dest, Inst) == -1) {
            errs() << "fail to insertPropagateTaint in: \n";
            printSrcCodeInfo(Inst);
            return -1;
        };

    }

    //ExtractValueInst
    if (ExtractValueInst *extrValue = dyn_cast<ExtractValueInst>(Inst)) {
        Value *src = extrValue->getOperand(0);
        Value *dest = extrValue;
        if (insertPropagateTaint(src, dest, Inst) == -1) {
            errs() << "fail to insertPropagateTaint in: \n";
            printSrcCodeInfo(Inst);
            return -1;
        };
    }


    if (VAArgInst *VAA = dyn_cast<VAArgInst>(Inst)) {

    }


    if (CastInst *cast = dyn_cast<CastInst>(Inst)) {
        Value *src = cast->getOperand(0);
        Value *dest = cast;
        if (insertPropagateTaint(src, dest, Inst) == -1) {
            errs() << "fail to insertPropagateTaint in: \n";
            printSrcCodeInfo(Inst);
            return -1;
        };
    }


    //TerminatorInst in the following:
    if (InvokeInst *invoke = dyn_cast<InvokeInst>(Inst)) {

    }

    if (ResumeInst *resume = dyn_cast<ResumeInst>(Inst)) {

    }

    if (ReturnInst *ret = dyn_cast<ReturnInst>(Inst)) {
        Function *fun = ret->getParent()->getParent();
        if (!fun->getReturnType()->isVoidTy()) {
            Value *src = ret->getReturnValue();
            Value *dest = ret;
            if (insertPropagateTaint(src, dest, Inst) == -1) {
                errs() << "fail to insertPropagateTaint in: \n";
                printSrcCodeInfo(Inst);
                return -1;
            };
        }
    }

    if (SwitchInst *swit = dyn_cast<SwitchInst>(Inst)) {

    }

    if (UnreachableInst *unreach = dyn_cast<UnreachableInst>(Inst)) {

    }

    if (BranchInst *br = dyn_cast<BranchInst>(Inst)) {

    }

    if (CatchReturnInst *catchRet = dyn_cast<CatchReturnInst>(Inst)) {

    }

    if (CatchSwitchInst *catchSwit = dyn_cast<CatchSwitchInst>(Inst)) {

    }

    if (CleanupReturnInst *cleanRet = dyn_cast<CleanupReturnInst>(Inst)) {

    }

    if (IndirectBrInst *indirectRr = dyn_cast<IndirectBrInst>(Inst)) {

    }

    return 0;
}

void DynTaintAnalyzer::DoInstrument() {

    for (Module::iterator FI = this->pModule->begin(); FI != this->pModule->end(); FI++) {
        if (Function *F = &*FI) {
            if (F->isDeclaration()) {
                continue;
            }

            for (Function::iterator BI = F->begin(); BI != F->end(); BI++) {
                if (BasicBlock *BB = &*BI) {
                    for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {
                        if (Instruction *Inst = &*II) {

                            //errs()<< Inst->getName()<<"\n";

                            if (GetInstructionID(Inst) == -1) {
                                continue;  //do not handle instrument instruction
                            }
#ifdef Debug
                            // Inst->dump();
#endif

                            if (TaintSeed(Inst) == -1) {
                                errs() << "Error: Failed to TaintSeed in " << Inst->getName() << "\n";
                            };

                            if (TaintPropagate(Inst) == -1) {
                                errs() << "Error: Failed to TaintPropagate in " << Inst->getName() << "\n";
                            };

                            if (PrintTaintInst(Inst) == -1) {
                                errs() << "Error: Failed to TaintPropagate in " << Inst->getName() << "\n";
                            };

                        }
                    }
                }
            }
        }
    }
}

void DynTaintAnalyzer::ImportConfig() {

    ifstream file(NetworkIOPath);
    vector<string> strVect;

    if (!file) {
        errs() << "Invalid network io path\n";
    } else {
        string line;
        while (getline(file, line)) {
            boost::split(strVect, line, boost::is_any_of(","), boost::token_compress_on);
            struct TaintFunArg temp_taintFunArg;
            string fname = strVect[0];
            boost::trim(fname);
            temp_taintFunArg.funName = fname;
            for (int i = 1; i < strVect.size(); i++) {
                int argLoc = atoi(strVect[i].c_str());
                temp_taintFunArg.argList.push_back(argLoc);
            }
            this->funArgListSet.insert(temp_taintFunArg);
        }
    }

}

bool DynTaintAnalyzer::runOnModule(Module &M) {

    this->pModule = &M;

    if (strFileName.empty()) {
        errs() << "Please set output file path and name." << "\n";
        return false;
    }


    if (NetworkIOPath.empty()) {
        errs() << "Please set network io file path, which should include"
                "network io function name, and argument need to be marked tainted." << "\n";
        return false;
    }

    //import TaintFunArg config
    ImportConfig();

#ifdef Debug
    for (auto &ele:this->funArgListSet) {
        errs() << ele.funName << "\n";
        for (auto &e: ele.argList) {
            errs() << e << "\n";
        }
    }
#endif



    //Init for Instrument
    InitForInstument();

    //Process Instrument
    DoInstrument();


    errs() << "Finish Instrument\n";




    //ouput to file
//    ofstream f_out;
//    f_out.open(strFileName, std::ofstream::out | std::ofstream::app);
//    f_out << "end" << "\n";
//    f_out.close();
    return false;

}
