#include <fstream>
#include "Common/BasicBlockCommon.h"
#include "StateVarIdentifier/StateVarIdentifier.h"


#define _DEBUG 1
#define Call_System_IO 1
#define Call_Wrapper_IO 2


static RegisterPass<StateVarIdentifier> X(
        "identify-state-var",
        "identify state variable",
        false,
        true);

//
//static cl::opt<std::string> SystemIOPath("SystemIOPath",
//                                         cl::desc("Path of file containing system io"), cl::Optional,
//                                         cl::value_desc("SystemIOPath"));

static cl::opt<std::string> strFileName("strFileName",
                                        cl::desc("The name of File to store candidate state variable"), cl::Optional,
                                        cl::value_desc("strFileName"),
                                        cl::init(""));


char StateVarIdentifier::ID = 0;



void StateVarIdentifier::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}


StateVarIdentifier::StateVarIdentifier() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializePostDominatorTreeWrapperPassPass(Registry);
    initializeDominatorTreeWrapperPassPass(Registry);
    initializeLoopInfoWrapperPassPass(Registry);
}


void printSrcCodeInfo(Instruction *pInst) {
    const DILocation *DIL = pInst->getDebugLoc();

    if (!DIL) {
        //errs()<< "failed to get DIL\n";
        return;
    }

    char pPath[200];

    string sFileName = DIL->getDirectory().str() + "/" + DIL->getFilename().str();
    realpath(sFileName.c_str(), pPath);
    sFileName = string(sFileName);
    unsigned int numLine = DIL->getLine();
    errs() << "//---" << sFileName << ": " << numLine << "\n";

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

void StateVarIdentifier::MarkIOFlagToFunction(Module &M, Function *F) {
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



set<string> StateVarSet;

bool StateVarIdentifier::runOnModule(Module &M) {

    if (strFileName.empty()) {
        errs() << "Please set output file path and name." << "\n";
        return false;
    }

    errs()<<"find state variable\n";

    //ouput to file
    ofstream f_out;
    f_out.open(strFileName, std::ofstream::out | std::ofstream::app);

    for (auto &ele: StateVarSet) {
        errs() << ele << "\n";
        f_out << ele << "\n";
    }

    f_out.close();
    return false;

}
