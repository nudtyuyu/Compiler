#include "RegManager.h"
#include <cstdint>
#include <string>

using std::cerr;
const string space = string(4, ' ');
const string endl = "\n";

namespace backend {

using RegId = RegManager::RegId;
    
static uint16_t getWordHigh(uint32_t cval){
    return (cval >> 16) & 0xFFFF;
}

static uint16_t getWordLow(uint32_t cval){
    return cval & 0xFFFF;
}

RegManager::RegManager(int *_spOffset) : spOffset(_spOffset) {
    for (int i = 0; i < 16; i++) {
        // std::vector<std::string> rval;
        RVALUE[RegId(i)] = std::vector<Value *>();
    }
}

std::vector<RegId> RegManager::AssignReg(int Number) {
    // Really Really Easy!
    std::vector<RegId> RegList;
    srand((unsigned)time(NULL));
    if (Number == 2) {
        int count = 0;
        int r0 = -1;
        while (count < 2) {
            for (int i = 4; i <= 12; i++) {
                if (count >= 2) {
                    break;
                }
                if (i == 11 || i == 9) {
                    continue;
                } else if (RVALUE[RegId(i)].size() == 0) {
                    RegList.push_back(RegId(i));
                    count++;
                    r0 = i;
                }
            }
            if (count == 1) {
                int label = 4 + rand() % 13;
                while ((label == 11 || label == 9) || label == r0) {
                    label = 4 + rand() % 13;
                }
                RegList.push_back(RegId(label));
                count++;
            } else if (count == 0) {
                int label0 = 4 + rand() % 13;
                int label1 = 4 + rand() % 13;
                while (label0 == 11 || label0 == 9) {
                    label0 = 4 + rand() % 13;
                }
                while ((label1 == 11 || label1 == 9) || label1 == label0) {
                    label1 = 4 + rand() % 13;
                }
                RegList.push_back(RegId(label0));
                RegList.push_back(RegId(label1));
                count += 2;
            }
        }
    }

    else if (Number == 1) {
        for (int i = 4; i <= 12; i++) {
            if (i == 11 || i == 9) {
                continue;
            } else if (RVALUE[RegId(i)].size() == 0) {
                RegList.push_back(RegId(i));
                return RegList;
            }
        }

        int label = 4 + rand() % 13;
        while (label == 11 || label == 9) {
            label = 4 + rand() % 13;
        }
        RegList.push_back(RegId(label));
    }
    return RegList;
}

RegId RegManager::getReg(Value *value) const {
    auto result = AVALUE.find(value);
    return (result == AVALUE.end() ? RNONE : result->second->reg);
}

int RegManager::getAddress(Value *value) const {
    auto result = AVALUE.find(value);
    // std::cout<<"getAddress of "<<value<<endl;
    if (result == AVALUE.end() || result->second->kind == Entry::RELATIVE) {
        // std::cout<<(result->second->kind==Entry::RELATIVE)<<endl;
        return 0;
    } else {
        return result->second->mem;
    }
}

int RegManager::getOffset(Value *value) const {
    auto result = AVALUE.find(value);
    if (result == AVALUE.end() || result->second->kind == Entry::ABSOLUTE) {
        return 0;
    } else {
        return result->second->mem;
    }
}

void RegManager::setReg(Value *value, RegId reg) {
    // std::cerr << "[debug]<setReg> " + value->getName() + " " + toString(reg) + endl;
    if (AVALUE.find(value) == AVALUE.end()) {
        AVALUE[value] = new Entry();
        AVALUE[value]->kind = Entry::RELATIVE;
    }
    AVALUE[value]->reg = reg;
}

void RegManager::setAddress(Value *value, int mem) {
    if (AVALUE.find(value) == AVALUE.end()) {
        AVALUE[value] = new Entry();
    }
    AVALUE[value]->mem = mem;
    AVALUE[value]->kind = Entry::ABSOLUTE;
}

void RegManager::ShowAVALUE() {
    std::cout << "#######################" << endl;
    for (auto iter = AVALUE.begin(); iter != AVALUE.end(); iter++) {
        std::cout << (iter->first) << " " << (iter->second->mem) << endl;
    }
    std::cout << "#######################" << endl;
}

void RegManager::setOffset(Value *value, int mem) {
    // std::cerr << "[debug]<setOffset> " + value->getName() + " " + to_string(mem) + endl;
    if (AVALUE.find(value) == AVALUE.end()) {
        AVALUE[value] = new Entry();
    }
    AVALUE[value]->mem = mem;
    AVALUE[value]->kind = Entry::RELATIVE;
}

std::string RegManager::storeBack(Value *value) {
    std::string code;
    int addr = getAddress(value);
    int offset = getOffset(value);
    RegId reg = getReg(value);
    // if (addr != 0) {
    //     // 需要用一个寄存器用来放地址，先用一个不安全的方法
    //     RegId rtmp = (reg == R0 ? R1 : R0);
    //     code += space + "push   {" + toString(rtmp) + "}" + endl;
    //     code += space + "movw   " + toString(rtmp) + ", #:lower16:" + value->getName() + endl;
    //     code += space + "movt   " + toString(rtmp) + ", #:upper16:" + value->getName() + endl;
    //     code += space + "str    " + toString(reg) + ", " + toString(rtmp) + endl;
    //     code += space + "pop    {" + toString(rtmp) + "}" + endl;
    // } else {
    //     if (offset == 0) {
    //         *spOffset -= 4;
    //         offset = *spOffset;
    //         code += space + "sub    sp, #4" + " @ " + value->getName() + "(" + to_string(offset) + ")" + endl;
    //         setOffset(value, offset);
    //     }
    //     code += space + "str    " + toString(reg) + ", [fp, #" + to_string(offset) + "]" + endl;
    // }
    if (addr == 0 && offset == 0) {
        *spOffset -= 4;
        offset = *spOffset;
        code += space + "sub    sp, #4" + " @ " + value->getName() + "(" + to_string(offset) + ")" + endl;
        setOffset(value, offset);
        code += space + "str    " + toString(reg) + ", [fp, #" + to_string(offset) + "]" + endl;
    }

    return code;
}

std::pair<std::string, std::vector<RegId>> RegManager::query(Value *dstVal, const std::vector<Value *> &srcVal) {
    std::vector<RegId> regs;
    std::string code;
    if (dstVal != nullptr) {
        RegId reg = getReg(dstVal);
        int addr = getAddress(dstVal);
        int offset = getOffset(dstVal);
        if (reg == RNONE) {
            for (auto r : UserRegs) {
                if (IsEmpty(r)) {
                    reg = r;
                    break;
                }
            }
            if (reg == RNONE) {
                reg = UserRegs[rand() % 11];
                Value *oldVal = getRVALUE(reg)[0];
                code += storeBack(oldVal);
                setReg(oldVal, RNONE);
            }
            clearRVALUE(reg);
            insertRVALUE(reg, dstVal);
            setReg(dstVal, reg);
        }
        regs.push_back(reg);
    }

    for (auto *value : srcVal) {
        RegId reg = getReg(value);
        int addr = getAddress(value);
        int offset = getOffset(value);
        if (reg == RNONE) {
            for (auto r : UserRegs) {
                if (IsEmpty(r)) {
                    reg = r;
                    break;
                }
            }
            if (reg == RNONE) {
                do {
                    reg = UserRegs[rand() % 11];
                    for (auto r : regs) {
                        if (reg == r) {
                            reg = RNONE;
                            break;
                        }
                    }
                } while (reg == RNONE);
                Value *oldVal = getRVALUE(reg)[0];
                code += storeBack(oldVal);
                setReg(oldVal, RNONE);
            }
            if (addr != 0) {
                // global
                // std::cout<<"add some code!"<<endl;
                code += space + "movw   " + toString(reg) + ", #:lower16:" + value->getName() + endl;
                code += space + "movt   " + toString(reg) + ", #:upper16:" + value->getName() + endl;
            } else if (offset != 0) {
                code += space + "ldr    " + toString(reg) + ", [fp, #" + to_string(offset) + "]" + endl;
            } else {
                auto constv = dynamic_cast<sysy::ConstantValue*>(value);
                if (constv != nullptr) {
                    if (constv->getType()->isFloat()) {
                        float number = constv->getFloat();
                        uint16_t highBits = getWordHigh(*(uint32_t *)&number);
                        uint16_t lowBits = getWordLow(*(uint32_t *)&number);
                        code += space + "movw   " + toString(reg) + ", #" + to_string(lowBits) + endl;
                        code += space + "movt   " + toString(reg) + ", #" + to_string(highBits) + endl;
                    } else if (constv->getType()->isInt()) {
                        int number = constv->getInt();
                        uint16_t highBits = getWordHigh(*(uint32_t *)&number);
                        uint16_t lowBits = getWordLow(*(uint32_t *)&number);
                        code += space + "movw   " + toString(reg) + ", #" + to_string(lowBits) + endl;
                        code += space + "movt   " + toString(reg) + ", #" + to_string(highBits) + endl;
                    } else {
                        cerr << "Constant value inaccessible:" << value->getName() << endl;
                    }
                } else {
                    cerr << "Source value inaccessible:" << value->getName() << endl;
                }
            }
        }
        setReg(value, reg);
        clearRVALUE(reg);
        insertRVALUE(reg, value);
        regs.push_back(reg);
    }

    return std::make_pair(code, regs);
}

void RegManager::clear() {
    for (auto reg : UserRegs) {
        clearRVALUE(reg);
    }
    for (auto iter = AVALUE.begin(); iter != AVALUE.end();) {
        if (iter->second->kind != RegManager::Entry::ABSOLUTE) {
            AVALUE.erase(iter++);
        } else {
            ++iter;
        }
    }
}

} // namespace backend