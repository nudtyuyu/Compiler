#include "SymbolTable.h"

namespace sysy {

SymbolInfo::SymbolInfo() {
    type = NONE;
    addr = 0;
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

/* example
#include "SymbolTable.h"

using namespace sysy;

SymbolTable tb;

int main() {
    // 插入一个整型x，并设置信息
    if (!tb.insert("x"))
        std::cout << "error\n";
    else {
        SymbolInfo *pinfo = tb.query("x");
        pinfo->type = INT;
        pinfo->addr = 0x4000064;
    }

    // 插入一个函数func，并设置信息
    if (!tb.insert("func"))
        std::cout << "error\n";
    else {
        SymbolInfo *pinfo = tb.query("func");
        pinfo->type = FUNC;
        pinfo->addr = 0x1000000;
    }

    tb.view();

    return 0;
}
*/