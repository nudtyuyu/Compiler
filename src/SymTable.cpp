#include "SymTable.h"

namespace sysy {

SymTable::Entry *SymTable::insert(const std::string &name, Value *value, const std::vector<Value *> &dims, Value *initValue, bool constant) {
    Entry *entry = new Entry(value, dims, initValue, constant);
    if (entries.find(name) != entries.end()) {
        return nullptr;
    } else {

        entries[name] = entry;
        return entry;
    }
}

SymTable::Entry *SymTable::query(const std::string &name) {
    SymTable *pTable = this;
    while (pTable != nullptr) {
        if (pTable->entries.find(name) != pTable->entries.end())
            return pTable->entries[name];
        pTable = pTable->parent;
    }
    return nullptr;
}

} // namespace sysy