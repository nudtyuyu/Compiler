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