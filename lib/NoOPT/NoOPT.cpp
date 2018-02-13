


#include "NoOPT/NoOPT.h"

#include "llvm/IR/Function.h"

using namespace llvm;

static RegisterPass<NoOPT> X(
		"no-opt",
		"remove no optimization functon attribute",
		false, false);


char NoOPT::ID = 0;

NoOPT::NoOPT(): FunctionPass(ID) {}

void NoOPT::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.setPreservesCFG();
}

bool NoOPT::runOnFunction(Function &F)
{
	F.removeFnAttr(Attribute::OptimizeNone);

	return true;
}
