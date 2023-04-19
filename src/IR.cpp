#include "IR.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace sysy {

//===----------------------------------------------------------------------===//
// Types
//===----------------------------------------------------------------------===//

Type *Type::getIntType() {
  static Type intType(kInt);
  return &intType;
}

Type *Type::getFloatType() {
  static Type floatType(kFloat);
  return &floatType;
}

Type *Type::getVoidType() {
  static Type voidType(kVoid);
  return &voidType;
}

Type *Type::getLabelType() {
  static Type labelType(kLabel);
  return &labelType;
}

Type *Type::getPointerType(Type *baseType) {
  // forward to PointerType
  return PointerType::get(baseType);
}

Type *Type::getFunctionType(Type *returnType,
                            const std::vector<Type *> &paramTypes) {
  // forward to FunctionType
  return FunctionType::get(returnType, paramTypes);
}

Type *Type::getInitListType() {
  static Type initListType(kInitList);
  return &initListType;
}

int Type::getSize() const {
  switch (kind) {
  case kInt:
  case kFloat:
    return 4;
  case kLabel:
  case kPointer:
  case kFunction:
    return 8;
  case kVoid:
    return 0;
  case kInitList:
    return -1;
  }
  return 0;
}

PointerType *PointerType::get(Type *baseType) {
  static std::map<Type *, std::unique_ptr<PointerType>> pointerTypes;
  auto iter = pointerTypes.find(baseType);
  if (iter != pointerTypes.end())
    return iter->second.get();
  auto type = new PointerType(baseType);
  assert(type);
  auto result = pointerTypes.emplace(baseType, type);
  return result.first->second.get();
}

FunctionType *FunctionType::get(Type *returnType,
                                const std::vector<Type *> &paramTypes) {
  static std::set<std::unique_ptr<FunctionType>> functionTypes;
  auto iter =
      std::find_if(functionTypes.begin(), functionTypes.end(),
                   [&](const std::unique_ptr<FunctionType> &type) -> bool {
                     if (returnType != type->getReturnType() or
                         paramTypes.size() != type->getParamTypes().size())
                       return false;
                     return std::equal(paramTypes.begin(), paramTypes.end(),
                                       type->getParamTypes().begin());
                   });
  if (iter != functionTypes.end())
    return iter->get();
  auto type = new FunctionType(returnType, paramTypes);
  assert(type);
  auto result = functionTypes.emplace(type);
  return result.first->get();
}

void Function::generateCode(std::ostream &out) const {
  for (const auto &block : blocks) {
    block->generateCode(out);
  }
}

void Value::replaceAllUsesWith(Value *value) {
  for (auto &use : uses)
    use->getUser()->setOperand(use->getIndex(), value);
  uses.clear();
}

ConstantValue *ConstantValue::get(int value, const std::string &name) {
  static std::map<int, std::unique_ptr<ConstantValue>> intConstants;
  auto iter = intConstants.find(value);
  if (iter != intConstants.end())
    return iter->second.get();
  auto inst = new ConstantValue(value,name);
  assert(inst);
  auto result = intConstants.emplace(value, inst);
  return result.first->second.get();
}

ConstantValue *ConstantValue::get(float value, const std::string &name) {
  static std::map<float, std::unique_ptr<ConstantValue>> floatConstants;
  auto iter = floatConstants.find(value);
  if (iter != floatConstants.end())
    return iter->second.get();
  auto inst = new ConstantValue(value,name);
  assert(inst);
  auto result = floatConstants.emplace(value, inst);
  return result.first->second.get();
}

void User::setOperand(int index, Value *value) {
  assert(index < getNumOperands());
  operands[index].setValue(value);
}

void User::replaceOperand(int index, Value *value) {
  assert(index < getNumOperands());
  auto &use = operands[index];
  use.getValue()->removeUse(&use);
  use.setValue(value);
}

Value * InitList::getElement(const std::vector<int> &indices) {
  auto *pList = this;
  for (auto index : indices) {
    if (index >= pList->getNumOperands())
      return nullptr;
    auto *tmp = pList->getOperand(index);
    if (tmp->getType() == Type::getInitListType()) {
      pList = dynamic_cast<InitList *>(tmp);
    } else {
      return tmp;
    }
  }

  return nullptr;
}

CallInst::CallInst(Function *callee, const std::vector<Value *> args,
                          BasicBlock *parent, const std::string &name)
    : Instruction(kCall, callee->getReturnType(), parent, name) {
  addOperand(callee);
  addOperands(args);
}

Function *CallInst::getCallee() const {
  return dynamic_cast<Function *>(getOperand(0));
}

void CallInst::generateCode(std::ostream &out) const {
    auto *func = getCallee();
    auto args = getArguments();
    out << "    " << getName() << " = call " << func->getName() << "(";
    for (auto it = args.begin(); it != args.end(); ++it)
      out << (it != args.begin() ? ", " : "") << it->getValue()->getName();
    out << ")\n";
  }

void UnaryInst::generateCode(std::ostream &out) const {
  out << "    " + getName() << " = ";
  switch (kind) {
    case Instruction::kPos :
    case Instruction::kFPos :
      out << "+";
      break;
    case Instruction::kNeg :
    case Instruction::kFNeg :
      out << "-";
      break;
    case Instruction::kNot :
      out << "not ";
      break;
    case Instruction::kFtoI :
      out << "ftoi ";
      break;
    case Instruction::kIToF :
      out << "itof ";
      break;
    default:
      out << "<error unary instruction type>";
  }
  out << getOperand()->getName() << "\n";
}

void BinaryInst::generateCode(std::ostream &out) const {
  out << "    " + getName() << " = " << getLhs()->getName() << " ";
  switch (kind) {
    case Instruction::kAdd :
    case Instruction::kFAdd :
      out << "+";
      break;
    case Instruction::kSub :
    case Instruction::kFSub :
      out << "-";
      break;
    case Instruction::kMul :
    case Instruction::kFMul :
      out << "*";
      break;
    case Instruction::kDiv :
    case Instruction::kFDiv :
      out << "/";
      break;
    case Instruction::kAnd :
      out << "&&";
      break;
    case Instruction::kOr :
      out << "||";
      break;
    case Instruction::kICmpEQ :
    case Instruction::kFCmpEQ :
      out << "==";
      break;
    case Instruction::kICmpNE :
    case Instruction::kFCmpNE :
      out << "!=";
      break;
    case Instruction::kICmpLT :
    case Instruction::kFCmpLT :
      out << "<";
      break;
    case Instruction::kICmpGT :
    case Instruction::kFCmpGT :
      out << ">";
      break;
    case Instruction::kICmpLE :
    case Instruction::kFCmpLE :
      out << "<=";
      break;
    case Instruction::kICmpGE :
    case Instruction::kFCmpGE :
      out << ">=";
      break;
    case Instruction::kRem :
    case Instruction::kFRem :
      out << "%";
      break;
    default:
      out << "<error binary instruction type>";
      break;
  }
  out << " " << getRhs()->getName() << "\n";
}

void ReturnInst::generateCode(std::ostream &out) const  {
  auto *retVal = this->getReturnValue();
  if (retVal != nullptr) {
    out << "    return " << retVal->getName() << "\n";
  } else {
    out << "    return\n";
  }
}

void UncondBrInst::generateCode(std::ostream &out) const {
  auto *target = getOperand(0);
  auto args = getArguments();
  out << "    bruc " << target->name << "(";
  for (auto it = args.begin(); it != args.end(); ++it) {
    out << (it != args.begin() ? ", " : "") << it->getValue()->name;
  }
  out << ")\n";
}

void CondBrInst::generateCode(std::ostream &out) const {
  auto *condition = getCondition();
  auto *thenBlock = getThenBlock();
  auto *elseBlock = getElseBlock();
  auto thenArgs = getThenArguments();
  auto elseArgs = getElseArguments();
  out << "    brc " << getCondition()->name;
  out << ", " << thenBlock->name << "(";
  for (auto it = thenArgs.begin(); it != thenArgs.end(); ++it) {
    out << (it != thenArgs.begin() ? ", " : "") << it->getValue()->name;
  }
  out << ")";
  out << ", " << elseBlock->name << "(";
  for (auto it = elseArgs.begin(); it != elseArgs.end(); ++it) {
    out << (it != elseArgs.begin() ? ", " : "") << it->getValue()->name;
  }
  out << ")\n";
}

void AllocaInst::generateCode(std::ostream &out) const {
  out << "    " + getName() << " = " << "alloca ";
  if (getType()->isFloat())
    out << "float";
  else if (getType()->isInt()) {
    out << "int";
  }
  if (getNumDims() > 0) {
    out << "[";
    auto dims = getDims();
    for (auto it = dims.begin(); it != dims.end(); ++it) {
      out << (it != dims.begin() ? ", " : "") << it->getValue()->getName();
    }
    out << "]";
  }
  out << "\n";
}

void LoadInst::generateCode(std::ostream &out) const {
  auto *ptr = getPointer();
  out << "    " << getName() << " = load " << ptr->getName() << "\n";
}

void StoreInst::generateCode(std::ostream &out) const {
  auto *ptr = getPointer();
  auto *val = getValue();
  auto indices = getIndices();
  out << "    store " << ptr->getName() << ", " << val->getName();
  for (auto index : indices) {
    out << "[" << index.getValue()->getName() << "]";
  }
  out << "\n";
}

void BasicBlock::generateCode(std::ostream &out) const {
  out << getName() << "(";
  for (auto it = arguments.begin(); it != arguments.end(); ++it) {
    out << (it != arguments.begin() ? ", " : "");
    out << (*it)->getName();
  }
  out << "):\n";
  for (auto it = instructions.begin(); it != instructions.end(); ++it) {
    (*it)->generateCode(out);
  }
}

void Module::generateCode(std::ostream &out) const {
  for (auto &func : functions) {
    func.second->generateCode(out);
  }
}

} // namespace sysy
