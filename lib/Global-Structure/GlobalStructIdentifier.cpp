//===----------------------------------------------------------------------===//
//
// This file implements a versions of the LLVM "GlobalStructIdentifier" pass
//
//===----------------------------------------------------------------------===//


#include "Global-Structure/GlobalStructIdentifier.h"

static RegisterPass<GlobalStructIdentifier> X(
        "global-identify-struct",
        "Identify global data structure",
        false,true);


static cl::opt<std::string> strFileName(
        "strFileName",
        cl::desc("The name of File to store class and struct"), cl::Optional,
        cl::value_desc("strFileName"));


char GlobalStructIdentifier::ID = 0;


bool GlobalStructIdentifier::runOnModule(Module &M){
     //errs() << "Module: " << M.getName()<< "\n";
    if (strFileName.empty()) {
        errs() << "Please set file name!" << "\n";
        exit(1);
    }

    ofstream sysCallFile;
    sysCallFile.open(strFileName, std::ofstream::out | std::ofstream::app);

     for(auto &S : M.getIdentifiedStructTypes()){
     //  errs()<<S->getName()<<"\n";
         sysCallFile << S->getName().str() << "\n";
     }

    sysCallFile.close();
     return false;
 }
