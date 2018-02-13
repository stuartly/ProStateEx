//
// Created by stly on 1/16/18.
//


#include <stdio.h>

#include <ctime>
#include <map>
#include <set>
#include <vector>
#include <string.h>
#include <iostream>
#include <fstream>


#ifdef __cplusplus
using namespace std;


extern "C" {


bool isTaintAddr(void *memory_addr, unsigned int offset);
void MarkTaintAddr(void *memory_addr, unsigned int offset);
void RemoveTaintAddr(void *memory_addr, unsigned int offset);


bool isTaint(int Inst_id);
void MarkTaint(int Inst_id);
void RemoveTaint(int Inst_id);
void PropagateTaintRegToReg(int OpInst, int ResultInst);
void PropagateTaintRegToAddr(int OpInst, void *dest_mem_addr, unsigned int dest_offset);
void PropagateTaintAddrToReg(void *src_mem_addr, unsigned int src_offset, int ResultInst);
void PropagateTaintAddrToAddr(void *src_mem_addr, unsigned int src_offset,
                              void *dest_mem_addr, unsigned int dest_offset);
void printTaint(int Inst_id);

};

#endif
