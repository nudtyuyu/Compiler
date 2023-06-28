#pragma once
#include <vector>
#include <string>
#include <map>

#include <IR.h>

using std::string;
using std::to_string;
using sysy::Value;

namespace backend {

class RegManager{
public:
    enum RegId : unsigned {
        R0 = 0,
        R1 = 1,
        R2 = 2,
        R3 = 3,
        R4 = 4,
        R5 = 5,
        R6 = 6,
        R7 = 7,
        R8 = 8,
        R9 = 9,
        R10 = 10,
        R11 = 11,
        R12 = 12,
        SP  = 13,
        LR  = 14,
        PC  = 15,
        RNONE = 1024,
        RANY = 2048,
    };
    static constexpr const RegId UserRegs[] = {R0, R1, R2, R3, R4, R5, R6, R7, R8, R10, R12};
    static string toString(RegId reg){
        if(reg == RNONE) return "RNONE";
        if(reg == RANY) return "RANY";
        return "r" + to_string(reg);
    }

private:
    std::map<RegId,std::vector<Value*> > RVALUE;
    struct Entry {
        RegId reg;
        int mem;
        enum Kind {ABSOLUTE, RELATIVE} kind;

        Entry() : reg(RNONE), mem(0), kind(ABSOLUTE) {};
    };
    std::map<Value *, Entry *> AVALUE;
    int *spOffset;

private:
    std::string storeBack(Value *value);

public:
    RegManager(int *_spOffset);

public:
    std::vector<RegId> AssignReg(int Number);
    std::pair<std::string, std::vector<RegId>> query(Value *dstVal, const std::vector<Value *> &srcVal = {});
    RegId getReg(Value *value) const;
    int getAddress(Value *value) const;
    int getOffset(Value *value) const;
    void setReg(Value *value, RegId reg);
    void setAddress(Value *value, int address);
    void setOffset(Value *value, int offset);
    void ShowAVALUE();
    void clear();
    
    std::vector<Value*> getRVALUE(RegId reg)
    {
        return RVALUE[reg];
    }
    void clearRVALUE(RegId reg)
    {
        RVALUE[reg].clear();
    }
    void insertRVALUE(RegId reg, Value* val)
    {
        RVALUE[reg].push_back(val);
    }
    bool IsEmpty(RegId reg)
    {
        return (RVALUE[reg].size()==0);
    }
};

} // namespace sysy