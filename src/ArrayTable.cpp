/**
 * @file ArrayTable.cpp
 * @brief 符号表的定义。声明见ArrayTable.h
 * @author Rhein_E
 * @version 1.0
 * @date 2023-03-28
 *
 * @details 通过std::map将符号的名字(std::string)映射到指向Value对象的指针。
 */
#include "ArrayTable.h"

namespace sysy {

bool ArrayTable::insert(const std::string &name, AEntry ptr) {
    auto &topTable = *table.back();
    if (topTable.find(name) != topTable.end())
        return false;
    topTable[name] = ptr;
    return true;
}

AEntry *ArrayTable::query(const std::string &name) {
    for (auto it = table.rbegin(); it != table.rend(); ++it)
        if ((*it)->find(name) != (*it)->end())
            return &(*it)->at(name);
    return nullptr;
}

void ArrayTable::newTable() {
    table.push_back(new SubTable2());
}

void ArrayTable::destroyTop() {
    if (!table.empty()) {
        delete table.back();
        table.pop_back();
    }
}

void ArrayTable::view() const {
    for (auto *pSubTable : table) {
        std::cout << "\n+---------------+---------------+\n";
        std::cout << std::right;
        std::cout << "|" << std::setw(15) << "Name"
                  << "|" << std::setw(15) << "Pointer"
                  << "|\n";
        std::cout << "+---------------+---------------+\n";
        for (auto &pair : *pSubTable) {
            std::cout << "|" << std::setw(15) << pair.first;
            std::cout << "|" << std::setw(15) << std::hex << std::showbase << (unsigned long long)pair.second.ptr << "|\n";
        }
        std::cout.unsetf(std::cout.flags());
        std::cout << "+---------------+---------------+\n";
    }
}

} // namespace sysy
