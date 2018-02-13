#include "llvm/Pass.h"

using namespace llvm;

struct SysLibCounter : public ModulePass
{
	static char ID;
	SysLibCounter();

	virtual void getAnalysisUsage(AnalysisUsage &AU) const;
	virtual bool runOnModule(Module& M);
	virtual void print(raw_ostream &O, const Module * M) const;

};
