#ifndef _H_SONGLH_REACHANALYSIS
#define _H_SONGLH_REACHANALYSIS
#include "llvm/IR/BasicBlock.h"

#include <set>

using namespace llvm;
using namespace std;

BasicBlock * nextstep(BasicBlock * x, BasicBlock * y);

#endif