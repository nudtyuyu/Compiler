#include "codegen.hpp"
#include <string>
#include<iostream>
namespace backend{
    using RegId = RegManager::RegId;
    #define out std::cout
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
    /**
     * prologue :
     *          preserve callee-saved register (lr, fp and other callee-saved regs)
     *          set new fp
     *          alloc stack space for local var/ args / return value.
     *          store args to stack
     * */
   string CodeGen::prologueCode_gen(Function *func){
        string code;
        int spOffset = 0;
        fpOffset = -8;
        
        // preserve callee-saved registers (R4-R8, R10, fp, lr)
        code += space + "push   {fp, lr}\n";
        code += space + "add    fp, sp, #4\n";
        spOffset = -4;
        // 此处代码待优化：仅保存使用过的变量寄存器
        code += space + "push   {r4, r5, r6, r7, r8, r10}\n";
        spOffset += -4 * 6;
        // 此处代码待补充：为局部变量分配空间
        // code += space + "sub    sp, sp, #x";
        // arguments & return value (4 bytes)
        auto arguments = func->getEntryBlock()->getArguments();
        code += space + "sub    sp, sp, #" + to_string(std::min(4ul, arguments.size()) * 4) + "\n";
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i < 4) {
                paramsStOffset[arguments[i]] = spOffset;
                spOffset -= 4;
                code += space + "str    " + regm.toString(RegId(i)) + ", [fp, #" + to_string(spOffset) + "]\n";
            } else {
                paramsStOffset[arguments[i]] = (i - 3) * 4;
            }
        }
        if (!func->getReturnType()->isVoid()) {
            retValueStOffset = spOffset;
            spOffset -= 4;
            code += space + "sub    sp, sp, #4\n";
        }

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
        RegManager::RegId dstRegId = regm.AssignReg();
        code += space + "sub    " + regm.toString(dstRegId) + ", fp, #28" + endl;
        code += space + "mov    sp, " + regm.toString(dstRegId) + endl;
        // 此处代码待优化：仅恢复使用过的变量寄存器
        code += space + "pop    {r4, r5, r6, r7, r8, r10}\n";
        code += space + "pop    {fp, lr}\n";
        code += space + "bx     lr\n";
        return code;
    }

    string CodeGen::function_gen(Function *func){
    	//fpOffset = -8;
        curFunc = func;
        clearFunctionRecord(func);
        string bbCode;
        auto bbs = func->getBasicBlocks();
        for(auto iter = bbs.begin(); iter != bbs.end(); ++iter){
            auto bb = iter->get();
            bbCode += basicBlock_gen(bb);
        }
        string code;
        string funcHead = functionHead_gen(func);
        string prologueCode = prologueCode_gen(func);
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
            out << "<error binary instruction type>";
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
            out << "<error uinary instruction type>";
            exit(0);
            break;
        }
        // *********************************************
        return {dstRegId, code};
    }

    pair<RegId, string> 
    CodeGen::allocaInst_gen(AllocaInst *aInst, RegManager::RegId dstRegId){
        string code;
        /** 
         *code in here
        */
        dstRegId = regm.AssignReg();
        if(!regm.IsEmpty(dstRegId))
        {
        	auto Rvalue = regm.getRVALUE(dstRegId);
        	for(int j=0;j<Rvalue.size();j++)
        	{
        		auto var = Rvalue[j];
        		//TODO find the addr in AVALUE(var)
        		auto varp = curBB->getSymTable()->query(var);
        		auto offset = varp->getOffset();
        		code += space + "str    " + regm.toString(dstRegId) + ", " ", [fp, #" + to_string(offset) +"]"  + endl;
        		
        	}
        	regm.clearRVALUE(dstRegId);
        }
        
        // TODO get The Value or do nothing
        auto varName = aInst->getName();
        regm.insertRVALUE(dstRegId,varName);
        auto Sym = curBB->getSymTable()->query(varName);
        if(Sym->getDims().size()==0)
        {
        	auto value = Sym->getInitValue();
        	auto constVal = dynamic_cast<ConstantValue*>(value);
        	if(constVal!=nullptr)
        	{
        		if(constVal->isInt())
        		{
        			auto digit = constVal->getInt();
        			code += space + "movs   " + regm.toString(dstRegId) + ", "+ "#"+ to_string(digit) +endl;
        		}
        		// TODO: the value of float???
        		else if(constVal->isFloat())
        		{
        			auto number = constVal->getFloat();
        			void*nump = &number;
        			char bytes[4];
        			for(int ii=0;ii<4;ii++)
        			{
        				bytes[ii] = ((char*)nump)[ii];
        			}
        			//int *nump = (int*)malloc(sizeof(int));
        			//*nump = number;
        			char ObjValue[50];
        			//int *mynum = bytes;
        			void*mynum = bytes;
        			sprintf(ObjValue,"%d",*(int*)mynum);
        			code += space + "movt   " + regm.toString(dstRegId) + ", "+ ObjValue +endl;
        		}
        	}
        	else
        	{
        		auto valueName = value->getName();
        		//TODO Find the Reg which store the valueName
        		//auto val = curBB->getSymTable()->query(valueName);
        		//auto offset = val->getOffset();
        		//code += space + "ldr     " + regm.toString(dstRegId) + ", [fp, #" + to_string(offset) +"]" + endl;
        		code += space + "mov    " + regm.toString(dstRegId) + ", "+ regm.toString(RegManager::RANY) +endl; 
        		   
        	}
        	Sym->setOffset(fpOffset);
        	fpOffset -= 4;
        	Sym->addReg(dstRegId);
        }
        //TODO Array
        //code += space + "movs    " + regm.toString(dstRegId) + ", "+ "#MyValue"+endl;
        
        
        return {dstRegId, code};
    }

    string CodeGen::storeInst_gen(StoreInst *stInst){
        string code;
        /** 
         *code in here
        */
        auto varName = stInst->getName();
        auto var = curBB->getSymTable()->query(varName);
        auto offset = var->getOffset();
        //TODO Find srcRegId from AVALUE!!!
        auto srcRegId = var->getReg();
        code += space + "str    " + regm.toString(RegManager::RegId(srcRegId)) +  ", [fp, #" + to_string(offset) +"]"  + endl;
        
        return code;
    }
    pair<RegId, string> 
    CodeGen::loadInst_gen(LoadInst  *ldInst, RegId dstRegId){
        string code;
        /** 
         *code in here
        */
        return {dstRegId, code};
    }
    string CodeGen::returnInst_gen(ReturnInst *retInst){
        string code;
        /** 
         *code in here
        */
        if(!retInst->hasReturnValue())
        {
        	code = "";
        	return code;
        }
        RegManager::RegId dstRegId = regm.AssignReg();
        if(!regm.IsEmpty(dstRegId))
        {
        	auto Rvalue = regm.getRVALUE(dstRegId);
        	for(int j=0;j<Rvalue.size();j++)
        	{
        		auto var = Rvalue[j];
        		//TODO find the addr in AVALUE(var)
        		auto varp = curBB->getSymTable()->query(var);
        		auto offset = varp->getOffset();
        		code += space + "str    " + regm.toString(dstRegId) +  ", [fp, #" + to_string(offset) +"]"  + endl;
        		
        	}
        	regm.clearRVALUE(dstRegId);
        }
        auto ReturnVal = retInst->getReturnValue();
        auto constVal = dynamic_cast<ConstantValue*>(ReturnVal);
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
        	auto valueName = ReturnVal->getName();
        	auto val = curBB->getSymTable()->query(valueName);
        	auto glbMap = module->getGlobalValues();
        	if(glbMap->find(valueName)!=glbMap->end())
        	{
        		code += space + "movw   " + regm.toString(dstRegId) + ", "+ "#:lower16:"+ valueName +endl;
        		code += space + "movt   " + regm.toString(dstRegId) + ", "+ "#:upper16:"+ valueName +endl;
        		code += space + "ldr    " + regm.toString(dstRegId) + ", "+ "[" + regm.toString(dstRegId) + "]" +endl;
        	}
        	else
        	{
        		auto dims = val->getDims();
        		if(dims.size()==0)
        		{
        			if(!val->isNotInReg())
        			{
        				auto srcRegId = val->getReg();
        				code += space + "mov    " + regm.toString(dstRegId) + ", " + regm.toString(RegManager::RegId(srcRegId)) + endl;
        			}
        			else
        			{
        				auto offset = val->getOffset();
        				code += space + "ldr    " + regm.toString(dstRegId) + ", [fp, #" + to_string(offset) +"]" + endl;
        			}
        		}
        		//TODO: array[offset]
       		}
       	}
        code += space + "mov    " + regm.toString(RegManager::RegId(0)) + ", " + regm.toString(dstRegId) + endl;
        return code;
    }
    string CodeGen::uncondBrInst_gen(UncondBrInst *ubInst){
        string code;
        /** 
         *code in here
        */
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
        /** 
         *code in here
        */
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
            tmp = loadInst_gen(ldInst, RegManager::RANY);
            //code += M_emitComment("load inst");
            code += tmp.second;
            dstRegId = tmp.first;
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
            code += storeRegToStack_gen(dstRegId, instr);
            // regm.freeReg(dstRegId);//TODO : code in here.
        }
        return code;
    }
    //
    string CodeGen::globaldata_gen(){
        string asmCode;
        /** 
         *code in here
        */
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
        	char ObjSpace[50];
        	sprintf(ObjSpace,"%d",allSpace);
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
        	asmCode+= space + ".size    " + name + ", " + ObjSpace + endl;
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
        				char ObjValue[50];
        				sprintf(ObjValue,"%d",number);
        				asmCode+= space + ".word   " + ObjValue +endl;
        			}
        			// TODO float???
        			else if(initv->isFloat())
        			{
        				float number = initv->getFloat();
        				//char *nump =(char*)malloc(sizeof(char));
        				void*nump = &number;
        				char bytes[4];
        				for(int ii=0;ii<4;ii++)
        				{
        					bytes[ii] = ((char*)nump)[ii];
        				}
        				//int *nump = (int*)malloc(sizeof(int));
        				//*nump = number;
        				char ObjValue[50];
        				//int *mynum = bytes;
        				void*mynum = bytes;
        				sprintf(ObjValue,"%d",*(int*)mynum);
        				asmCode+= space + ".word   " + ObjValue +endl;
        			
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
        		asmCode+= space + ".space   " + ObjSpace +endl;
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
        				char ZeroSpace[50];
        				sprintf(ZeroSpace,"%d",zero);
        				asmCode+= space + ".space   " + ZeroSpace + endl;
        				if(j<ElementNum)
        					asmCode+= space + ".word    " + element->name +endl;
        			}
        			else
        				asmCode+= space + ".word    " + element->name +endl;
        		}
        	}
        	
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

}//namespace backend
