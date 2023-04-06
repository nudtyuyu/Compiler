/**
 * @file IDTable.h
 * @brief 符号表的声明。定义见IDTable.cpp
 * @author Rhein_E
 * @version 1.0
 * @date 2023-03-28
 * 
 * @details 以栈的方式组织多个符号表，每个符号表对应一个域。
 * 使用时只需要创建一个IDTable类。完整的示例见文件末尾。
 */
#include <iostream>
#include <map>
#include <string>
#include <iomanip>
#include <vector>

#include "IR.h"

namespace sysy {

struct Entry
{
	Value * ptr;
	int ValType; // 0 is constantVal, 1 is varVal
	union
	{
		int iValue;
		float fValue;
	};
	Entry() = default;
	Entry(Value* v,int t, int value):ptr(v),ValType(t),iValue(value){};
	Entry(Value* v,int t, float value):ptr(v),ValType(t),fValue(value){};
	
};

class IDTable {
using SubTable=std::map<std::string, Entry>;
private:
    std::vector<SubTable *> table;

public:
    /**
     * @brief 插入符号
     * @param[in] name 符号名字
     * @param[in] entry 符号表项 
     * @return true 插入成功 \\
     * @return false 插入失败，通常表明有名字冲突
     * @author Rhein_E
     * @example table.insert("x", (sysy::Entry *) 0x1000080);
     */
    bool insert(const std::string &name, Entry entry);

    /**
     * @brief  查询符号
     * @param[in] name 符号的名字
     * @return 返回一个指向Value对象的指针，若为nullptr表示没有找到对应名字的符号
     * @author Rhein_E
     */
    Entry *query(const std::string &name);

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

/*
#include"IDTable.h"
#include "IR.h"
int main() {
    sysy::IDTable table;

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
*/
