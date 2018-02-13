
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>


using namespace std;
using namespace llvm;

#define DEBUG_TYPE "GlobalStructIdentifier"


// GlobalStructureIdentifier - access the struct in a module based on ModulePass
struct GlobalStructIdentifier : public ModulePass{
        static char ID; // Pass identification, replacement for typeid
	GlobalStructIdentifier():ModulePass(ID){}
	bool runOnModule(Module &M);
};

