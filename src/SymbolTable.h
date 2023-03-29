/**
 * @file SymbolTable.h
 * @brief 符号表的声明。定义见SymbolTable.cpp
 * @author Rhein_E
 * @version 1.0
 * @date 2023-03-28
 * 
 * @details 以栈的方式组织多个符号表，每个符号表对应一个域。
 * 使用时只需要创建一个SymbolTable类。完整的示例见文件末尾。
 */
#include <iostream>
#include <map>
#include <string>
#include <iomanip>
#include <vector>

#include "IR.h"

namespace sysy {

class SymbolTable {
using SubTable=std::map<std::string, Value *>;
private:
    std::vector<SubTable *> table;

public:
    /**
     * @brief 插入符号
     * @param[in] name 符号名字
     * @return true 插入成功 \\
     * @return false 插入失败，通常表明有名字冲突
     * @author Rhein_E
     * @example table.insert("x", (sysy::Value *) 0x1000080);
     */
    bool insert(const std::string &name, Value *ptr);

    /**
     * @brief  查询符号
     * @param[in] name 符号的名字
     * @return 返回一个指向Value对象的指针，若为nullptr表示没有找到对应名字的符号
     * @author Rhein_E
     */
    Value *query(const std::string &name);

    /**
     * @brief 创建一个子表并入栈
     */
    void newTable();

    /**
     * @brief 销毁栈顶符号表
     */
    void destroyTop();

    /**
     * @brief 打印符号表（调试使用）
     * @author Rhein_E
     */
    void view() const;
};

} // namespace sysy

