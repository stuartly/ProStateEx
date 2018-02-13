#ifndef PROJECT_FUNCTIONCOMMON_H
#define PROJECT_FUNCTIONCOMMON_H

#include <set>
#include <string>

#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

using namespace std;
using namespace llvm;

Function * SearchFunctionByName(Module & M, string strFunctionName);

bool HasIO(Function *F);
bool HasIO(Instruction *Inst);
bool HasLoop(Function *F);
int HasLooPIO(Function *F);

std::set <Function *> GetSpecialFunctionList(Module &M, int FlagType);

#endif //PROJECT_FUNCTIONCOMMON_H
