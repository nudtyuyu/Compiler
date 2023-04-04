#pragma once

#include <memory>
#include "IR.h"
#include "IRBuilder.h"
#include "SysYBaseVisitor.h"
#include "SysYParser.h"
#include "IDTable.h"

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
    IDTable idt;

public:
    SysYIRGenerator() = default;

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

    // virtual std::any visitFuncDef(SysYParser::FuncDefContext *context) override;

    // virtual std::any visitFuncType(SysYParser::FuncTypeContext *context) override;

    // virtual std::any visitFuncFParams(SysYParser::FuncFParamsContext *context) override;

    // virtual std::any visitFuncFParam(SysYParser::FuncFParamContext *context) override;

    // virtual std::any visitBlock(SysYParser::BlockContext *context) override;

    // virtual std::any visitBlockItem(SysYParser::BlockItemContext *context) override;

    virtual std::any visitStmt(SysYParser::StmtContext *context) override;

    virtual std::any visitExp(SysYParser::ExpContext *context) override;

    virtual std::any visitCond(SysYParser::CondContext *context) override;

    virtual std::any visitLVal(SysYParser::LValContext *context) override;

    virtual std::any visitPrimaryExp(SysYParser::PrimaryExpContext *context) override;

    virtual std::any visitNumber(SysYParser::NumberContext *context) override;

    // virtual std::any visitString(SysYParser::StringContext *context) override;

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

}; // class SysYIRGenerator

} // namespace sysy
