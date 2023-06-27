#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
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
    bool globalScope;
    // std::vector<int> iDims;
    // std::vector<int> iCounts;
    //int iCounts;
    // std::vector<int> MyCount;
    // int ConstInitListName;
    int InitListName;
    int nowElement;
    // int layer;// visit initvalue layer for var Array
    // int constLayer; //visit initvalue layer for const Array
    // std::string ArrayName;
    // std::string ConstArrayName;
    // int Rows;
    // int Cols;

public:
    SysYIRGenerator() = default;

public:
    // void AssignArray(std::vector<std::vector<Value*> >Values,std::vector<int>Dims,int dim);
    std::any getInitValues(std::vector<std::vector<Value*> >Values,std::vector<int>Dims,int dim,int flag);

    Value *getElementPtr(Value *base, const std::vector<Value *> &dims, const std::vector<Value *> &indices);

public:
  Module *get() const { return module.get(); }


public:
    virtual std::any visitCompUnit(SysYParser::CompUnitContext *context) override;

    virtual std::any visitDecl(SysYParser::DeclContext *context) override;

    virtual std::any visitConstDecl(SysYParser::ConstDeclContext *context) override;

    virtual std::any visitBType(SysYParser::BTypeContext *context) override;

    // virtual std::any visitConstDef(SysYParser::ConstDefContext *context) override;

    // deprecated
    // virtual std::any visitConstInitVal(SysYParser::ConstInitValContext *context) override;

    std::any visitConstInitVal(SysYParser::ConstInitValContext *context, const std::vector<int> &dims, int layer);

    virtual std::any visitVarDecl(SysYParser::VarDeclContext *context) override;

    // virtual std::any visitVarDef(SysYParser::VarDefContext *context) override;

    // deprecated
    // virtual std::any visitInitVal(SysYParser::InitValContext *context) override;

    std::any visitInitVal(SysYParser::InitValContext *context, const std::vector<int> &dims, int layer);

    void storeLocalArray(Value *base, Value *initVal);

    virtual std::any visitFuncDef(SysYParser::FuncDefContext *context) override;

    virtual std::any visitFuncType(SysYParser::FuncTypeContext *context) override;

    // virtual std::any visitFuncFParams(SysYParser::FuncFParamsContext *context) override;

    // virtual std::any visitFuncFParam(SysYParser::FuncFParamContext *context) override;

    virtual std::any visitBlock(SysYParser::BlockContext *context) override;

    virtual std::any visitBlockItem(SysYParser::BlockItemContext *context) override;

    virtual std::any visitStmt(SysYParser::StmtContext *context) override;

    virtual std::any visitExp(SysYParser::ExpContext *context) override;

    virtual std::any visitCond(SysYParser::CondContext *context) override;

    virtual std::any visitLVal(SysYParser::LValContext *context) override;

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
    
    // void visitConstInitVal2(SysYParser::ConstInitValContext *ctx,InitList* values);
    // void visitInitVal2(SysYParser::InitValContext *ctx,InitList* values);
    
    // std::any GenerateZero(int Lay, std::string name);

    std::string newTemp() {
        static std::stringstream ss;
        static int tmpCount = 0;
        ss.str("");
        ss << "%" << (tmpCount++);
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
