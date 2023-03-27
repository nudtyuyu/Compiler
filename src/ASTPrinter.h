#pragma once

#include "SysYBaseVisitor.h"

class ASTPrinter : public SysYBaseVisitor {
public:
  std::any visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override;
//   std::any visitExpAsRParam(SysYParser::ExpAsRParamContext *ctx) override;
//   std::any visitStringAsRParam(SysYParser::StringAsRParamContext *ctx) override;
  
  std::any visitNumber(SysYParser::NumberContext *ctx) override;
  std::any visitString(SysYParser::StringContext *ctx) override;
  //std::any visitExp(SysYParser::ExpContext *ctx) override;
  std::any visitLVal(SysYParser::LValContext *ctx) override;
  std::any visitPrimaryExp(SysYParser::PrimaryExpContext *ctx) override;
  std::any visitUnaryOp(SysYParser::UnaryOpContext *ctx) override;
  std::any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override;
  std::any visitExp(SysYParser::ExpContext *ctx) override;
  std::any visitAddExp(SysYParser::AddExpContext *ctx) override;
  std::any visitMulExp(SysYParser::MulExpContext *ctx) override;
  std::any visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override;
  std::any visitFuncFParam(SysYParser::FuncFParamContext *ctx) override;
  std::any visitBType(SysYParser::BTypeContext *ctx) override;
  std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override;
  std::any visitConstExp(SysYParser::ConstExpContext *ctx) override;
  std::any visitConstInitVal(SysYParser::ConstInitValContext *ctx) override;
  std::any visitConstDef(SysYParser::ConstDefContext *ctx) override;
  std::any visitInitVal(SysYParser::InitValContext *ctx) override;
  std::any visitVarDef(SysYParser::VarDefContext *ctx) override;
  std::any visitVarDecl(SysYParser::VarDeclContext *ctx) override;
  std::any visitConstDecl(SysYParser::ConstDeclContext *ctx) override;
  std::any visitDecl(SysYParser::DeclContext *ctx) override;
  std::any visitRelExp(SysYParser::RelExpContext *ctx) override;
  std::any visitEqExp(SysYParser::EqExpContext *ctx) override;
  std::any visitLAndExp(SysYParser::LAndExpContext *ctx) override;
  std::any visitLOrExp(SysYParser::LOrExpContext *ctx) override;
  std::any visitCond(SysYParser::CondContext *ctx) override;
  std::any visitStmt(SysYParser::StmtContext *ctx) override;
  std::any visitBlockItem(SysYParser::BlockItemContext *ctx) override;
  std::any visitBlock(SysYParser::BlockContext *ctx) override;
  std::any visitFuncDef(SysYParser::FuncDefContext *ctx) override;
  std::any visitCompUnit(SysYParser::CompUnitContext *ctx) override;
};
