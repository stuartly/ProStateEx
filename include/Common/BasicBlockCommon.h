//
// Created by kevin on 11/14/17.
//

#ifndef PROJECT_BASICBLOCKCOMMON_H
#define PROJECT_BASICBLOCKCOMMON_H


#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"


using namespace llvm;


int GetBasicBlockID(BasicBlock * BB);

BasicBlock * SearchBlockByIndex(Function * pFunction, unsigned numIndex);


#endif //PROJECT_BASICBLOCKCOMMON_H
