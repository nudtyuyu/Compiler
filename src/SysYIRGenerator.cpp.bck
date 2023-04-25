#include "IR.h"
#include <any>
#include <iostream>
#include <memory>
#include <vector>
using namespace std;
#include "Diagnostic.h"
#include "SysYIRGenerator.h"

namespace sysy {

any SysYIRGenerator::visitModule(SysYParser::ModuleContext *ctx) {
  // global scople of the module
  SymbolTable::ModuleScope scope(symbols);
  // create the IR module
  auto pModule = new Module();
  assert(pModule);
  module.reset(pModule);
  // generates globals and functions
  visitChildren(ctx);
  // return the IR module
  return pModule;
}

any SysYIRGenerator::visitDecl(SysYParser::DeclContext *ctx) {
  // global and local declarations are handled in different ways
  return symbols.isModuleScope() ? visitGlobalDecl(ctx) : visitLocalDecl(ctx);
}

any SysYIRGenerator::visitGlobalDecl(SysYParser::DeclContext *ctx) {
  error(ctx, "not implemented yet");
  std::vector<Value *> values;
  bool isConst = ctx->CONST();
  auto type = any_cast<Type *>(visitBtype(ctx->btype()));
  for (auto varDef : ctx->varDef()) {
    auto name = varDef->lValue()->ID()->getText();
    vector<Value *> dims;
    for (auto exp : varDef->lValue()->exp())
      dims.push_back(any_cast<Value *>(exp->accept(this)));
    auto init = varDef->ASSIGN()
                    ? any_cast<Value *>(visitInitValue(varDef->initValue()))
                    : nullptr;
    values.push_back(module->createGlobalValue(name, type, dims, init));
  }
  // FIXME const
  return values;
}

any SysYIRGenerator::visitLocalDecl(SysYParser::DeclContext *ctx) {
  // a single declaration statement can declare several variables,
  // collect them in a vector
  std::vector<Value *> values;
  // obtain the declared type
  auto type = Type::getPointerType(any_cast<Type *>(visitBtype(ctx->btype())));
  // handle variables
  for (auto varDef : ctx->varDef()) {
    // obtain the variable name and allocate space on the stack
    auto name = varDef->lValue()->ID()->getText();
    auto alloca = builder.createAllocaInst(type, {}, name);
    // update the symbol table
    symbols.insert(name, alloca);
    // if an initial value is given, create a store instruction
    if (varDef->ASSIGN()) {
      auto value = any_cast<Value *>(visitInitValue(varDef->initValue()));
      auto store = builder.createStoreInst(value, alloca);
    }
    // collect the created variable (pointer)
    values.push_back(alloca);
  }
  return values;
}

any SysYIRGenerator::visitFunc(SysYParser::FuncContext *ctx) {
  // obtain function name and type signature
  auto name = ctx->ID()->getText();
  vector<Type *> paramTypes;
  vector<string> paramNames;
  if (ctx->funcFParams()) {
    auto params = ctx->funcFParams()->funcFParam();
    for (auto param : params) {
      paramTypes.push_back(any_cast<Type *>(visitBtype(param->btype())));
      paramNames.push_back(param->ID()->getText());
    }
  }
  Type *returnType = any_cast<Type *>(visitFuncType(ctx->funcType()));
  auto funcType = Type::getFunctionType(returnType, paramTypes);
  // create the IR function
  auto function = module->createFunction(name, funcType);
  // update the symbol table
  symbols.insert(name, function);
  // create the function scope
  SymbolTable::FunctionScope scope(symbols);
  // create the entry block with the same parameters as the function,
  // and update the symbol table
  auto entry = function->getEntryBlock();
  for (auto i = 0; i < paramTypes.size(); ++i) {
    auto arg = entry->createArgument(paramTypes[i], paramNames[i]);
    symbols.insert(paramNames[i], arg);
  }
  // setup the instruction insert point
  builder.setPosition(entry, entry->end());
  // generate the function body
  visitBlockStmt(ctx->blockStmt());
  return function;
}

any SysYIRGenerator::visitBtype(SysYParser::BtypeContext *ctx) {
  return ctx->INT() ? Type::getIntType() : Type::getFloatType();
}
any SysYIRGenerator::visitFuncType(SysYParser::FuncTypeContext *ctx) {
  return ctx->INT()
             ? Type::getIntType()
             : (ctx->FLOAT() ? Type::getFloatType() : Type::getVoidType());
}

any SysYIRGenerator::visitBlockStmt(SysYParser::BlockStmtContext *ctx) {
  // the insert point has already been setup
  for (auto item : ctx->blockItem())
    visitBlockItem(item);
  // return the corresponding IR block
  return builder.getBasicBlock();
}

any SysYIRGenerator::visitAssignStmt(SysYParser::AssignStmtContext *ctx) {
  // generate the rhs expression
  auto rhs = any_cast<Value *>(ctx->exp()->accept(this));
  // get the address of the lhs variable
  auto lValue = ctx->lValue();
  auto name = lValue->ID()->getText();
  auto pointer = symbols.lookup(name);
  if (not pointer)
    error(ctx, "undefined variable");
  // update the variable
  Value *store = builder.createStoreInst(rhs, pointer);
  return store;
}

any SysYIRGenerator::visitNumberExp(SysYParser::NumberExpContext *ctx) {
  Value *result = nullptr;
  assert(ctx->number()->ILITERAL() or ctx->number()->FLITERAL());
  if (auto iLiteral = ctx->number()->ILITERAL())
    result = ConstantValue::get(std::stoi(iLiteral->getText()));
  else
    result =
        ConstantValue::get(std::stof(ctx->number()->FLITERAL()->getText()));
  return result;
}

any SysYIRGenerator::visitLValueExp(SysYParser::LValueExpContext *ctx) {
  auto name = ctx->lValue()->ID()->getText();
  Value *value = symbols.lookup(name);
  if (not value)
    error(ctx, "undefined variable");
  if (isa<GlobalValue>(value) or isa<AllocaInst>(value))
    value = builder.createLoadInst(value);
  return value;
}

any SysYIRGenerator::visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) {
  // generate the operands
  auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
  auto rhs = any_cast<Value *>(ctx->exp(1)->accept(this));
  // create convert instruction if needed
  auto lhsTy = lhs->getType();
  auto rhsTy = rhs->getType();
  auto type = getArithmeticResultType(lhsTy, rhsTy);
  if (lhsTy != type)
    lhs = builder.createIToFInst(lhs);
  if (rhsTy != type)
    rhs = builder.createIToFInst(rhs);
  // create the arithmetic instruction
  Value *result = nullptr;
  if (ctx->ADD())
    result = type->isInt() ? builder.createAddInst(lhs, rhs)
                           : builder.createFAddInst(lhs, rhs);
  else
    result = type->isInt() ? builder.createSubInst(lhs, rhs)
                           : builder.createFSubInst(lhs, rhs);
  return result;
}

any SysYIRGenerator::visitMultiplicativeExp(
    SysYParser::MultiplicativeExpContext *ctx) {
  // generate the operands
  auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
  auto rhs = any_cast<Value *>(ctx->exp(1)->accept(this));
  // create convert instruction if needed
  auto lhsTy = lhs->getType();
  auto rhsTy = rhs->getType();
  auto type = getArithmeticResultType(lhsTy, rhsTy);
  if (lhsTy != type)
    lhs = builder.createIToFInst(lhs);
  if (rhsTy != type)
    rhs = builder.createIToFInst(rhs);
  // create the arithmetic instruction
  Value *result = nullptr;
  if (ctx->MUL())
    result = type->isInt() ? builder.createMulInst(lhs, rhs)
                           : builder.createFMulInst(lhs, rhs);
  else if (ctx->DIV())
    result = type->isInt() ? builder.createDivInst(lhs, rhs)
                           : builder.createFDivInst(lhs, rhs);

  else
    result = type->isInt() ? builder.createRemInst(lhs, rhs)
                           : builder.createFRemInst(lhs, rhs);
  return result;
}

any SysYIRGenerator::visitReturnStmt(SysYParser::ReturnStmtContext *ctx) {
  auto value =
      ctx->exp() ? any_cast<Value *>(ctx->exp()->accept(this)) : nullptr;
  Value *result = builder.createReturnInst(value);
  return result;
}

any SysYIRGenerator::visitCall(SysYParser::CallContext *ctx) {
  auto funcName = ctx->ID()->getText();
  auto func = dyncast<Function>(symbols.lookup(funcName));
  assert(func);
  vector<Value *> args;
  if (auto rArgs = ctx->funcRParams()) {
    for (auto exp : rArgs->exp()) {
      args.push_back(any_cast<Value *>(exp->accept(this)));
    }
  }
  Value *call = builder.createCallInst(func, args);
  return call;
}

} // namespace sysy