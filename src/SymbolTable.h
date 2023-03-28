/**
 * @file SymbolTable.h
 * @brief 符号表的声明。定义见SymbolTable.cpp
 * @author Rhein_E
 * @version 1.0
 * @date 2023-03-28
 */
#include <iostream>
#include <map>
#include <string>
#include <iomanip>

#include "IR.h"

namespace sysy {

class SymbolTable {
private:
    std::map<std::string, Value *> table;

public:
    /**
     * @brief 插入符号
     * @param name 符号名字
     * @return true 插入成功 \\
     * @return false 插入失败，通常表明有名字冲突
     * @author Rhein_E
     */
    bool insert(const std::string &name, const Value *ptr);

    /**
     * @brief  查询符号
     * @param name 符号的名字
     * @return 返回一个指向Value对象的指针，若为nullptr表示没有找到对应名字的符号
     * @author Rhein_E
     */
    Value *query(const std::string &name);

    /**
     * @brief 打印符号表（调试使用）
     * @author Rhein_E
     */
    void view() const;
};

} // namespace sysy

