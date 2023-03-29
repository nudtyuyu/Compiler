#include"SymbolTable.h"
#include "IR.h"
int main() {
    sysy::SymbolTable table;

    // 一级符号表（全局）
    table.newTable();
    table.insert("x", (sysy::Value *) 0x1000080);
    table.insert("func", (sysy::Value *) 0x4000068);

    // 二级符号表（函数）
    table.newTable();
    table.insert("abc", (sysy::Value *) 0x8000040);
    table.insert("w", (sysy:: Value *) 0x8000008);
    
    table.view();

    // 查询符号表
    auto *ptr = table.query("abc");
    std::cout << "The pointer of \"abc\" is : ";
    std::cout << std::hex << std::showbase << (unsigned long long)ptr << "\n";

    return 0;
}