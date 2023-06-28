/**
 * TODO : 简单的寄存器分配方法 处理分配不到寄存器的情况.
 * 
 * 后端设计需要注意的点
 * 
 * label分配   : 全局变量的label,常量的label,basicblock的label,函数入口label,函数出口label
 * 全局数据处理
 * 常量处理
 * 临时变量的处理(包括函数参数也是临时变量)
 * 函数参数的处理
 * 块参数的处理
 * 函数栈空间结构的设计
 * 块的遍历问题
 * arm32的过程调用约定
 * 
 * 后端测试需要注意的点
 * 
 * 如何汇编与链接.
 * qemu以及树莓派如何运行.
*/
#pragma once
#include <string>
#include <set>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include <IR.h>
#include "RegManager.h"

using namespace sysy;
using std::string;
using std::map;
using std::vector;
using std::pair;

/*
#define Macro_ERROR_MSG(...) \
do{\
    fprintf(stderr, "ERROR in line-%d of %s: ", __LINE__, __FILE__);\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\n");\
    fflush(stderr);\
    exit(1);\
}while(0);
*/

// #define M_emitInst(STR) (space + STR + endl)
// #define M_emitComment(STR) (space + "//" + STR + endl)
// #define M_emitLabel(STR) (STR + ":" + endl)

namespace backend {

    class RegManager;
    class CodeGen;
    class Operand;

    using RegId = RegManager::RegId;

    //only for armv7
    static  const int int_align = 4;
    static  const int int_size = 4;
    static  const int int_p2align = 2;
    static  const int reg_size = 4;

    //default arm32 max imm
    static const uint32_t maxMovImm = 0xFFF;
    static  const string stackIndexReg = "fp";
    // class Operand{
    // public:
    //     using RegId = RegManager::RegId;
    //     enum Kind {
    //         kReg,
    //         kImm,
    //     };
    //     Kind kind;
    //     union {
    //         uint32_t imm;
    //         RegId regId;
    //     };
    //     Operand() {};
    //     Operand(uint32_t imm) : kind(kImm), imm(imm) {}
    //     Operand(RegId regId) : kind(kReg), regId(regId) {}
    //     bool isImm(void) {return kind == kImm;}
    //     bool isReg(void) {return kind == kReg;}
    //     uint32_t getImm(void) {assert(kind == kImm);return imm;}
    //     RegId getRegId(void) {assert(kind == kReg);return regId;}
    //     string toString(void) {
    //         if(kind == kImm) return "#" + to_string(imm);
    //         else return RegManager::toString(regId);
    //     }
    // };

class CodeGen{
private:
    Module *module;
    Function *curFunc;
    BasicBlock *curBB;
    //
    RegManager regm;
    //globalValue
    // bool loadGlobalValByMOVWT = true;
    //basicBlock
    vector<BasicBlock *> linear_bb;
    int bb_no = 0;
    //function params, return value and localVar
    // map<Argument *, int> paramsStOffset;
    // map<Instruction *, int> localVarStOffset;
    // int retValueStOffset = 0;
    size_t stOffsetAcc = 0;
    //label manager
    map<BasicBlock *, string> bb_labels;
    uint64_t label_no = 0;
    // size_t fpOffset = -8;
    int spOffset;

    

public:
    //TODO: add the builder to the CodeGen!!!!
    CodeGen(Module *module) : module(module), regm(&spOffset) {}
    //code_gen function list
    string code_gen();
    string module_gen(Module *module);
    string function_gen(Function *func);
    string basicBlock_gen(BasicBlock *bb);
    string instruction_gen(Instruction *instr);
    string globaldata_gen();
    string prologueCode_gen(Function *func);
    string epilogueCode_gen(Function *func);
    string literalPoolsCode_gen(Function *func);
    string functionHead_gen(Function *func);
    //Module
    void clearModuleRecord(Module *module){
        label_no = 0;
    }
    
    void clearFunctionRecord(Function *func);
    string getBBLabel(BasicBlock *bb);

    //instruction gen function list
    string loadInst_gen(LoadInst  *ldInst);
    string storeInst_gen(StoreInst  *stInst);
    pair<RegId, string> allocaInst_gen(AllocaInst *aInst, RegId dstRegId);
    string returnInst_gen(ReturnInst *retInst); 
    pair<RegId, string> callInst_gen(CallInst *retInst, RegId dstRegId);
    pair<RegId, string> binaryInst_gen(BinaryInst *bInst, RegId dstRegId);
    pair<RegId, string> unaryInst_gen(UnaryInst *bInst, RegId dstRegId);
    string uncondBrInst_gen(UncondBrInst *ubInst);
    string condBrInst_gen(CondBrInst *ubInst);
};

} // namespace backend
