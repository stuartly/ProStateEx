#include <fstream>
#include "Common/BasicBlockCommon.h"
#include "StaticTaintAnalyzer/StaticTaintAnalyzer.h"


#define Call_System_IO 1
#define Call_Wrapper_IO 2
#define Debug

static RegisterPass<StaticTaintAnalyzer> X(
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


char StaticTaintAnalyzer::ID = 0;

void StaticTaintAnalyzer::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

StaticTaintAnalyzer::StaticTaintAnalyzer() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    initializePostDominatorTreeWrapperPassPass(Registry);
    initializeDominatorTreeWrapperPassPass(Registry);
    initializeLoopInfoWrapperPassPass(Registry);
}

int StaticTaintAnalyzer::printSrcCodeInfo(Instruction *pInst) {
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


void StaticTaintAnalyzer::ImportConfig() {

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

bool StaticTaintAnalyzer::runOnModule(Module &M) {

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



    errs() << "Finish Instrument\n";




    //ouput to file
//    ofstream f_out;
//    f_out.open(strFileName, std::ofstream::out | std::ofstream::app);
//    f_out << "end" << "\n";
//    f_out.close();
    return false;

}
