#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace sysy {

class Value;
class InitList;

class SymTable {
public:
    class Entry;

private:
    SymTable *parent;
    std::map<std::string, Entry *> entries;

public:
    SymTable() = delete;
    SymTable(SymTable *_parent):parent(_parent), entries() {}

public:
    Entry *insert(const std::string &name, Value *value, const std::vector<Value *> &dims, Value *initValue, bool constant);
    Entry *query(const std::string &name);

};

class SymTable::Entry {
private:
    Value *value;       // 数组存首地址；基本类型变量存地址；基本类型常量存值。
    std::vector<Value *> dims;
    Value *initValue;   // 初始值
    bool constant;
    int addr_offset;    // 汇编中的地址。全局变量为绝对地址，局部变量为相对(fp)地址。

public:
    Entry() = delete;
    Entry(Value *_value, const std::vector<Value *> &_dims, Value *_initValue, bool _constant)
        :value(_value), dims(_dims), initValue(_initValue), constant(_constant), addr_offset(0) {}

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
};

} // namespace sysy
