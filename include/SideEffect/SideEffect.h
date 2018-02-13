
#ifndef _H_SONGLH_SIDEEFFECT
#define _H_SONGLH_SIDEEFFECT

#include "llvm/Pass.h"

using namespace llvm;

struct SideEffect : public ModulePass
{
	static char ID;
	SideEffect();

	virtual void getAnalysisUsage(AnalysisUsage &AU) const;
	virtual bool runOnModule(Module& M);
	virtual void print(raw_ostream &O, const Module * M) const;



};



#endif