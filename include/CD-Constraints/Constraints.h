#ifndef _H_SONGLH_CONSTRAINTS
#define _H_SONGLH_CONSTRAINTS

#include "llvm/Pass.h"

using namespace llvm;

struct Constraints : public ModulePass
{
	static char ID;
	Constraints();

	virtual void getAnalysisUsage(AnalysisUsage &AU) const;
	virtual bool runOnModule(Module& M);
	virtual void print(raw_ostream &O, const Module * M) const;



};



#endif 