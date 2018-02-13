#ifndef _H_SONGLH_NOOPT
#define _H_SONGLH_NOOPT


#include "llvm/Pass.h"

using namespace llvm;

struct NoOPT: public FunctionPass
{
	static char ID;
	NoOPT();
	virtual void getAnalysisUsage(AnalysisUsage &AU) const;
	virtual bool runOnFunction(Function &F);
};

#endif 