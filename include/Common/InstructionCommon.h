//
// Created by kevin on 11/14/17.
//

#ifndef PROJECT_INSTRUCTIONCOMMON_H
#define PROJECT_INSTRUCTIONCOMMON_H

#include "llvm/IR/Instruction.h"

using namespace llvm;


int GetInstructionID(Instruction * II);

Instruction * SearchInstByIndex(Function * pFunction, unsigned numIndex);

std::string printSrcCodeInfo(Instruction *pInst);

#endif //PROJECT_INSTRUCTIONCOMMON_H
