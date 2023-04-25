#include "IR.h"
#include "range.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace std;

namespace sysy {

template <typename T>
ostream &interleave(ostream &os, const T &container, const string sep = ", ") {
  auto b = container.begin(), e = container.end();
  if (b == e)
    return os;
  os << *b;
  for (b = next(b); b != e; b = next(b))
    os << sep << *b;
  return os;
}
static inline ostream &printVarName(ostream &os, const Value *var) {
  return os << (dyncast<GlobalValue>(var) ? '@' : '%')
            << var->getName();
}
static inline ostream &printBlockName(ostream &os, const BasicBlock *block) {
  return os << '^' << block->getName();
}
static inline ostream &printFunctionName(ostream &os, const Function *fn) {
  return os << '@' << fn->getName();
}
static inline ostream &printOperand(ostream &os, const Value *value) {
  auto constant = dyncast<ConstantValue>(value);
  if (constant) {
    constant->print(os);
    return os;
  }
  return printVarName(os, value);
}
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
                            const vector<Type *> &paramTypes) {
  // forward to FunctionType
  return FunctionType::get(returnType, paramTypes);
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
  }
  return 0;
}

void Type::print(ostream &os) const {
  auto kind = getKind();
  switch (kind) {
  case kInt:
    os << "int";
    break;
  case kFloat:
    os << "float";
    break;
  case kVoid:
    os << "void";
    break;
  case kPointer:
    static_cast<const PointerType *>(this)->getBaseType()->print(os);
    os << "*";
    break;
  case kFunction:
    static_cast<const FunctionType *>(this)->getReturnType()->print(os);
    os << "(";
    interleave(os, static_cast<const FunctionType *>(this)->getParamTypes());
    os << ")";
    break;
  case kLabel:
  default:
    cerr << "Unexpected type!\n";
    break;
  }
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

void Value::replaceAllUsesWith(Value *value) {
  for (auto &use : uses)
    use->getUser()->setOperand(use->getIndex(), value);
  uses.clear();
}

bool Value::isConstant() const {
  if (dyncast<ConstantValue>(this))
    return true;
  if (dyncast<GlobalValue>(this) or
      dyncast<Function>(this))
    return true;
  // if (auto array = dyncast<const ArrayValue *>(this)) {
  //   auto elements = array->getValues();
  //   return all_of(elements.begin(), elements.end(),
  //                 [](Value *v) -> bool { return v->isConstant(); });
  // }
  return false;
}

ConstantValue *ConstantValue::get(int value) {
  static std::map<int, std::unique_ptr<ConstantValue>> intConstants;
  auto iter = intConstants.find(value);
  if (iter != intConstants.end())
    return iter->second.get();
  auto constant = new ConstantValue(value);
  assert(constant);
  auto result = intConstants.emplace(value, constant);
  return result.first->second.get();
}

ConstantValue *ConstantValue::get(float value) {
  static std::map<float, std::unique_ptr<ConstantValue>> floatConstants;
  auto iter = floatConstants.find(value);
  if (iter != floatConstants.end())
    return iter->second.get();
  auto constant = new ConstantValue(value);
  assert(constant);
  auto result = floatConstants.emplace(value, constant);
  return result.first->second.get();
}

void ConstantValue::print(ostream &os) const {
  if (isInt())
    os << getInt();
  else
    os << getFloat();
}

Argument::Argument(Type *type, BasicBlock *block, int index,
                   const std::string &name)
    : Value(kArgument, type, name), block(block), index(index) {
  if (not hasName())
    setName(to_string(block->getParent()->allocateVariableID()));
}

void Argument::print(std::ostream &os) const {
  assert(hasName());
  printVarName(os, this) << ": " << *getType();
}

BasicBlock::BasicBlock(Function *parent, const std::string &name)
    : Value(kBasicBlock, Type::getLabelType(), name), parent(parent),
      instructions(), arguments(), successors(), predecessors() {
  if (not hasName())
    setName("bb" + to_string(getParent()->allocateblockID()));
}

void BasicBlock::print(std::ostream &os) const {
  assert(hasName());
  os << "  ";
  printBlockName(os, this);
  auto args = getArguments();
  auto b = args.begin(), e = args.end();
  if (b != e) {
    os << '(';
    printVarName(os, b->get()) << ": " << *b->get()->getType();
    for (auto &arg : make_range(std::next(b), e)) {
      os << ", ";
      printVarName(os, arg.get()) << ": " << *arg->getType();
    }
    os << ')';
  }
  os << ":\n";
  for (auto &inst : instructions) {
    os << "    " << *inst << '\n';
  }
}

Instruction::Instruction(Kind kind, Type *type, BasicBlock *parent,
                         const std::string &name)
    : User(kind, type, name), kind(kind), parent(parent) {
  if (not type->isVoid() and not hasName())
    setName(to_string(getFunction()->allocateVariableID()));
}

void CallInst::print(std::ostream &os) const {
  if (not getType()->isVoid())
    printVarName(os, this) << " = call ";
  printFunctionName(os, getCallee()) << '(';
  auto args = getArguments();
  auto b = args.begin(), e = args.end();
  if (b != e) {
    printOperand(os, *b);
    for (auto arg : make_range(std::next(b), e)) {
      os << ", ";
      printOperand(os, arg);
    }
  }
  os << ") : " << *getType();
}

void UnaryInst::print(std::ostream &os) const {
  printVarName(os, this) << " = ";
  switch (getKind()) {
  case kNeg:
    os << "neg";
    break;
  case kNot:
    os << "not";
    break;
  case kFNeg:
    os << "fneg";
    break;
  case kFtoI:
    os << "ftoi";
    break;
  case kIToF:
    os << "itof";
    break;
  default:
    assert(false);
  }
  printOperand(os, getOperand()) << " : " << *getType();
}

void BinaryInst::print(std::ostream &os) const {
  printVarName(os, this) << " = ";
  switch (getKind()) {
  case kAdd:
    os << "add";
    break;
  case kSub:
    os << "sub";
    break;
  case kMul:
    os << "mul";
    break;
  case kDiv:
    os << "div";
    break;
  case kRem:
    os << "rem";
    break;
  case kICmpEQ:
    os << "icmpeq";
    break;
  case kICmpNE:
    os << "icmpne";
    break;
  case kICmpLT:
    os << "icmplt";
    break;
  case kICmpGT:
    os << "icmpgt";
    break;
  case kICmpLE:
    os << "icmple";
    break;
  case kICmpGE:
    os << "icmpge";
    break;
  case kFAdd:
    os << "fadd";
    break;
  case kFSub:
    os << "fsub";
    break;
  case kFMul:
    os << "fmul";
    break;
  case kFDiv:
    os << "fdiv";
    break;
  case kFRem:
    os << "frem";
    break;
  case kFCmpEQ:
    os << "fcmpeq";
    break;
  case kFCmpNE:
    os << "fcmpne";
    break;
  case kFCmpLT:
    os << "fcmplt";
    break;
  case kFCmpGT:
    os << "fcmpgt";
    break;
  case kFCmpLE:
    os << "fcmple";
    break;
  case kFCmpGE:
    os << "fcmpge";
    break;
  default:
    assert(false);
  }
  os << ' ';
  printOperand(os, getLhs()) << ", ";
  printOperand(os, getRhs()) << " : " << *getType();
}

void ReturnInst::print(std::ostream &os) const {
  os << "return";
  if (auto value = getReturnValue()) {
    os << ' ';
    printOperand(os, value) << " : " << *value->getType();
  }
}

void UncondBrInst::print(std::ostream &os) const {
  os << "br ";
  printBlockName(os, getBlock());
  auto args = getArguments();
  auto b = args.begin(), e = args.end();
  if (b != e) {
    os << '(';
    printOperand(os, *b);
    for (auto arg : make_range(std::next(b), e)) {
      os << ", ";
      printOperand(os, arg);
    }
    os << ')';
  }
}

void CondBrInst::print(std::ostream &os) const {
  os << "condbr ";
  printOperand(os, getCondition()) << ", ";
  printBlockName(os, getThenBlock());
  {
    auto args = getThenArguments();
    auto b = args.begin(), e = args.end();
    if (b != e) {
      os << '(';
      printOperand(os, *b);
      for (auto arg : make_range(std::next(b), e)) {
        os << ", ";
        printOperand(os, arg);
      }
      os << ')';
    }
  }
  os << ", ";
  printBlockName(os, getElseBlock());
  {
    auto args = getElseArguments();
    auto b = args.begin(), e = args.end();
    if (b != e) {
      os << '(';
      printOperand(os, *b);
      for (auto arg : make_range(std::next(b), e)) {
        os << ", ";
        printOperand(os, arg);
      }
      os << ')';
    }
  }
}

void AllocaInst::print(std::ostream &os) const {
  if (getNumDims())
    cerr << "not implemented yet\n";
  printVarName(os, this) << " = ";
  os << "alloca "
     << *static_cast<const PointerType *>(getType())->getBaseType();
  os << " : " << *getType();
}

void LoadInst::print(std::ostream &os) const {
  if (getNumIndices())
    cerr << "not implemented yet\n";
  printVarName(os, this) << " = ";
  os << "load ";
  printOperand(os, getPointer()) << " : " << *getType();
}

void StoreInst::print(std::ostream &os) const {
  if (getNumIndices())
    cerr << "not implemented yet\n";
  os << "store ";
  printOperand(os, getValue()) << ", ";
  printOperand(os, getPointer()) << " : " << *getValue()->getType();
}

void Function::print(std::ostream &os) const {
  auto returnType = getReturnType();
  auto paramTypes = getParamTypes();
  os << *returnType << ' ';
  printFunctionName(os, this) << '(';
  auto b = paramTypes.begin(), e = paramTypes.end();
  if (b != e) {
    os << *(*b);
    for (auto type : make_range(std::next(b), e))
      os << ", " << *type;
  }
  os << ") {\n";
  for (auto &bb : getBasicBlocks()) {
    os << *bb << '\n';
  }
  os << "}";
}

void Module::print(std::ostream &os) const {
  for (auto &value : children)
    os << *value << '\n';
}

// ArrayValue *ArrayValue::get(Type *type, const vector<Value *> &values) {
//   static map<pair<Type *, size_t>, unique_ptr<ArrayValue>> arrayConstants;
//   hash<string> hasher;
//   auto key = make_pair(
//       type, hasher(string(reinterpret_cast<const char *>(values.data()),
//                           values.size() * sizeof(Value *))));

//   auto iter = arrayConstants.find(key);
//   if (iter != arrayConstants.end())
//     return iter->second.get();
//   auto constant = new ArrayValue(type, values);
//   assert(constant);
//   auto result = arrayConstants.emplace(key, constant);
//   return result.first->second.get();
// }

// ArrayValue *ArrayValue::get(const std::vector<int> &values) {
//   vector<Value *> vals(values.size(), nullptr);
//   std::transform(values.begin(), values.end(), vals.begin(),
//                  [](int v) { return ConstantValue::get(v); });
//   return get(Type::getIntType(), vals);
// }

// ArrayValue *ArrayValue::get(const std::vector<float> &values) {
//   vector<Value *> vals(values.size(), nullptr);
//   std::transform(values.begin(), values.end(), vals.begin(),
//                  [](float v) { return ConstantValue::get(v); });
//   return get(Type::getFloatType(), vals);
// }

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

CallInst::CallInst(Function *callee, const std::vector<Value *> &args,
                   BasicBlock *parent, const std::string &name)
    : Instruction(kCall, callee->getReturnType(), parent, name) {
  addOperand(callee);
  for (auto arg : args)
    addOperand(arg);
}

Function *CallInst::getCallee() const {
  return dyncast<Function>(getOperand(0));
}

} // namespace sysy