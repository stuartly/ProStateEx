#include "CallBack/CallBack.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Use.h"



#include "llvm/Support/raw_ostream.h"

#include <set>

using namespace std;
using namespace llvm;



static RegisterPass<CallBack> X(
		"callback-analysis",
		"static callback analysis",
		false,
		true);


char CallBack::ID = 0;

void CallBack::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.setPreservesAll();
}

CallBack::CallBack(): ModulePass(ID)
{
	//PassRegistry &Registry = *PassRegistry::getPassRegistry();
}

void CallBack::print(raw_ostream &O, const Module *M) const
{
	return;
}

void CallBack::getCallBackValue(Instruction * pInst, set<Value *> & setCallBackValue)
{

	if(CallInst * pCall = dyn_cast<CallInst>(pInst))
	{
		Function * pFunc = pCall->getCalledFunction();

		if(pFunc == NULL)
		{
			return;
		}

		if(pFunc->getName() == "signal")
		{
			return;
		}

		for(unsigned int i = 0; i < pCall->getNumArgOperands(); i ++)
		{
			Value * pVal = pCall->getArgOperand(i);

			if(isa<PointerType>(pVal->getType()))
			{
				if(isa<FunctionType>((dyn_cast<PointerType>(pVal->getType())->getElementType())))
				{
					setCallBackValue.insert(pVal);
				}
			}
		}

	}
	else if(InvokeInst * pInvoke = dyn_cast<InvokeInst>(pInst))
	{
		pInvoke->dump();
		assert(false);
	}


	return;
}


bool CallBack::runOnModule(Module & M)
{
	set<Value *> setCallBackValue;

	for(Module::iterator FF = M.begin(); FF != M.end(); FF ++)
	{
		for(Function::iterator BB = FF->begin(); BB != FF->end(); BB ++)
		{
			for(BasicBlock::iterator II = BB->begin(); II != BB->end(); II ++)
			{
				if(Instruction * pInst = dyn_cast<Instruction>(II))
				{
					getCallBackValue(pInst, setCallBackValue);
				}
			}
		}
	}

	set<Value *>::iterator itBegin = setCallBackValue.begin();
	set<Value *>::iterator itEnd = setCallBackValue.end();

	for(; itBegin != itEnd; itBegin ++)
	{
		if(LoadInst * pLoad = dyn_cast<LoadInst>(*itBegin))
		{
			Value * pValue = pLoad->getPointerOperand();

			for(Value::use_iterator useBegin = pValue->use_begin(); useBegin != pValue->use_end(); useBegin ++)
			{
				if(StoreInst * pStore = dyn_cast<StoreInst>(*useBegin))
				{
					errs() << pStore->getValueOperand()->getName() << "\n";
				}

			}

		}
	}

	return false;	
}
