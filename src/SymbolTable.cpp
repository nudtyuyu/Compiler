/**
 * @file SymbolTable.cpp
 * @brief 符号表的定义。声明见SymbolTable.h
 * @author Rhein_E
 * @version 1.0
 * @date 2023-03-28
 * 
 * @details 实现方式：通过std::map将符号的名字(std::string)映射到SymbolInfo对象（以指针的形式）。
 */
#include "SymbolTable.h"

namespace sysy {

SymbolInfo::SymbolInfo() {
    type = NONE;
    addr = 0x0;
}
bool SymbolTable::insert(const std::string &name) {
    if (table.find(name) != table.end())
        return false;
    table[name] = new SymbolInfo();
    return true;
}

SymbolInfo *SymbolTable::query(const std::string &name) {
    if (table.find(name) != table.end())
        return table[name];
    return nullptr;
}

void SymbolTable::view() const {
    std::cout << std::right;
    std::cout << "|" << std::setw(15) << "Name"
              << "|" << std::setw(8) << "Type"
              << "|" << std::setw(15) << "Address"
              << "|\n";
    for (auto &pair : table) {
        std::cout << "|" << std::setw(15) << pair.first << "|" << std::setw(8);
        switch (pair.second->type) {
            case NONE:
                std::cout << "NONE";
                break;
            case INT:
                std::cout << "INT";
                break;
            case FLOAT:
                std::cout << "FLOAT";
                break;
            case FUNC:
                std::cout << "FUNC";
                break;
        }
        std::cout << "|" << std::setw(15) << std::hex << std::showbase << pair.second->addr << "|\n";
    }
    std::cout.unsetf(std::cout.flags());
}

SymbolTable::~SymbolTable() {
    for (auto &pair : table)
        delete pair.second;
}

} // namespace sysy
