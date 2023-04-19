#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include "IR.h"
#include "IRBuilder.h"
#include "SysYBaseVisitor.h"
#include "SysYParser.h"
#include "SymTable.h"
namespace sysy {

class SysYIRGenerator : public SysYBaseVisitor {
private:
    std::unique_ptr<Module> module;
    IRBuilder builder;
    std::vector<BasicBlock *> loopEntry;
    std::vector<BasicBlock *> loopExit;

public:
    bool GlobalVal;
    bool LocalVal;
    SymTable symTable;
    ArrayTable arrayTable;
    std::vector<int> iDims;
    int ConstInitListName;
    int InitListName;
    int Layer;// visit initvalue layer for var Array
    int ConstLayer; //visit initvalue layer for const Array
    std::string ArrayName;
    std::string ConstArrayName;

public:
    SysYIRGenerator() = default;
public:
    void AssignArray(std::vector<std::vector<Value*> >Values,std::vector<int>Dims,int dim);
    std::any getInitValues(std::vector<std::vector<Value*> >Values,std::vector<int>Dims,int dim,int flag);
    Value *getElementPtr(AllocaInst *base, std::vector<Value *> indices);

public:
    virtual std::any visitCompUnit(SysYParser::CompUnitContext *context) override;

    virtual std::any visitDecl(SysYParser::DeclContext *context) override;

    virtual std::any visitConstDecl(SysYParser::ConstDeclContext *context) override;

    virtual std::any visitBType(SysYParser::BTypeContext *context) override;

    // virtual std::any visitConstDef(SysYParser::ConstDefContext *context) override;

    virtual std::any visitConstInitVal(SysYParser::ConstInitValContext *context) override;

    virtual std::any visitVarDecl(SysYParser::VarDeclContext *context) override;

    // virtual std::any visitVarDef(SysYParser::VarDefContext *context) override;

    virtual std::any visitInitVal(SysYParser::InitValContext *context) override;

    virtual std::any visitFuncDef(SysYParser::FuncDefContext *context) override;

    virtual std::any visitFuncType(SysYParser::FuncTypeContext *context) override;

    // virtual std::any visitFuncFParams(SysYParser::FuncFParamsContext *context) override;

    // virtual std::any visitFuncFParam(SysYParser::FuncFParamContext *context) override;

    virtual std::any visitBlock(SysYParser::BlockContext *context) override;

    virtual std::any visitBlockItem(SysYParser::BlockItemContext *context) override;

    virtual std::any visitStmt(SysYParser::StmtContext *context) override;

    virtual std::any visitExp(SysYParser::ExpContext *context) override;

    virtual std::any visitCond(SysYParser::CondContext *context) override;

    /**
     * @brief 
     * @param context 
     * @return  std::any -> std::pair<Value *ptr, Value *val>
     * @return  若为常量, ptr为nullptr, val为ConstantValue*;
     * @return  若为变量, ptr为指针值, val为nullptr.(不返回变量值是因为涉及到数组元素修改导致值作废的问题)
     */
    virtual std::any visitLVal(SysYParser::LValContext *context) override;

    /**
     * @brief 
     * @param context 
     * @return std::any -> sysy::Value *
     * @return 若为变量值，返回基类指针Value *
     * @return 若为常量值，返回子类指针ConstantValue *
     */
    virtual std::any visitPrimaryExp(SysYParser::PrimaryExpContext *context) override;

    virtual std::any visitNumber(SysYParser::NumberContext *context) override;

    virtual std::any visitUnaryExp(SysYParser::UnaryExpContext *context) override;

    // virtual std::any visitUnaryOp(SysYParser::UnaryOpContext *context) override;

    // virtual std::any visitFuncRParams(SysYParser::FuncRParamsContext *context) override;

    virtual std::any visitMulExp(SysYParser::MulExpContext *context) override;

    virtual std::any visitAddExp(SysYParser::AddExpContext *context) override;

    virtual std::any visitRelExp(SysYParser::RelExpContext *context) override;

    virtual std::any visitEqExp(SysYParser::EqExpContext *context) override;

    virtual std::any visitLAndExp(SysYParser::LAndExpContext *context) override;

    virtual std::any visitLOrExp(SysYParser::LOrExpContext *context) override;

    virtual std::any visitConstExp(SysYParser::ConstExpContext *context) override;

public:
    void generateCode(std::ostream &out) const {
        module->generateCode(out);
    }
    std::any GenerateZero(int Lay, std::string name);

    std::string newTemp() {
        static std::stringstream ss;
        static int tmpCount = 0;
        ss.str("");
        ss << "T" << (tmpCount++);
        return ss.str();
    }

    std::string newBlockName() {
        static std::stringstream ss;
        static int blockCount = 0;
        ss.str("");
        ss << ".L" << (blockCount++);
        return ss.str();
    }

}; // class SysYIRGenerator

} // namespace sysy
