#ifndef _H_SONGLH_LOOPCOUNTER
#define _H_SONGLH_LOOPCOUNTER

#include <set>
#include <string>
#include <ostream>
#include <fstream>

#include "llvm/Pass.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"


using namespace llvm;
using namespace std;


struct LoopCounter : public ModulePass
{
	static char ID;
	LoopCounter();


	virtual void getAnalysisUsage(AnalysisUsage &AU) const;
	virtual bool runOnModule(Module& M);
    virtual void print(raw_ostream &O, const Module * M) const;

};

#endif