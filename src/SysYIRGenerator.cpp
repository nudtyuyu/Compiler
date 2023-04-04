#include <iostream>
#include <memory>
#include <vector>
#include <cstdio>
using namespace std;
#include "SysYIRGenerator.h"

namespace sysy {

any SysYIRGenerator::visitCompUnit(SysYParser::CompUnitContext *ctx)
{
	cout<<"visitCompUnit"<<endl;
	auto Unit = new Module;
	assert(Unit);
	module.reset(Unit);
	// Decl: createGlobal value
	idt.newTable();
	for(auto decl:ctx->decl())
	{
		GlobalVal  = true;
		//module->createGlobalValue()
		visitDecl(decl);
	}
	cout<<"visit decl finish!"<<endl;
	return 0;
}

any SysYIRGenerator::visitDecl(SysYParser::DeclContext *ctx)
{
	cout<<"visitDecl"<<endl;
	auto decl = (ctx->constDecl())?visitConstDecl(ctx->constDecl()):visitVarDecl(ctx->varDecl());
	GlobalVal = false;
	LocalVal = false;
	return decl;
}

any SysYIRGenerator::visitConstDecl(SysYParser::ConstDeclContext *ctx)
{
	cout<<"visitConstDecl"<<endl;
	vector <Value*> values;
	auto type = any_cast<Type*>(visitBType(ctx->bType()));
	for(auto constdef:ctx->constDef())
	{
		auto name = constdef->Identifier()->getText();
		vector <Value *> dims;
		if(!constdef->constExp().empty())
		{
			for(auto constexp:constdef->constExp())
			{
			dims.push_back(any_cast<Value*>(visitConstExp(constexp)));
			}
			auto alloca = builder.createAllocaInst(type,dims,name);
			if(constdef->Assign())
			{
				auto value = any_cast<Value*>(visitConstInitVal(constdef->constInitVal()));
				//Init List???
				//auto store = builder.createStoreInst(value,alloca);
				
			}
			values.push_back(alloca);
			if(GlobalVal)
			{
				//auto gval = module->createGlobalValue(name,type,dims);
				//gval->HaveInit();
				idt.insert(name,alloca);
			}
			
			
		}
		else
		{
			auto alloca = builder.createAllocaInst(type,{},name);
			if(constdef->Assign())
			{
				auto value = any_cast<Value*>(visitConstInitVal(constdef->constInitVal()));
				auto store = builder.createStoreInst(value,alloca);
			}
			else
			{
				if(type->isInt())
				{
					int a = 0;
					Value* value = (Value*)ConstantValue::get(a);
					auto store = builder.createStoreInst(value,alloca);
				}
				else if(type->isFloat())
				{
					float a = 0.0;
					Value* value = (Value*)ConstantValue::get(a);
					auto store = builder.createStoreInst(value,alloca);
				}
			
			}
			values.push_back(alloca);
			if(GlobalVal)
			{
				//auto gval = module->createGlobalValue(name,type,{});
				//gval->HaveInit();
				idt.insert(name,alloca);
			}
		}
	}
	return values;
}

any SysYIRGenerator::visitBType(SysYParser::BTypeContext *ctx)
{
	cout<<"visitBType"<<endl;
	return ctx->Int() ? Type::getIntType() : Type::getFloatType();
}

// visitConstDef here

any SysYIRGenerator::visitConstInitVal(SysYParser::ConstInitValContext *ctx)
{
	cout<<"visitConstInitVal"<<endl;
	if(ctx->constExp())
	{
		auto constexp = visitConstExp(ctx->constExp());
		return constexp;
	}
	else
	{
		vector <Value*> values;
		for(auto constinit:ctx->constInitVal())
		{
			values.push_back(any_cast<Value*>(visitConstInitVal(constinit)));
		}
		return values;
	}
	return nullptr;
}


any SysYIRGenerator::visitVarDecl(SysYParser::VarDeclContext *ctx)
{
	cout<<"visitVarDecl"<<endl;
	vector <Value*> values;
	auto type = any_cast<Type*>(visitBType(ctx->bType()));
	for(auto vardef:ctx->varDef())
	{
		auto name = vardef->Identifier()->getText();
		vector <Value *> dims;
		if(!vardef->constExp().empty())
		{
			for(auto constexp:vardef->constExp())
			{
			dims.push_back(any_cast<Value*>(visitConstExp(constexp)));
			}
			auto alloca = builder.createAllocaInst(type,dims,name);
			if(vardef->Assign())
			{
				auto value = any_cast<Value*>(visitInitVal(vardef->initVal()));
				//Init List???
				//auto store = builder.createStoreInst(value,alloca);
			}
			values.push_back(alloca);
			if(GlobalVal)
			{
				//auto gval = module->createGlobalValue(name,type,dims);
				//gval->HaveInit();
				idt.insert(name,alloca);
			}
			
		}
		else
		{
			auto alloca = builder.createAllocaInst(type,{},name);
			if(vardef->Assign())
			{
				auto value = any_cast<Value*>(visitInitVal(vardef->initVal()));
				cout<<"getVardefAssignInitVal"<<endl;
				auto store = builder.createStoreInst(value,alloca);
				cout<<"createStoreInst"<<endl;
			}
			else
			{
				if(type->isInt())
				{
					int a = 0;
					Value* value = (Value*)ConstantValue::get(a);
					auto store = builder.createStoreInst(value,alloca);
				}
				else if(type->isFloat())
				{
					float a = 0.0;
					Value* value = (Value*)ConstantValue::get(a);
					auto store = builder.createStoreInst(value,alloca);
				}
			
			}
			values.push_back(alloca);
			if(GlobalVal)
			{
				//auto gval = module->createGlobalValue(name,type,{});
				idt.insert(name,alloca);
				cout<<"have create globalvalue"<<endl;
				//gval->HaveInit();
				cout<<"Have Init"<<endl;
			}
		}
		
	}
	return values;
}

// visitVarDef here

any SysYIRGenerator::visitInitVal(SysYParser::InitValContext *ctx)
{
	cout<<"visitInitVal"<<endl;
	if(ctx->exp())
	{
		auto constexp = visitExp(ctx->exp());
		return constexp;
	}
	else
	{
		vector <Value*> values;
		for(auto initv:ctx->initVal())
		{
			values.push_back(any_cast<Value*>(visitInitVal(initv)));
		}
		return values;
	}
	return nullptr;
}

// visitFuncDef here

// visitFuncType here

// visitFuncFParams here

// visitFuncFParam here

// visitBlock here

// visitBlockItem here

std::any SysYIRGenerator::visitStmt(SysYParser::StmtContext *ctx) {
    Value *ret = nullptr;
    if (ctx->lVal() != nullptr) {
        Value *ptr = any_cast<Value *>(visitLVal(ctx->lVal()));
        Value *exp = any_cast<Value *>(visitExp(ctx->exp()));
        ret = builder.createStoreInst(exp, ptr);
    } else if (ctx->Return() != nullptr) {
        Value *value = nullptr;
        if (ctx->exp() != nullptr)
            value = any_cast<Value *>(visitExp(ctx->exp()));
        ret = builder.createReturnInst(value);
    } else if (ctx->If() != nullptr) {
        auto *block = builder.getBasicBlock();   ///< 当前所在基本块
        auto *func = block->getParent();         ///< 当前所在函数
        auto *thenBlock = func->addBasicBlock(); ///< then分支基本块
        BasicBlock *elseBlock = nullptr;         ///< else分支基本块（可能没有）
        auto *exitBlock = func->addBasicBlock(); ///< if-then-else结束后的部分
        std::vector<Value *> thenArgs, elseArgs; ///< then/else基本块的实参列表，暂时用不上，留空即可
        auto *cond = any_cast<Value *>(visitCond(ctx->cond()));

        // translate thenblock
        block->getSuccessors().push_back(thenBlock);
        thenBlock->getPredecessors().push_back(block);
        builder.setPosition(thenBlock, thenBlock->end());
        visitStmt(ctx->stmt()[0]);
        builder.setPosition(block, block->end());

        // translate elseblock
        if (ctx->Else() != nullptr) {
            elseBlock = func->addBasicBlock();
            block->getSuccessors().push_back(elseBlock);
            elseBlock->getPredecessors().push_back(block);
            builder.setPosition(thenBlock, elseBlock->end());
            visitStmt(ctx->stmt()[1]);
            builder.setPosition(block, elseBlock->end());
        }

        ret = builder.createCondBrInst(cond, thenBlock, elseBlock, thenArgs, elseArgs);

        // prepare to translate exitblock
        block->getSuccessors().push_back(exitBlock);
        exitBlock->getPredecessors().push_back(block);
        builder.setPosition(exitBlock, exitBlock->end());
    } else if (ctx->While() != nullptr) {
        auto *block = builder.getBasicBlock();    ///< 当前所在基本块
        auto *func = block->getParent();          ///< 当前所在函数
        auto *entryBlock = func->addBasicBlock(); ///< 循环入口，即循环条件计算和判断
        auto *innerBlock = func->addBasicBlock(); ///< while循环体块
        auto *exitBlock = func->addBasicBlock();  ///< while循环结束后的部分
        std::vector<Value *> innerArgs;           ///< 循环体块实参列表，暂时留空

        block->getSuccessors().push_back(entryBlock);
        entryBlock->getPredecessors().push_back(block);
        entryBlock->getSuccessors().push_back(innerBlock);
        innerBlock->getPredecessors().push_back(entryBlock);
        entryBlock->getSuccessors().push_back(exitBlock);
        exitBlock->getPredecessors().push_back(entryBlock);

        // translate cond/entry block
        builder.setPosition(entryBlock, entryBlock->end());
        auto *cond = any_cast<Value *>(visitCond(ctx->cond()));
        ret = builder.createCondBrInst(cond, innerBlock, nullptr, innerArgs, std::vector<Value *>());

        // translate inner block
        builder.setPosition(innerBlock, innerBlock->end());
        loopEntry.push_back(entryBlock);
        loopExit.push_back(exitBlock);
        visitStmt(ctx->stmt()[0]);
        loopEntry.pop_back();
        loopExit.pop_back();

        // prepare to translate exitblock
        builder.setPosition(exitBlock, exitBlock->end());
    } else if (ctx->Break()) {
        std::vector<Value *> args; ///< 留空
        ret = builder.createUncondBrInst(loopExit.back(), args);
    } else if (ctx->Continue()) {
        std::vector<Value *> args; ///< 留空
        ret = builder.createUncondBrInst(loopEntry.back(), args);
    } else if (ctx->block()) {
        ret = any_cast<Value *>(visitBlock(ctx->block()));
    } else if (ctx->exp()) {
        ret = any_cast<Value *>(visitExp(ctx->exp()));
    }
    return ret;
}

any SysYIRGenerator::visitExp(SysYParser::ExpContext *ctx)
{
	cout<<"visitExp"<<endl;
	return visitAddExp(ctx->addExp());
}

any SysYIRGenerator::visitCond(SysYParser::CondContext* ctx)
{
	cout<<"visitCond"<<endl;
	return visitLOrExp(ctx->lOrExp());
}

any SysYIRGenerator::visitLVal(SysYParser::LValContext *ctx) {
    auto name = ctx->Identifier()->getText();
    vector<Value *> exps;
    auto *ident = idt.query(name);
    assert(ident != nullptr);
    cout << "visitLVal: " << ctx->getText() << endl;

    for (auto *exp : ctx->exp()) {
        exps.push_back(any_cast<Value *>(visitExp(exp)));
    }

    auto *base = dynamic_cast<AllocaInst *>(ident);
    assert(base != nullptr);
    int ndim = base->getNumDims();
    Value *pOffset = builder.createAllocaInst(Type::getPointerType(Type::getIntType()));
    builder.createStoreInst(ConstantValue::get(0), pOffset);
    Value *offset = builder.createLoadInst(pOffset);

    if (ndim != 0) {
        offset = builder.createPAddInst(offset, exps[0]);
    }
    for (int i = 1; i < ndim; ++i) {
        offset = builder.createPMulInst(base->getDim(i), offset);
        offset = builder.createPAddInst(offset, exps[i]);
    }

    Value *ptr = builder.createPAddInst(base, offset);
    return ptr;
}

any SysYIRGenerator::visitPrimaryExp(SysYParser::PrimaryExpContext *ctx)
{
	cout<<"visitPrimaryExp"<<endl;
	if(ctx->LeftBracket())
	{
		return visitExp(ctx->exp());
	}
	else if(ctx->lVal())
	{
		cout<<"getLVal"<<endl;
		auto addr = any_cast<Value*>(visitLVal(ctx->lVal()));//waiting
		cout<<"get addr: "<<endl;
		idt.view();
		auto value = builder.createLoadInst(addr);
		cout<<"load value"<<endl;
		//idt.view();
		return (Value *)value;
		//cout<<"This is LVal, waiting to edit"<<endl;
	}
	else if(ctx->number())
	{
		auto num = visitNumber(ctx->number());//waiting
		cout<<"getnum"<<endl;
		return num;
	}
	return (Value *)nullptr;
	
}

any SysYIRGenerator::visitNumber(SysYParser::NumberContext *ctx)
{
	cout<<"visitNumber"<<endl;
	//return ctx->IntConst() ? Type::getInt() : Type::getFloat();
	if(ctx->IntConst())
	{
		//return module->createGlobalValue(ctx->IntConst()->getText(),Type::getIntType());	
		//return ctx->IntConst();
		int value = atoi(ctx->IntConst()->getText().c_str());
		return (Value*)ConstantValue::get(value);
			
	}
	else if(ctx->FloatConst())
	{
		//return module->createGlobalValue(ctx->FloatConst()->getText(),Type::getFloatType());
		//return ctx->FloatConst();
		float value = atof(ctx->FloatConst()->getText().c_str());
		return (Value*)ConstantValue::get(value);		
	}
	return (Value*)nullptr;
}

// visitString here


any SysYIRGenerator::visitUnaryExp(SysYParser::UnaryExpContext *ctx)
{
	auto *child = ctx->children[0];
	auto *Primary_child = dynamic_cast<SysYParser::PrimaryExpContext *>(child);
	auto *UnaryOp_child = dynamic_cast<SysYParser::UnaryExpContext *>(child);
	cout<<"visitUnaryExp"<<endl;
	if(Primary_child != nullptr)
	{
		
		auto pri = visitPrimaryExp(ctx->primaryExp());
		cout<<"got visitPrimaryExp"<<endl;
		return pri;
	}
	else if(ctx->Identifier())
	{
		cout<<"function calling!"<<endl;
		auto callee = module->getFunction(ctx->Identifier()->getText());
		vector <Value*> args;
		auto params = ctx->funcRParams()->exp();
		auto entry = callee->getEntryBlock();
		auto parent = entry->getParent();
		cout<<"entry yes!"<<endl;
		//auto parent = BasicBlock::getParent();
		for(auto param:params)
		{
			cout<<"param"<<endl;
			args.push_back(any_cast<Value *>(visitExp(param)));
		}
		
		return builder.createCallInst(callee,args,entry);
		
		
	} //identifier table!!!
	else if(UnaryOp_child != nullptr)
	{
		Value* term = any_cast<Value *>(visitUnaryExp(ctx->unaryExp()));
		if(term->isInt())
		{
			if(ctx->unaryOp()->Add())
			{
			
				//create
				auto unary = builder.createPosInst(term);
				cout<<"PositiveUnary: wait for edit!"<<endl;
				return unary;
			}
			else if(ctx->unaryOp()->Sub())
			{
				// Type! Int or Float!
				auto unary = builder.createNegInst(term);
				cout<<"getunarysub"<<endl;
				return unary;
			}
			else if(ctx->unaryOp()->Not())
			{
				// Type! Int or Float!
				auto unary = builder.createNotInst(term);
				cout<<"getunarynot"<<endl;
				return unary;
			}
		}
		if(term->isFloat())
		{
			if(ctx->unaryOp()->Add())
			{
			
				//create
				auto unary = builder.createFPosInst(term);
				cout<<"PositiveUnary: wait for edit!"<<endl;
				return unary;
			}
			else if(ctx->unaryOp()->Sub())
			{
				// Type! Int or Float!
				auto unary = builder.createFNegInst(term);
				cout<<"getunarysub"<<endl;
				return unary;
			}
			else if(ctx->unaryOp()->Not())
			{
				// Type! Int or Float!
				auto unary = builder.createNotInst(term);
				cout<<"getunarynot"<<endl;
				return unary;
			}
		}
		
		
	}
	return nullptr;
}

// visitUnaryOp here

// visitFuncRParams here

any SysYIRGenerator::visitMulExp(SysYParser::MulExpContext *ctx)
{
	auto *child = ctx->children[0];
	auto *Mul_child = dynamic_cast<SysYParser::MulExpContext *>(child);
	auto *Unary_child = dynamic_cast<SysYParser::UnaryExpContext *>(child);
	//auto mul;
	cout<<"visitMulExp"<<endl;
	if(Unary_child != nullptr)
	{
		auto unary = visitUnaryExp(ctx->unaryExp());
		cout<<"getmulunary"<<endl;
		return unary;
	}
	else if(Mul_child !=nullptr)
	{
		Value* unaryterm = any_cast<Value *>(visitUnaryExp(ctx->unaryExp()));
		Value* multerm = any_cast<Value *>(visitMulExp(ctx->mulExp()));
		// Type tansfer
		if(unaryterm->isInt() && multerm->isInt())
		{
			if(ctx->Mul())
			{
				auto mul = builder.createMulInst(multerm,unaryterm);
				cout<<"getmul"<<endl;
				return mul;
			}
			else if(ctx->Div())
			{
				auto div = builder.createDivInst(multerm,unaryterm);
				cout<<"getdiv"<<endl;
				return div;
			}
			else if(ctx->Mod())
			{
			
				auto mod = builder.createRemInst(multerm,unaryterm);
				cout<<"getmod"<<endl;
				return mod;
			}
		}
		else if(unaryterm->isFloat() && multerm->isInt())
		{
			cout<<"floatunary!"<<endl;
			builder.createIToFInst(multerm);
			if(ctx->Mul())
			{
				auto mul = builder.createFMulInst(multerm,unaryterm);
				cout<<"getmul"<<endl;
				return mul;
			}
			else if(ctx->Div())
			{
				auto div = builder.createFDivInst(multerm,unaryterm);
				cout<<"getdiv"<<endl;
				return div;
			}
			else if(ctx->Mod())
			{
			
				auto mod = builder.createFRemInst(multerm,unaryterm);
				cout<<"getmod"<<endl;
				return mod;
			}
		}
		else if(unaryterm->isInt() && multerm->isFloat())
		{
			cout<<"floatmul!"<<endl;
			builder.createIToFInst(unaryterm);
			if(ctx->Mul())
			{
				auto mul = builder.createFMulInst(multerm,unaryterm);
				cout<<"getmul"<<endl;
				return mul;
			}
			else if(ctx->Div())
			{
				auto div = builder.createFDivInst(multerm,unaryterm);
				cout<<"getdiv"<<endl;
				return div;
			}
			else if(ctx->Mod())
			{
			
				auto mod = builder.createFRemInst(multerm,unaryterm);
				cout<<"getmod"<<endl;
				return mod;
			}
		}
		else if(unaryterm->isFloat() && multerm->isFloat())
		{
			cout<<"floatmul!"<<endl;
			//builder.createIToFInst(multerm);
			if(ctx->Mul())
			{
				auto mul = builder.createFMulInst(multerm,unaryterm);
				cout<<"getmul"<<endl;
				return mul;
			}
			else if(ctx->Div())
			{
				auto div = builder.createFDivInst(multerm,unaryterm);
				cout<<"getdiv"<<endl;
				return div;
			}
			else if(ctx->Mod())
			{
			
				auto mod = builder.createFRemInst(multerm,unaryterm);
				cout<<"getmod"<<endl;
				return mod;
			}
		}
		
		
		
	}
	return nullptr;
	
}

any SysYIRGenerator::visitAddExp(SysYParser::AddExpContext *ctx)
{
	auto *child = ctx->children[0];
	auto *Mul_child = dynamic_cast<SysYParser::MulExpContext *>(child);
	auto *Add_child = dynamic_cast<SysYParser::AddExpContext *>(child);
	//auto add;
	cout<<"visitAddExp"<<endl;
	if(Mul_child != nullptr)
	{
		auto mul = visitMulExp(ctx->mulExp());
		cout<<"getaddmul"<<endl;
		return mul;
	}
	else if(Add_child != nullptr)
	{
		//type transfer
		Value* addterm = any_cast<Value *>(visitAddExp(ctx->addExp()));
		cout<<"visitadd:getaddterm"<<endl;
		Value* multerm = any_cast<Value *>(visitMulExp(ctx->mulExp())); //wait
		cout<<"visitadd:getmulterm"<<endl;
		if(addterm->isInt() && multerm->isInt())
		{
			if(ctx->Add())
			{
				cout<<"getadd"<<endl;
				auto add = builder.createAddInst(addterm, multerm); //name????
			
				return add;
			}
			else if(ctx->Sub())
			{
				auto sub = builder.createSubInst(addterm, multerm); //name????
				cout<<"getsub"<<endl;
				return sub;
			}
		}
		else if(addterm->isFloat() && multerm->isInt())
		{
			cout<<"float!"<<endl;
			builder.createIToFInst(multerm);
			if(ctx->Add())
			{
				cout<<"getfloatadd"<<endl;
				auto add = builder.createFAddInst(addterm, multerm); //name????
				return add;
			}
			else if(ctx->Sub())
			{
				auto sub = builder.createFSubInst(addterm, multerm); //name????
				cout<<"getfloatsub"<<endl;
				return sub;
			}
		}
		else if(addterm->isInt() && multerm->isFloat())
		{
			cout<<"float!"<<endl;
			builder.createIToFInst(addterm);
			if(ctx->Add())
			{
				cout<<"getfloatadd"<<endl;
				auto add = builder.createFAddInst(addterm, multerm); //name????
			
				return add;
			}
			else if(ctx->Sub())
			{
				auto sub = builder.createFSubInst(addterm, multerm); //name????
				cout<<"getfloatsub"<<endl;
				return sub;
			}
		}
		else if(addterm->isFloat() && multerm->isFloat())
		{
			cout<<"float!"<<endl;
			//builder.createIToFInst(multerm);
			if(ctx->Add())
			{
				cout<<"getfloatadd"<<endl;
				auto add = builder.createFAddInst(addterm, multerm); //name????
			
				return add;
			}
			else if(ctx->Sub())
			{
				auto sub = builder.createFSubInst(addterm, multerm); //name????
				cout<<"getfloatsub"<<endl;
				return sub;
			}
		}
		
	}
	return nullptr;
}

any SysYIRGenerator::visitRelExp(SysYParser::RelExpContext* ctx)
{
	auto *child = ctx->children[0];
	auto *Add_child = dynamic_cast<SysYParser::AddExpContext *>(child);
	auto *Rel_child = dynamic_cast<SysYParser::RelExpContext *>(child);
	if(Add_child != nullptr)
	{
		return visitAddExp(ctx->addExp());//waiting 
	}
	else if(Rel_child !=nullptr)
	{
		Value* addterm = any_cast<Value *>(visitAddExp(ctx->addExp()));
		Value* relterm = any_cast<Value *>(visitRelExp(ctx->relExp()));
		//lor = builder.create... undefine,waiting
		if(addterm->isInt() && relterm->isInt())
		{
			if(ctx->LessThan())
			{
				return builder.createICmpLTInst(relterm,addterm);
			}
			else if(ctx->GreaterThan())
			{
				return builder.createICmpGTInst(relterm,addterm);
			}
			else if(ctx->LessEqual())
			{
				return builder.createICmpLEInst(relterm,addterm);
			}
			else if(ctx->GreaterEqual())
			{
				return builder.createICmpGEInst(relterm,addterm);
			}	
		}
		else if(addterm->isFloat() && relterm->isInt())
		{
			builder.createIToFInst(relterm);
			if(ctx->LessThan())
			{
				return builder.createFCmpLTInst(relterm,addterm);
			}
			else if(ctx->GreaterThan())
			{
				return builder.createFCmpGTInst(relterm,addterm);
			}
			else if(ctx->LessEqual())
			{
				return builder.createFCmpLEInst(relterm,addterm);
			}
			else if(ctx->GreaterEqual())
			{
				return builder.createFCmpGEInst(relterm,addterm);
			}	
		}
		else if(addterm->isInt() && relterm->isFloat())
		{
			builder.createIToFInst(addterm);
			if(ctx->LessThan())
			{
				return builder.createFCmpLTInst(relterm,addterm);
			}
			else if(ctx->GreaterThan())
			{
				return builder.createFCmpGTInst(relterm,addterm);
			}
			else if(ctx->LessEqual())
			{
				return builder.createFCmpLEInst(relterm,addterm);
			}
			else if(ctx->GreaterEqual())
			{
				return builder.createFCmpGEInst(relterm,addterm);
			}	
		}
		else if(addterm->isFloat() && relterm->isFloat())
		{
			//builder.createIToFInst(relterm);
			if(ctx->LessThan())
			{
				return builder.createFCmpLTInst(relterm,addterm);
			}
			else if(ctx->GreaterThan())
			{
				return builder.createFCmpGTInst(relterm,addterm);
			}
			else if(ctx->LessEqual())
			{
				return builder.createFCmpLEInst(relterm,addterm);
			}
			else if(ctx->GreaterEqual())
			{
				return builder.createFCmpGEInst(relterm,addterm);
			}	
		}
		
			
	}
	return nullptr;
}

any SysYIRGenerator::visitEqExp(SysYParser::EqExpContext* ctx)
{
	auto *child = ctx->children[0];
	auto *Eq_child = dynamic_cast<SysYParser::EqExpContext *>(child);
	auto *Rel_child = dynamic_cast<SysYParser::RelExpContext *>(child);
	if(Rel_child != nullptr)
	{
		return visitRelExp(ctx->relExp());//waiting 
	}
	else if(Eq_child !=nullptr)
	{
		Value* eqterm = any_cast<Value *>(visitEqExp(ctx->eqExp()));
		Value* relterm = any_cast<Value *>(visitRelExp(ctx->relExp()));
		//lor = builder.create... undefine,waiting
		if(eqterm->isInt() && relterm->isInt())
		{
			if(ctx->Equal())
			{
				return builder.createICmpEQInst(eqterm,relterm);
			}
			else if(ctx->NonEqual())
			{
				return builder.createICmpNEInst(eqterm,relterm);
			}
		}
		else if(eqterm->isInt() && relterm->isFloat())
		{
			builder.createIToFInst(eqterm);
			if(ctx->Equal())
			{
				return builder.createFCmpEQInst(eqterm,relterm);
			}
			else if(ctx->NonEqual())
			{
				return builder.createFCmpNEInst(eqterm,relterm);
			}
		}
		else if(eqterm->isFloat() && relterm->isInt())
		{
			builder.createIToFInst(relterm);
			if(ctx->Equal())
			{
				return builder.createFCmpEQInst(eqterm,relterm);
			}
			else if(ctx->NonEqual())
			{
				return builder.createFCmpNEInst(eqterm,relterm);
			}
		}
		else if(eqterm->isFloat() && relterm->isFloat())
		{
			//builder.createIToF(eqterm);
			if(ctx->Equal())
			{
				return builder.createFCmpEQInst(eqterm,relterm);
			}
			else if(ctx->NonEqual())
			{
				return builder.createFCmpNEInst(eqterm,relterm);
			}
		}
		
			
	}
	return nullptr;
}

any SysYIRGenerator::visitLAndExp(SysYParser::LAndExpContext* ctx)
{
	auto *child = ctx->children[0];
	auto *And_child = dynamic_cast<SysYParser::LAndExpContext *>(child);
	auto *Eq_child = dynamic_cast<SysYParser::EqExpContext *>(child);
	if(Eq_child != nullptr)
	{
		return visitEqExp(ctx->eqExp());//waiting 
	}
	else if(And_child !=nullptr)
	{
		Value* andterm = any_cast<Value *>(visitLAndExp(ctx->lAndExp()));
		Value* eqterm = any_cast<Value *>(visitEqExp(ctx->eqExp()));
		// return builder.create... undefine,waiting
		//return ....
		auto land = builder.createAndInst(andterm,eqterm);
		return land;
	}
	return nullptr;
}

any SysYIRGenerator::visitLOrExp(SysYParser::LOrExpContext* ctx)
{
	auto *child = ctx->children[0];
	auto *And_child = dynamic_cast<SysYParser::LAndExpContext *>(child);
	auto *Or_child = dynamic_cast<SysYParser::LOrExpContext *>(child);
	if(And_child != nullptr)
	{
		return visitLAndExp(ctx->lAndExp());//waiting 
	}
	else if(Or_child !=nullptr)
	{
		Value* andterm = any_cast<Value *>(visitLAndExp(ctx->lAndExp()));
		Value* orterm = any_cast<Value *>(visitLOrExp(ctx->lOrExp()));
		//return builder.create... undefine,waiting
		auto lor = builder.createOrInst(andterm,orterm);
		return lor;
	}
	return nullptr;
}

any SysYIRGenerator::visitConstExp(SysYParser::ConstExpContext *ctx)
{
	cout<<"visitConstExp"<<endl;
	return visitAddExp(ctx->addExp());
}

/*any SysYIRGenerator::visitModule(SysYParser::ModuleContext *ctx) {
  auto pModule = new Module();
  assert(pModule);
  module.reset(pModule);
  visitChildren(ctx);
  return pModule;
}

any SysYIRGenerator::visitFunc(SysYParser::FuncContext *ctx) {
  auto name = ctx->ID()->getText();
  auto params = ctx->funcFParams()->funcFParam();
  vector<Type *> paramTypes;
  vector<string> paramNames;
  for (auto param : params) {
    paramTypes.push_back(any_cast<Type *>(visitBtype(param->btype())));
    paramNames.push_back(param->ID()->getText());
  }
  Type *returnType = any_cast<Type *>(visitFuncType(ctx->funcType()));
  auto funcType = Type::getFunctionType(returnType, paramTypes);
  auto function = module->createFunction(name, funcType);
  auto entry = function->getEntryBlock();
  for (auto i = 0; i < paramTypes.size(); ++i)
    entry->createArgument(paramTypes[i], paramNames[i]);
  builder.setPosition(entry, entry->end());
  visitBlockStmt(ctx->blockStmt());
  return function;
}
any SysYIRGenerator::visitBtype(SysYParser::BtypeContext *ctx) {
  return ctx->INT() ? Type::getIntType() : Type::getFloatType();
}

any SysYIRGenerator::visitBlockStmt(SysYParser::BlockStmtContext *ctx) {
  for (auto item : ctx->blockItem())
    visitBlockItem(item);
  return builder.getBasicBlock();
}

any SysYIRGenerator::visitBlockItem(SysYParser::BlockItemContext *ctx) {
  return ctx->decl() ? visitDecl(ctx->decl()) : visitStmt(ctx->stmt());
}

any SysYIRGenerator::visitDecl(SysYParser::DeclContext *ctx) {
  std::vector<Value *> values;
  auto type = any_cast<Type *>(visitBtype(ctx->btype()));
  for (auto varDef : ctx->varDef()) {
    auto name = varDef->lValue()->ID()->getText();
    auto alloca = builder.createAllocaInst(type, {}, name);
    if (varDef->ASSIGN()) {
      auto value = any_cast<Value *>(varDef->initValue()->accept(this));
      auto store = builder.createStoreInst(value, alloca);
    }
    values.push_back(alloca);
  }
  return values;
}*/

} // namespace sysy
