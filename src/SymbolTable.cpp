/**
 * @file SymbolTable.cpp
 * @brief 符号表的定义。声明见SymbolTable.h
 * @author Rhein_E
 * @version 1.0
 * @date 2023-03-28
 * 
 * @details 通过std::map将符号的名字(std::string)映射到指向Value对象的指针。
 */
#include "SymbolTable.h"

namespace sysy {

bool SymbolTable::insert(const std::string &name, const Value *ptr) {
    if (table.find(name) != table.end())
        return false;
    table[name] = ptr;
    return true;
}

Value *SymbolTable::query(const std::string &name) {
    if (table.find(name) != table.end())
        return table[name];
    return nullptr;
}

void SymbolTable::view() const {
    std::cout << std::right;
    std::cout << "|" << std::setw(15) << "Name"
              << "|" << std::setw(15) << "Address"
              << "|\n";
    for (auto &pair : table) {
        std::cout << "|" << std::setw(15) << pair.first
        std::cout << "|" << std::setw(15) << std::hex << std::showbase << (unsigned int)pair.second->addr << "|\n";
    }
    std::cout.unsetf(std::cout.flags());
}

} // namespace sysy
