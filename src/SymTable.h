#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

namespace sysy {

class Value;
class InitList;

class SymTable;
class SymTableEntry;


class SymTableEntry {
public:
    enum Kind {GLOBAL, LOCAL, ARGUMENT};
private:
    Value *value;       // 数组存首地址；基本类型变量存地址；基本类型常量存值。
    std::vector<Value *> dims;
    Value *initValue;   // 初始值
    bool constant;
    int addr_offset;    // 汇编中的地址。全局变量为绝对地址，局部变量为相对(fp)地址。考虑到已分配的不会是0，用0表示未分配。
    Kind kind;

public:
    SymTableEntry() = delete;
    SymTableEntry(Value *_value, const std::vector<Value *> &_dims, Value *_initValue, bool _constant, Kind _kind)
        :value(_value), dims(_dims), initValue(_initValue), constant(_constant), addr_offset(0), kind(_kind) {}

public:
    Value *getValue() const {
        return value;
    }
    const std::vector<Value *> getDims() const {
        return dims;
    }
    Value *getInitValue() const {
        return initValue;
    }
    bool isConstant() const {
        return constant;
    }
    void setOffset(int offset) {
        addr_offset = offset;
    }
    int getOffset() const {
        return addr_offset;
    }
    bool isGlobal() const {
        return (kind == GLOBAL);
    }
    bool isArgument() const {
        return (kind == ARGUMENT);
    }
    bool isLocal() const {
        return (kind == LOCAL);
    }
};

class SymTable {
private:
    SymTable *parent;
    std::map<std::string, SymTableEntry *> entries;

public:
    SymTable() = delete;
    SymTable(SymTable *_parent):parent(_parent), entries() {}

public:
    SymTableEntry *insert(const std::string &name, Value *value, const std::vector<Value *> &dims, Value *initValue, bool constant, SymTableEntry::Kind kind);
    SymTableEntry *query(const std::string &name);

public:
    const auto &getEntries() const {
        return entries;
    }
};

} // namespace sysy
