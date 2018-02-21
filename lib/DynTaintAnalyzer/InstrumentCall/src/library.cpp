//
// Created by stly on 1/16/18.
//


#include "library.h"

std::set<std::pair<unsigned long, unsigned int>> TaintValueSet;
std::set<int> TaintInstSet;

void isValue(Value* va){
    printf("I am a value\n");
}


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

bool isTaintAddr(void *memory_addr, unsigned int offset) {

    unsigned long addr = (unsigned long) memory_addr;

    std::pair<unsigned long, unsigned int> TempPair;
    TempPair.first = addr;
    TempPair.second = offset;

    if (TaintValueSet.find(TempPair) != TaintValueSet.end()) {
        return true;
    }

    return false;
}

void MarkTaintAddr(void *memory_addr, unsigned int offset) {
    unsigned long addr = (unsigned long) memory_addr;
    std::pair<unsigned long, unsigned int> TempPair;
    TempPair.first = addr;
    TempPair.second = offset;
    if (TaintValueSet.find(TempPair) == TaintValueSet.end()) {
        TaintValueSet.insert(TempPair);
    }
}

void RemoveTaintAddr(void *memory_addr, unsigned int offset) {
    unsigned long addr = (unsigned long) memory_addr;
    std::pair<unsigned long, unsigned int> TempPair;
    TempPair.first = addr;
    TempPair.second = offset;
    if (TaintValueSet.find(TempPair) != TaintValueSet.end()) {
        TaintValueSet.erase(TempPair);
    }
}


/* Inst id */
bool isTaint(int Inst_id) {
    if (TaintInstSet.find(Inst_id) != TaintInstSet.end()) {
        return true;
    } else {
        return false;
    }
}

void MarkTaint(int Inst_id) {
    if (TaintInstSet.find(Inst_id) == TaintInstSet.end()) {
        TaintInstSet.insert(Inst_id);
    }
}


void RemoveTaint(int Inst_id) {
    if (TaintInstSet.find(Inst_id) == TaintInstSet.end()) {
        TaintInstSet.erase(Inst_id);
    }
}


void PropagateTaintRegToReg(int OpInst, int ResultInst) {
    if (isTaint(OpInst)) {
        MarkTaint(ResultInst);
    }
}

void PropagateTaintRegToAddr(int OpInst, void *dest_mem_addr, unsigned int dest_offset) {
    if (isTaint(OpInst)) {
        MarkTaintAddr(dest_mem_addr, dest_offset);
    }
}

void PropagateTaintAddrToReg(void *src_mem_addr, unsigned int src_offset, int ResultInst) {
    if (isTaintAddr(src_mem_addr, src_offset)) {
        MarkTaint(ResultInst);
    }
}

void
PropagateTaintAddrToAddr(void *src_mem_addr, unsigned int src_offset, void *dest_mem_addr, unsigned int dest_offset) {
    if (isTaintAddr(src_mem_addr, src_offset)) {
        MarkTaintAddr(dest_mem_addr, dest_offset);
    } else {
        RemoveTaintAddr(dest_mem_addr, dest_offset);
    }
}

void printTaint(int Inst_id) {


    if (isTaint(Inst_id)) {
        //printf("Execute tainted instruction: %d.\n", Inst_id);
        ofstream f_out;
        f_out.open("logger.txt", std::ofstream::out | std::ofstream::app);
        //f_out << "[" << currentDateTime() << "]: " << "Execute tainted instruction: " << Inst_id << "\n";
        f_out <<Inst_id << "\n";
        f_out.close();

    } else {
        //printf("Not tainted instruction: %d.\n", Inst_id);
    }
}






