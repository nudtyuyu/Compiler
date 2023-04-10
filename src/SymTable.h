/**
 * @file SymTable.h
 * @brief 符号表的声明。定义见SymTable.cpp
 */
#include <iostream>
#include <map>
#include <string>
#include <iomanip>
#include <vector>

#include "IR.h"

namespace sysy {

class Entry {
public:
	Value *value;       ///< 若为变量，保存pointer；若为常量，保存值。
	bool isConst;       ///< 0为变量，1为常量。
	Entry(Value *_value):value(_value) {
        isConst = !(value->isPointer());
    }
};

class SymTable {
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
     */
    bool insert(const std::string &name, Entry entry);

    /**
     * @brief  查询符号，修改符号表须借助该函数实现
     * @param[in] name 符号的名字
     * @return 返回一个指向Value对象的指针，若为nullptr表示没有找到对应名字的符号
     */
    Entry *query(const std::string &name);

    void newTable();
    void destroyTop();
    void view() const;
};

class AEntry {
public:
	Value *ptr;         ///< 数组首地址
    InitList *value;    ///< 数组值（类名字叫初值，实际上可不是初值()）
	// int ValType; // 0 is constantVal, 1 is varVal
	// //int DataType; //0 is int, 1 is float;
	std::vector<Value*> dims; // dim info

private:
    void check(InitList *cur, int depth) const {
        assert(depth < dims.size());
        assert(cur->getNumOperands() < dims[depth]);
        for (auto use : cur->getOperands()) {
            auto *child = use.getValue();
            if (child->getType() == Type::getInitListType()) {
                check(dynamic_cast<InitList *>(child), depth + 1);
            }
        }
    }

public:
	AEntry() = default;
	AEntry(Value *_ptr, InitList* _value, std::vector<Value*> &_dims):ptr(_ptr), value(_value), dims(_dims)
	{
        // 检查是否越界
        check(value, 0);

		// dims.clear();
		// std::cout<<"Init a AEntry!"<<std::endl;
		// int L = dim.size();
		// std::cout<<"L: "<<L<<std::endl;
		// for(int i=0;i<L;i++)
		// {
		// 	std::cout<<"i: "<<i<<std::endl;
		// 	dims.push_back(dim[i]);
		// }
		// int Len = data.size();
		// for(int i=0;i<Len;i++)
		// {
		// 	idata.push_back(data[i]);
		// }
		std::cout<<"Init Finish!"<<std::endl;
	};
}; // class AEntry

class ArrayTable {
using SubTable=std::map<std::string, AEntry>;
private:
    std::vector<SubTable *> table;

public:
    /**
     * @brief 插入符号
     * @param[in] name 符号名字
     * @return true 插入成功 \\
     * @return false 插入失败，通常表明有名字冲突
     */
    bool insert(const std::string &name, AEntry entry);

    /**
     * @brief  查询符号
     * @param[in] name 符号的名字
     * @return 返回一个指向AEntry对象的指针，若为nullptr表示没有找到对应名字的符号
     */
    AEntry *query(const std::string &name);

    void newTable();
    void destroyTop();
    void view() const;
};

} // namespace sysy