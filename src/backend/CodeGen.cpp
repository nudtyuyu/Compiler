#include <cstddef>
#include <cstdlib>
#include <memory>
#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include "CodeGen.h"


using std::cerr;
const string space = string(4, ' ');
const string endl = "\n";

namespace backend {
    using RegId = RegManager::RegId;

    string CodeGen::code_gen(){
        string code;
        code += module_gen(module);
        return code;
    }

    string CodeGen::module_gen(Module *module){
        string code;
        string dataCode;
        string textCode;
        //clear last module's label record
        clearModuleRecord(module);
        //generate asmcode for all global values 
        dataCode += globaldata_gen();
        //regm.ShowAVALUE();

        code += space + ".arch armv7 " + endl;
        code += space + ".text " + endl;
        auto functions = module->getFunctions();
        //auto functions = module->functions;
        
        for(auto iter = functions->begin(); iter != functions->end(); ++iter){
            string name = iter->first;
            Function* func = iter->second;
            auto bblist = func->getBasicBlocks();
            if(bblist.empty()) continue;
            //generate asmcode for each function
            //regm.ShowAVALUE();
            textCode += function_gen(func) + endl;
            textCode += space + ".size    "+ name + ", " + ".-" + name + endl;
        }
        code += (dataCode + textCode);
        code += space + ".section        .note.GNU-stack,\"\",%progbits" +endl;
        return code;
    }

    string CodeGen::functionHead_gen(Function *func){
        string code;
        code += space + ".globl " + func->getName() + endl;
        code += space  + ".syntax unified" + endl;
        code += space + ".p2align " + std::to_string(int_p2align) + endl;
        code += space + ".type " + func->getName() + ", %function" + endl;
        code += func->getName() + ":" + endl;
        
        return code;
    }
    /**
     * stack structure:
     * 
     * last function stack
     * --------------------------   <-- fp  point
     * callee preserved regs (include fp, sp, lr)
     * --------------------------
     * tempory variable(ir inst)
     * --------------------------
     * arg0,arg1,arg2,arg3(as tempory variable)
     * --------------------------    
     * dynamic stack(by AllocaInst)  ********
     * -------------------------- 
     * caller preserved regs
     * --------------------------
     * arg4 ~ argX
     * --------------------------    <-- sp point
     * next function stack
     * 
    */
   string CodeGen::prologueCode_gen(Function *func){
        string code;
        auto arguments = func->getEntryBlock()->getArguments();
        static std::set<SymTable *> symTables;
        symTables.clear();
        
        // preserve callee-saved registers (R4-R8, R10, fp, lr)
        code += space + "push   {fp, lr}\n";
        code += space + "add    fp, sp, #4\n";
        spOffset = -4;
        code += space + "push   {r4, r5, r6, r7, r8, r10}\n";
        spOffset += -4 * 6;

        // arguments & local variebles
        for (size_t i = 0; i < arguments.size(); ++i) {
        	auto entry = func->getSymTable()->query(arguments[i]->getName());
            if (i < 4) {
                spOffset -= 4;
                code += space + "push   {" + regm.toString(RegId(i)) + "} @ " + arguments[i]->getName() + "(" + to_string(spOffset) + ")" + endl;
                spOffset -= 4;
                code += space + "mov    r7, sp" + endl;
                code += space + "push   {r7}" + endl;
                
                entry->setOffset(spOffset);
                regm.setOffset(arguments[i], spOffset);
            } else {
                code += space + "add    r4, fp, #" + to_string((i - 3) * 4) + endl;
                spOffset -= 4;
                code += space + "push   {r4}" + endl;
                entry->setOffset(spOffset);
                regm.setOffset(arguments[i], spOffset);
            }
        }
        for (const auto &basicBlock : func->getBasicBlocks()) {
            symTables.insert(basicBlock->getSymTable());
        }
        for (const auto *symTable : symTables) {
            for (auto &mapPair : symTable->getEntries()) {
                auto name = mapPair.first;
                auto *entry = mapPair.second;
                int elemSize, count = 1;

                if (entry->getOffset() != 0) // already allocated
                    continue;

                if (!entry->getDims().empty()) {
                    elemSize = entry->getValue()->getType()->as<PointerType>()->getBaseType()->getSize();
                    if (dynamic_cast<ConstantValue *>(entry->getDims().at(0))->getInt() != 0) {
                        for (auto *dim : entry->getDims()) {
                            count *= dynamic_cast<ConstantValue *>(dim)->getInt();
                        }
                    }
                } else if (entry->isConstant()) {
                    elemSize = entry->getValue()->getType()->getSize();
                } else {
                    elemSize = entry->getValue()->getType()->as<PointerType>()->getBaseType()->getSize();
                }
                spOffset -= count * elemSize;
                code += space + "sub    sp, sp, #" + to_string(count * elemSize) + " @ " + name + "(" + to_string(spOffset) + ")" + endl;
                spOffset -= 4;
                code += space + "mov    r7, sp" + endl;
                code += space + "push   {r7}" + endl;
                entry->setOffset(spOffset);
                regm.setOffset(entry->getValue(), spOffset);
            }
        }


        // // return value
        // if (!func->getReturnType()->isVoid()) {
        //     retValueStOffset = spOffset;
        //     spOffset -= 4;
        //     code += space + "sub    sp, sp, #4\n";
        // }

        return code;
   }

    /* epilogue : 
     *          free stack space
     *          restore sp
     *          restore used callee-saved register(lr, fp and other callee-saved regs)
     *          bx lr
    */
    string CodeGen::epilogueCode_gen(Function *func){
        string code;
        
        // 释放空间(局部变量、函数实参等)
        code += space + "sub    r7, fp, #28" + endl;
        code += space + "mov    sp, r7" + endl;
        // 此处代码待优化：仅恢复使用过的变量寄存器
        code += space + "pop    {r4, r5, r6, r7, r8, r10}\n";
        code += space + "pop    {fp, lr}\n";
        code += space + "bx     lr\n";
        return code;
    }

    string CodeGen::function_gen(Function *func){
        curFunc = func;
        clearFunctionRecord(func);
        string code;
        string funcHead = functionHead_gen(func);
        string prologueCode = prologueCode_gen(func);
        string bbCode;
        auto bbs = func->getBasicBlocks();
        for(auto iter = bbs.begin(); iter != bbs.end(); ++iter){
            auto bb = iter->get();
            bbCode += basicBlock_gen(bb);
        }
        string epilogueCode = epilogueCode_gen(func);
        string literalPoolsCode = literalPoolsCode_gen(func);
        //
        code = funcHead + prologueCode  + bbCode + \
                    epilogueCode + literalPoolsCode;
        return code;
    }

    string CodeGen::basicBlock_gen(BasicBlock *bb){
        curBB = bb;
        string bbLabel = getBBLabel(bb);
        string code;
        code += bbLabel + ":" + endl;
        for(auto &instr : bb->getInstructions()){
            auto instrType = instr->getKind();
            code += instruction_gen(instr.get());
        }
        return code;
    }
    /**
     * RegId : binaryInst_gen returns RegId as its destination operand
     * code  : asmcode generated by binaryInst_gen
    */
    pair<RegId, string> CodeGen::binaryInst_gen(BinaryInst *bInst, RegId dstRegId){
        string code;
        /** 
         *code in here
        */
        switch (bInst->getKind()) {
            case Instruction::kAdd :
            case Instruction::kFAdd :
            code+=space+"add";
            break;
            case Instruction::kSub :
            case Instruction::kFSub :
            code+=space+"sub";
            break;
            case Instruction::kMul :
            case Instruction::kFMul :
            code+=space+"mul";
            break;
            case Instruction::kDiv :
            case Instruction::kFDiv :
            code+=space+"bl	__aeabi_idiv";/*需要在前面声明*/
            break;
            /*大概从这往下都是cmp加上跳转指令*/
            case Instruction::kAnd :
            // code+=space+"and";
            // break;
            case Instruction::kOr :
            // code+=space+"or";
            // break;
            case Instruction::kICmpEQ :
            case Instruction::kFCmpEQ :
            // out << "==";
            // break;
            case Instruction::kICmpNE :
            case Instruction::kFCmpNE :
            // out << "!=";
            // break;
            case Instruction::kICmpLT :
            case Instruction::kFCmpLT :
            // out << "<";
            // break;
            case Instruction::kICmpGT :
            case Instruction::kFCmpGT :
            // out << ">";
            // break;
            case Instruction::kICmpLE :
            case Instruction::kFCmpLE :
            // out << "<=";
            // break;
            case Instruction::kICmpGE :
            case Instruction::kFCmpGE :
            // out << ">=";
            code+=space+"cmp";
            break;

            /*取余回头再说*/
            case Instruction::kRem :
            case Instruction::kFRem : 
            code+=space+"bl	__aeabi_idivmod";
            break;
            default:
            std::cerr << "<error binary instruction type>";
            exit(0);
            break;
        }
        // sysy:Kind kind=bInst->getKind();
        code += "    "+regm.toString(dstRegId);
        code+=" "+bInst->getLhs()->getName()+" "+bInst->getRhs()->getName();
        code+="\n";
        // std::cout<<bInst<<"\n";
        return {dstRegId, code};
    }

    pair<RegId, string> CodeGen::unaryInst_gen(UnaryInst *uInst, RegId dstRegId){
        string code;
        // *********************************************
        switch (uInst->getKind()) {
            case Instruction::kPos :
            case Instruction::kFPos :
            code += space + "mov" + "    "+regm.toString(dstRegId) + " " + uInst->getOperand()->getName() + "\n";
            break;

            // TODO 使用mvn的话,操作数为参数数值减1(编译器使用mvn?)
            // TODO 暂时使用sub
            case Instruction::kNeg :
            case Instruction::kFNeg :
            code += space + "sub" + "    "+regm.toString(dstRegId) + " " + "#0" + " " + uInst->getOperand()->getName() + "\n";
            break;

            case Instruction::kNot :
            code += space + "mvn" + "    "+regm.toString(dstRegId) + " " + uInst->getOperand()->getName() + "\n";
            break;

            // TODO
            case Instruction::kFtoI :
            case Instruction::kIToF :

            default:
            cerr << "<error uinary instruction type>";
            exit(0);
            break;
        }
        // *********************************************
        return {dstRegId, code};
    }

    pair<RegId, string> CodeGen::allocaInst_gen(AllocaInst *aInst, RegManager::RegId dstRegId){
        string code;
        return std::make_pair(dstRegId, code);

        /**
         * Deprecated
         */
        // dstRegId = regm.AssignReg(1)[0];
        // if(!regm.IsEmpty(dstRegId))
        // {
        // 	auto Rvalue = regm.getRVALUE(dstRegId);
        // 	for(int j=0;j<Rvalue.size();j++)
        // 	{
        // 		auto var = Rvalue[j];
        // 		//TODO find the addr in AVALUE(var)
        // 		//auto varp = curBB->getSymTable()->query(var);
        // 		//auto offset = varp->getOffset();
        // 		auto offset = regm.getOffset(var);
        // 		code += space + "str    " + regm.toString(dstRegId) + ", " ", [fp, #" + to_string(offset) +"]"  + endl;
        		
        // 	}
        // 	regm.clearRVALUE(dstRegId);
        // }
        
        // TODO get The Value or do nothing
        // auto varName = aInst->getName();
        
        // auto Sym = curBB->getSymTable()->query(varName);
        // //std::cout<<"Crazy???"<<endl;
        // if(Sym->getDims().size()==0)
        // {
        // 	auto value = Sym->getInitValue();
        // 	if(value==nullptr)
        // 	{
        // 		std::cout<<"initvalue is nullptr!"<<std::endl;
        // 	}
        	
        // 	auto constVal = dynamic_cast<ConstantValue*>(value);
        // 	if(constVal!=nullptr)
        // 	{	
        // 		std::cout<<"it is constantVal"<<std::endl;
        // 		if(constVal->isInt())
        // 		{
        // 			auto digit = constVal->getInt();
        			
        // 			code += space + "movs   " + regm.toString(dstRegId) + ", "+ "#"+ to_string(digit) +endl;
        // 		}
        // 		// TODO: the value of float???
        // 		else if(constVal->isFloat())
        // 		{
        // 			auto number = constVal->getFloat();
        // 			void*nump = &number;
        // 			char bytes[4];
        // 			for(int ii=0;ii<4;ii++)
        // 			{
        // 				bytes[ii] = ((char*)nump)[ii];
        // 			}
        // 			//int *nump = (int*)malloc(sizeof(int));
        // 			//*nump = number;
        // 			char ObjValue[50];
        // 			//int *mynum = bytes;
        // 			void*mynum = bytes;
        // 			sprintf(ObjValue,"%d",*(int*)mynum);
        // 			code += space + "movt   " + regm.toString(dstRegId) + ", "+ ObjValue +endl;
        // 		}
        // 		regm.setReg(value,dstRegId);
        // 	}
        // 	else
        // 	{
        // 		regm.insertRVALUE(dstRegId,value);
        // 		//TODO:Many Problem!!!!
        // 		//auto valueName = value->getName();
        // 		//value = Sym->getValue();
        // 		auto tmp = regm.query(aInst,{});
        // 		auto extraCode = tmp.first;
        // 		auto Regs = tmp.second;
        // 		auto srcReg = Regs[0];
        // 		//TODO Find the Reg which store the valueName
        // 		//auto val = curBB->getSymTable()->query(valueName);
        // 		//auto offset = val->getOffset();
        // 		//code += space + "ldr     " + regm.toString(dstRegId) + ", [fp, #" + to_string(offset) +"]" + endl;
        // 		code += extraCode;
        // 		//code += space + "mov    " + regm.toString(dstReg) + ", "+ regm.toString(dstReg) +endl; 
        		
        		   
        // 	}
        	//Sym->setOffset(fpOffset);
        	//fpOffset -= 4;
        	//Sym->addReg(dstRegId);
        	//std::cout<< space << "symValue*: " << varName<<space<<Sym->getValue() << endl;
        	//regm.setReg(value,dstRegId);
        	//regm.aTable.setReg(Sym->getValue()
        	//auto myreg = regm.aTable.getReg(Sym->getValue());
        	//std::cout<< space << "The reg I set: "<< regm.toString(myreg)<< endl;
        // }
        //TODO Array
        //code += space + "movs    " + regm.toString(dstRegId) + ", "+ "#MyValue"+endl;
        
        return {dstRegId, code};
    }

    string CodeGen::storeInst_gen(StoreInst *stInst){
        string code;
        // auto varName = stInst->getPointer()->getName();
        // auto var = curBB->getSymTable()->query(varName);
        auto *pointer = stInst->getPointer();
        auto *value = stInst->getValue();
        //auto offset = var->getOffset();
        //TODO Find srcRegId from AVALUE!!!
        //std::cout<< space << "var1Value*: " << varName << space << var1 << endl;
        auto tmp = regm.query(nullptr, {pointer, value});
        auto regs = tmp.second;
        code += tmp.first;
        code += space + "str    " + regm.toString(regs[1]) +  ", [" + regm.toString(regs[0]) +"]"  + endl;
        
        return code;
    }
    string CodeGen::loadInst_gen(LoadInst  *ldInst){
        string code;
        
        
        Value *operand = ldInst->getOperand(0);
        //std::cout<<"This is load!"<<endl;
        //std::cout<<"ldInst: "<<ldInst->getName()<<" "<<ldInst<<endl;
        //std::cout<<"operand: "<<operand->getName()<<" "<<operand<<endl;
        auto tmp = regm.query(ldInst, {operand});
        code += tmp.first;
        auto regs = tmp.second;
        auto dstRegId = regs[0];
        auto srcRegId = regs[1];
        code += space + "ldr    " + regm.toString(dstRegId) + ", [" + regm.toString(srcRegId) + "]\n";

        return code;
    }
    string CodeGen::returnInst_gen(ReturnInst *retInst){
        string code;
        if(!retInst->hasReturnValue())
        {
        	code = "";
        	return code;
        }
        // RegManager::RegId dstRegId = regm.AssignReg(1)[0];
        // if(!regm.IsEmpty(dstRegId))
        // {
        // 	auto Rvalue = regm.getRVALUE(dstRegId);
        // 	for(int j=0;j<Rvalue.size();j++)
        // 	{
        // 		auto var = Rvalue[j];
        // 		//TODO find the addr in AVALUE(var)
        // 		auto varp = curBB->getSymTable()->query(var);
        // 		//std::vector<Value*> values;
        // 		//values.push_back(varp->getValue());
        // 		//auto offset = regm.query(values);
        // 		auto offset = regm.getOffset(varp->getValue());
        // 		//auto offset = varp->getOffset();
        // 		code += space + "str    " + regm.toString(dstRegId) +  ", [fp, #" + to_string(offset) +"]"  + endl;
        		
        // 	}
        // 	regm.clearRVALUE(dstRegId);
        // }
        auto returnValue = retInst->getReturnValue();
        auto constVal = dynamic_cast<ConstantValue*>(returnValue);
        auto tmp = regm.query(nullptr, {returnValue});
        code += tmp.first;
        auto dstRegId = tmp.second[0];
        if(constVal!=nullptr)
        {
        	//TODO: float
        	if(constVal->isInt())
        		{
        			auto digit = constVal->getInt();
        			code += space + "movs   " + regm.toString(dstRegId) + ", "+ "#"+ to_string(digit) +endl;
        		}
        }
        else
        {
        	auto valueName = returnValue->getName();
        	// auto val = curBB->getSymTable()->query(valueName);
        	// auto glbMap = module->getGlobalValues();
        	//auto Reg = regm.getReg(returnValue);
            auto addr = regm.getAddress(returnValue);
            auto offset = regm.getOffset(returnValue);
        	if(addr != 0)
        	{
        		//global
        		code += space + "movw   " + regm.toString(dstRegId) + ", "+ "#:lower16:"+ valueName +endl;
        		code += space + "movt   " + regm.toString(dstRegId) + ", "+ "#:upper16:"+ valueName +endl;
        		code += space + "ldr    " + regm.toString(dstRegId) + ", "+ "[" + regm.toString(dstRegId) + "]" +endl;
        	}
        	else
        	{
        		
        		
        			//auto tmp = regm.query(var->getValue());
        			//auto srcRegId = tmp.second;
        			//auto extraCode =
        			// auto srcRegId = regm.getReg(val->getValue()); 
        			// auto offset = regm.getOffset(val->getValue());
        			// if(srcRegId!=RegManager::RNONE)
        			// {
        			// 	//auto srcRegId = val->getReg();
        			// 	code += space + "mov    " + regm.toString(dstRegId) + ", " + regm.toString(RegManager::RegId(srcRegId)) + endl;
        			// }
        			// else
        			// {
        			// 	//auto offset = val->getOffset();
        			//std::cout<<"return: "<<returnValue<<endl;
        			//code += space + "ldr    " + regm.toString(dstRegId) + ", [fp, #" + to_string(offset) +"]" + endl;
        			//code += space + "ldr    " + regm.toString(dstRegId) + ", [" + regm.toString(dstRegId) + "]" + endl;
        			// }
        			
        		//TODO: array[offset]
       		}
       	}
        code += space + "mov    " + regm.toString(RegManager::RegId(0)) + ", " + regm.toString(dstRegId) + endl;
        return code;
    }
    string CodeGen::uncondBrInst_gen(UncondBrInst *ubInst){
        string code;
        string bbname = ubInst->getBlock()->getName();
        code += space + "b      " + bbname + endl;
        return code;
    }
    string CodeGen::condBrInst_gen(CondBrInst *cbInst){
        string code;
        /** 
         *code in here
        */
        return code;
    }
    pair<RegId, string> 
    CodeGen::callInst_gen(CallInst *callInst, RegId dstRegId){
        string code;
        // preserve registers
        code += space + "push   {r0, r1, r2, r3}" + endl;

        // arguments
        size_t n_args = callInst->getNumOperands() - 1;
        // 还有问题，有可能query会分配到r0-r3
        for (size_t i = n_args; i > 0; --i) {
            auto tmp = regm.query(nullptr, {callInst->getOperand(i)});
            code += tmp.first;
            if (i > 4) {
                code += space + "push   {" + regm.toString(tmp.second[0]) + "}" + endl;
            } else {
                code += space + "mov    " + regm.toString(RegId(i - 1)) + ", " + regm.toString(tmp.second[0]) + endl;
            }
        }

        // call
        code += space + "bl     " + callInst->getCallee()->getName() + endl;

        // return value
        if (!callInst->getCallee()->getReturnType()->isVoid()) {
            auto tmp = regm.query(callInst, {});
            code += tmp.first;
            code += space + "mov    " + regm.toString(tmp.second[0]) + ", r0" + endl;
        }

        // restore stack
        if (n_args > 4) {
            code += space + "add    sp, #" + to_string((n_args - 4) * 4) + endl;
        }


        // restore registers
        code += space + "pop    {r0, r1, r2, r3}" + endl;

        return {dstRegId, code};
    }

    string CodeGen::instruction_gen(Instruction *instr)
    {
        string code;
        string unkName = instr->getName();
        RegManager::RegId dstRegId = RegManager::RNONE;
        auto instrType = instr->getKind();
        pair<RegId, string> tmp;
        switch (instrType)
        {
        //binary inst
        case Instruction::kAdd :
        case Instruction::kMul :
        case Instruction::kSub :
        case Instruction::kDiv :
        case Instruction::kRem :
        case Instruction::kAnd :
        case Instruction::kOr  :
        case Instruction::kICmpEQ :
    	case Instruction::kICmpNE :
    	case Instruction::kICmpLT :
    	case Instruction::kICmpGT :
    	case Instruction::kICmpLE :
    	case Instruction::kICmpGE :
    	case Instruction::kFAdd   :
    	case Instruction::kFSub   :
    	case Instruction::kFMul :
    	case Instruction::kFDiv :
   	    case Instruction::kFRem :
    	case Instruction::kFCmpEQ :
    	case Instruction::kFCmpNE :
    	case Instruction::kFCmpLT :
    	case Instruction::kFCmpGT :
        {
            BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
            //registers are used only for instruction operation, consider use which register (any one that is free for use)
            tmp = binaryInst_gen(bInst, RegManager::RANY);
            code += tmp.second;
            dstRegId = tmp.first;
            break;
        }
        //Unary Inst
        case Instruction::kPos :
        case Instruction::kNeg :
    	case Instruction::kNot :
    	case Instruction::kFPos :
    	case Instruction::kFNeg :
    	case Instruction::kFtoI :
    	case Instruction::kIToF :
        {
        	UnaryInst *uInst = dynamic_cast<UnaryInst *>(instr);
        	tmp = unaryInst_gen(uInst,RegManager::RANY);
        	code+= tmp.second;
        	dstRegId = tmp.first;
        	break;
        }
        case Instruction::kLoad :
        {
            LoadInst *ldInst = dynamic_cast<LoadInst *>(instr);
            //code += M_emitComment("load inst");
            code += loadInst_gen(ldInst);
            break;
        }
        case Instruction::kStore :
        {
            StoreInst *stInst = dynamic_cast<StoreInst *>(instr);
            //code += M_emitComment("store inst");
            code += storeInst_gen(stInst);
            return code;
            break;
        }
        case Instruction::kAlloca:
        {
            AllocaInst *aInst = dynamic_cast<AllocaInst *>(instr);
            tmp = allocaInst_gen(aInst, RegManager::RANY);
            //code += M_emitComment("alloca inst");
            code += tmp.second;
            dstRegId = tmp.first;
            break;
        }
        case Instruction::kReturn:
        {
            ReturnInst *retInst = dynamic_cast<ReturnInst *>(instr);
            //code += M_emitComment("return inst");
            code += returnInst_gen(retInst);
            return code;
            break;
        }
        case Instruction::kCall:
        {
            CallInst *cInst = dynamic_cast<CallInst *>(instr);
            auto tmp = callInst_gen(cInst, RegManager::RANY);
            code += tmp.second;
            dstRegId = tmp.first;
            if(dstRegId == RegManager::R0) return code;
            break;
        }
        case Instruction::kBr:
        {
            UncondBrInst *ubInst = dynamic_cast<UncondBrInst *>(instr);
            code += uncondBrInst_gen(ubInst);
            return code;
            break;
        }
        case Instruction::kCondBr:
        {
            CondBrInst *cbInst = dynamic_cast<CondBrInst *>(instr);
            code += condBrInst_gen(cbInst);
            return code;
            break;
        }
        default:
        {
            code += "ERROR CODE : instruction " + \
                unkName + " is not implementation" + endl;
            break;
        }
        }
        if(!instr->getType()->isVoid()){
            // code += storeRegToStack_gen(dstRegId, instr);
            // regm.freeReg(dstRegId);//TODO : code in here.
        }
        return code;
    }
    //
    string CodeGen::globaldata_gen(){
        string asmCode;
        auto globalValues = module->getGlobalValues(); // get the list of the globalValue
        for(auto iter = globalValues->begin();iter!=globalValues->end();iter++)
        {
        	string name = iter->first;
        	GlobalValue* gb = iter->second;
        	//auto sym = builder.getSymTable()->query(name);
        	auto NumDims = gb->getNumDims();
        	auto type = gb->getType();
        	int size = 4;
        	int allSpace = 0;
        	int i;
        	int ElementNum = 1;
        	for(i=0;i<NumDims;i++)
        	{
        		auto d = dynamic_cast<ConstantValue *>(gb->getDim(i));
        		ElementNum*=d->getInt();
        	}
        	allSpace= ElementNum*size;
        	/*if(i==0)
        	{
        		allSpace = size;
        	}*/
        	asmCode += space + ".global  " + name + endl;
        	if(gb->isConstant())
        	{
        		asmCode += space + ".section " + space+".rodata" + endl;
        	}
        	else if(gb->getInitVal()==nullptr)
        	{
        		asmCode += space + ".bss" + endl;
        	}
        	else
        	{
        		asmCode += space + ".data" + endl;
        	}
        	asmCode+= space + ".align   " + "2" +endl;
        	asmCode+= space + ".type    " + name + ","+ " %object"+endl;
        	asmCode+= space + ".size    " + name + ", " + to_string(allSpace) + endl;
        	asmCode+= name + ":" +endl;
        	auto initV = gb->getInitVal();
        	//auto numDims = gb->getNumDims();
        	if(NumDims==0 && initV!=nullptr)
        	{
        		// not array (has init)
        		auto initv = dynamic_cast<ConstantValue *>(initV);
        		if(initv!=nullptr)
        		{
        			if(initv->isInt())
        			{
        				int number = initv->getInt();
        				asmCode+= space + ".word   " + to_string(number) +endl;
        			}
        			// TODO float???
        			else if(initv->isFloat())
        			{
        				float number = initv->getFloat();
        				//char *nump =(char*)malloc(sizeof(char));
        				// void*nump = &number;
        				// char bytes[4];
        				// for(int ii=0;ii<4;ii++)
        				// {
        				// 	bytes[ii] = ((char*)nump)[ii];
        				// }
        				//int *nump = (int*)malloc(sizeof(int));
        				//*nump = number;
        				//int *mynum = bytes;
        				// void*mynum = bytes;
        				asmCode+= space + ".word   " + to_string(*(int*)&number) +endl;
        			
        			}
        		}
        		//TODO: Move to IR to exam
        		else
        		{
        			std::cout<<"error: initializer element is not constant"<<endl;
        			exit(0);
        		}
        	}
        	else if(initV==nullptr)
        	{
        		asmCode+= space + ".space   " + to_string(allSpace) +endl;
        	}	
        	else
        	{
        		auto initv = dynamic_cast<InitList *>(initV);
        		int numZero;
        		for(int j=0;j<ElementNum;j++)
        		{
        			auto element = initv->getElement(j);
        			numZero = 0;
        			while(element->name=="0")
        			{
        				numZero++;
        				j++;
        				if(j>=ElementNum)
        					break;
        				element = initv->getElement(j);
        			}
        			if(numZero>0)
        			{
        				int zero = numZero*4;
        				asmCode+= space + ".space   " + to_string(zero) + endl;
        				if(j<ElementNum)
        					asmCode+= space + ".word    " + element->name +endl;
        			}
        			else
        				asmCode+= space + ".word    " + element->name +endl;
        		}
        	}
        	// 临时标记一下全局变量
            regm.setAddress(gb, 0x10000);
            //std::cout<<name<<": "<<", "<<gb<<regm.getAddress(gb)<<endl;
        }
        return asmCode;
    }

    string CodeGen::literalPoolsCode_gen(Function *func){
        string code;
        /** 
         *code in here
        */
        return code;
    }
    
    string CodeGen::getBBLabel(BasicBlock *bb){
        auto t = bb_labels.find(bb);
        string label;
        if(t == bb_labels.end()){
            label = ".LBB_" + to_string(label_no++);
            bb_labels.emplace(bb, label);
        }else{
            label = t->second;
        }
        return label;
    }

    
    void CodeGen::clearFunctionRecord(Function *func){
        // localVarStOffset.clear();
        // paramsStOffset.clear();
        // retValueStOffset = 0;
        spOffset = 0;
        bb_labels.clear();
        regm.clear();
        
        for (auto reg : RegManager::UserRegs) {
            regm.clearRVALUE(reg);
        }
        //
        stOffsetAcc = 0;
    }
}//namespace backend
