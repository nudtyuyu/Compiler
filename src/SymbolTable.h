/**
 * @file SymbolTable.h
 * @brief 符号表的声明。定义见SymbolTable.cpp
 * @author Rhein_E
 * @version 1.0
 * @date 2023-03-28
 * 
 * @details 总体逻辑：插入符号和编辑符号的信息互为独立的操作。
 * 对符号信息的编辑首先通过调用query获取对应SymbolInfo对象的指针，然后直接修改其成员。
 * 下面的代码是插入一个整型并设置其信息的示例。可编译运行的完整示例见本文件末尾。
 * @code {.cpp}
 *  if (!tb.insert("x"))
        std::cout << "error\n";
    else {
        SymbolInfo *pinfo = tb.query("x");
        pinfo->type = INT;
        pinfo->addr = 0x4000064;
    }
 * @endcode
 */
#include <iostream>
#include <map>
#include <string>
#include <iomanip>

namespace sysy {

enum typecode_t { NONE, INT, FLOAT, FUNC };

struct SymbolInfo {
    typecode_t type;
    int addr;

    SymbolInfo();
};

class SymbolTable {
private:
    std::map<std::string, SymbolInfo *> table;

public:
    /**
     * @brief 插入符号
     * @param name 符号名字
     * @return true 插入成功 \\
     * @return false 插入失败，通常表明有名字冲突
     * @author Rhein_E
     */
    bool insert(const std::string &name);

    /**
     * @brief  查询符号
     * @param name 符号的名字
     * @return 返回一个指向SymbolTableEntry对象的指针，若为nullptr表示没有找到对应名字的符号
     * @author Rhein_E
     */
    SymbolInfo *query(const std::string &name);

    /**
     * @brief 打印符号表（调试使用）
     * @author Rhein_E
     */
    void view() const;

    /**
     * @brief Destroy the Symbol Table object
     * @author Rhein_E
     */
    ~SymbolTable();
};

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