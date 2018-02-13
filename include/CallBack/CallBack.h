#ifndef _H_SONGLH_CALLBACK
#define _H_SONGLH_CALLBACK

#include "llvm/Pass.h"

#include "llvm/IR/Instructions.h"

#include <set>

using namespace llvm;
using namespace std;


struct CallBack : public ModulePass
{
	static char ID;
	CallBack();

	virtual void getAnalysisUsage(AnalysisUsage &AU) const;
	virtual bool runOnModule(Module& M);
	virtual void print(raw_ostream &O, const Module *M) const;

	void getCallBackValue(Instruction * pInst, set<Value *> & setCallBackValue);

};


#endif
