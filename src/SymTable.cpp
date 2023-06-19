#include "SymTable.h"

namespace sysy {

SymTableEntry *SymTable::insert(const std::string &name, Value *value, const std::vector<Value *> &dims, Value *initValue, bool constant, SymTableEntry::Kind kind) {
    SymTableEntry *entry = new SymTableEntry(value, dims, initValue, constant, kind);
    if (entries.find(name) != entries.end()) {
        return nullptr;
    } else {

        entries[name] = entry;
        return entry;
    }
}

SymTableEntry *SymTable::query(const std::string &name) {
    SymTable *pTable = this;
    while (pTable != nullptr) {
        if (pTable->entries.find(name) != pTable->entries.end())
            return pTable->entries[name];
        pTable = pTable->parent;
    }
    return nullptr;
}

} // namespace sysy