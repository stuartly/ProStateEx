#include <set>
#include <string>
#include <ostream>
#include <fstream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/User.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/DebugInfo.h"


#include "CFG/CFG.h"



using namespace std;
using namespace llvm;

#define DEBUG_TYPE "StructIdentifier"

class FunStructStore
{
	public:
		Function* Fun;
		StructType* Struct;
		StoreInst *Store;
		bool operator<(const FunStructStore &other) const { return Store < other.Store; }
};

class FunStruct
{
public:
    Function* Fun;
    StructType* Struct;
    bool operator<(const FunStruct &other) const { return Struct < other.Struct; }
};

class StructStoreInfo
{
public:
    StructType* Struct;
    unsigned FieldNum;
    unsigned RelatedStoreNum;
    unsigned DateDepStoreNum;
    bool operator<(const StructStoreInfo &other) const { return Struct < other.Struct; }
};

class LogicOp
{
	public:
		BasicBlock * B;
        Instruction * I;
 		bool operator < (const LogicOp &other) const { return B < other.B; }
};

struct StructIdentifier : public ModulePass{
        static char ID; // Pass identification, replacement for typeid
        StructIdentifier();
        bool runOnModule(Module &M);
        bool isDataDep_LogicOp(Function *F, BasicBlock* B,Value* V);
        bool isControlDep_LogicOp(Function* F, BasicBlock* B);
        bool isLogicalOperator(unsigned numOpCode);
		bool isFromConsecutiveMem(Function* F, BasicBlock* B, Value* V);
        unsigned getDataDepSotreNum(StructType* st, set<FunStructStore> FunStructStoreSet);
        void getAnalysisUsage(AnalysisUsage &AU) const;
};

