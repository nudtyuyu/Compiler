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
	for (auto *func : ctx->funcDef())
		visitFuncDef(func);
	
	cout << "finish!" << endl;
	return nullptr;
}


any SysYIRGenerator::visitDecl(SysYParser::DeclContext *ctx)
{
	cout<<"visitDecl"<<endl;
	auto decl = (ctx->constDecl())?visitConstDecl(ctx->constDecl()):visitVarDecl(ctx->varDecl());
	GlobalVal = false;
	LocalVal = false;
	return (decl);
}

any SysYIRGenerator::visitBType(SysYParser::BTypeContext *ctx)
{
	cout<<"visitBType"<<endl;
	return ctx->Int() ? Type::getIntType() : Type::getFloatType();
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
				auto dim = any_cast<Value*>(visitConstExp(constexp));
				if(dim->isInt())
					cout<<"dim is Int"<<endl;
				dims.push_back(dim);
			
			}
			auto alloca = builder.createAllocaInst(type,dims,name);
			DimNum = dims.size();
			if(constdef->Assign())
			{
				auto value = any_cast<std::vector<Value*>>(visitConstInitVal(constdef->constInitVal()));
				
				//Init List???
				//auto store = builder.createStoreInst(value,alloca);
				
			}
			values.push_back(alloca);
			if(GlobalVal)
			{
				//auto gval = module->createGlobalValue(name,Type::getPointerType(type),dims);
				//gval->HaveInit();
				//idt.insert(name,alloca);
			}
			if(LocalVal)
			{
				
				//Entry(alloca,type,
				//idt.insert(name,alloca);
			}
			
			
		}
		else
		{
			auto alloca = builder.createAllocaInst(type,{},name);
			int initV;
			float initV2;
			if(constdef->Assign())
			{
				auto value = any_cast<Value*>(visitConstInitVal(constdef->constInitVal()));
				cout<<"The Initial Value's Name: "<<value->name<<endl;
				auto vvi = module->getInteger(value->name);
				auto vvf = module->getFloat(value->name);
				if(type->isInt())
				{
					cout<<"The Initial Value's value: "<<(*vvi)<<endl;
					initV = *vvi;
				}
				else if(type->isFloat())
				{
					cout<<"The Initial Value's value: "<<(*vvf)<<endl;
					initV2 = *vvf;
				}
				auto store = builder.createStoreInst(value,alloca);
			}
			else
			{
				if(type->isInt())
				{
					int a = 0;
					initV = 0;
					Value* value = (Value*)ConstantValue::get(a);
					auto store = builder.createStoreInst(value,alloca);
				}
				else if(type->isFloat())
				{
					initV2 = 0.0;
					float a = 0.0;
					Value* value = (Value*)ConstantValue::get(a);
					auto store = builder.createStoreInst(value,alloca);
				}
			
			}
			values.push_back(alloca);
			if(type->isInt())
			{
				Entry entry(alloca,0,initV);
				idt.insert(name,entry);
			}
			else if(type->isFloat())
			{
				Entry entry(alloca,0,initV2);
				idt.insert(name,entry);
			}
		}
	}
	return values;
}

any SysYIRGenerator::visitConstInitVal(SysYParser::ConstInitValContext *ctx)
{
	cout<<"visitConstInitVal"<<endl;
	if(ctx->constExp())
	{
		auto constexp = visitConstExp(ctx->constExp());
		return (constexp);
	}
	else
	{
		vector <Value*> values;
		if(ctx->constInitVal().size()==0)
		{
			cout<<"constInitValSize: "<<0<<endl;
		}
		for(auto constinit:ctx->constInitVal())
		{
			cout<<"constInitValSize: "<<ctx->constInitVal().size()<<endl;
			auto value = any_cast<Value*>(visitConstInitVal(constinit));
			
			values.push_back(value);
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
				//auto gval = module->createGlobalValue(name,Type::getPointerType(type),dims);
				//gval->HaveInit();
				//idt.insert(name,alloca);
				
			}
			if(LocalVal)
			{
				//idt.insert(name,alloca);
				
			}
			
		}
		else
		{
			auto alloca = builder.createAllocaInst(type,{},name);
			int initV;
			float initV2;
			if(vardef->Assign())
			{
				auto value = any_cast<Value*>(visitInitVal(vardef->initVal()));
				cout<<"The Initial Value's Name: "<<value->name<<endl;
				auto vvi = module->getInteger(value->name);
				auto vvf = module->getFloat(value->name);
				if(type->isInt())
				{
					cout<<"The Initial Value's value: "<<*vvi<<endl;
					initV = *vvi;
				}
				else if(type->isFloat())
				{
					cout<<"The Initial Value's value: "<<*vvf<<endl;
					initV2 = *vvf;
				}
				cout<<"getVardefAssignInitVal"<<endl;
				auto store = builder.createStoreInst(value,alloca);
				cout<<"createStoreInst"<<endl;
			}
			else
			{
				if(type->isInt())
				{
					int a = 0;
					initV  = 0;
					Value* value = (Value*)ConstantValue::get(a);
					auto store = builder.createStoreInst(value,alloca);
				}
				else if(type->isFloat())
				{
					float a = 0.0;
					initV2  = 0.0;
					Value* value = (Value*)ConstantValue::get(a);
					auto store = builder.createStoreInst(value,alloca);
				}
			
			}
			values.push_back(alloca);
			if(type->isInt())
			{
				Entry entry(alloca,0,initV);
				idt.insert(name,entry);
			}
			else if(type->isFloat())
			{
				Entry entry(alloca,0,initV2);
				idt.insert(name,entry);
			}
		}
		
	}
	return values;
}

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

any SysYIRGenerator::visitFuncDef(SysYParser::FuncDefContext *ctx) {
	Type *retType = any_cast<Type *>(visitFuncType(ctx->funcType()));
	vector<Type *> paramTypes;
	vector<string> paramNames;
	for (auto *fparam : ctx->funcFParams()->funcFParam()) {
		paramTypes.emplace_back(any_cast<Type *>(visitBType(fparam->bType())));
		paramNames.emplace_back(fparam->Identifier()->getText());
	}
	Type *funcType = Type::getFunctionType(retType, paramTypes);
	auto *function = module->createFunction(ctx->Identifier()->getText(), funcType);
	auto *entry = function->getEntryBlock();
	builder.setPosition(entry->end());
	idt.newTable();
	for (int i = 0; i < paramTypes.size(); ++i) {
		auto *arg = entry->createArgument(paramTypes[i], paramNames[i]);
		auto *ptr = builder.createAllocaInst(paramTypes[i]);
		builder.createStoreInst(arg, ptr);
		// 这里还要插入符号表，先等符号表的实现定下来
	}
	idt.view();
	visitBlock(ctx->block());
	idt.destroyTop();

	return function;
}

any SysYIRGenerator::visitFuncType(SysYParser::FuncTypeContext *ctx) {
	if (ctx->Float())
		return Type::getFloatType();
	else if (ctx->Int())
		return Type::getIntType();
	else
		return Type::getVoidType();
}

any SysYIRGenerator::visitBlock(SysYParser::BlockContext *ctx) {
	for (auto *blockItem: ctx->blockItem())
		visitBlockItem(blockItem);
	return nullptr;
}

any SysYIRGenerator::visitBlockItem(SysYParser::BlockItemContext *ctx) {
	if (ctx->decl() != nullptr)
		visitDecl(ctx->decl());
	else visitStmt(ctx->stmt());
	return nullptr;
}

any SysYIRGenerator::visitStmt(SysYParser::StmtContext *ctx) {
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
    auto *entry = idt.query(name);
    assert(entry != nullptr);
    auto *ident = entry->ptr;
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

    Value *ptr = builder.createPAddInst(base, offset,name);
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
		auto name = addr->name;
		cout<<"the addr name: "<<name<<endl;
		auto *ptr = idt.query(name);
		//auto *found = module->getInteger(name);
		if(ptr!=nullptr)
		{
			
			int Var = ptr->ValType;
			auto *ident = dynamic_cast<PointerType*>(ptr->ptr->getType());
			auto *identType = ident->getBaseType();
			cout<<"ValType: "<<Var<<endl;
			if(Var==0)
			{
				if(identType->isInt())
				{
					cout<<"add const val to module integer!"<<endl;
					module->createInteger(name,ptr->iValue);
				}
				else if(identType->isFloat())
				{
					cout<<"add const val to module float!"<<endl;
					module->createFloat(name,ptr->fValue);
				}
			}
		}
		auto value = builder.createLoadInst(addr,{},name);
		cout<<"load value"<<endl;
		//idt.view();
		return (Value *)value;
		//cout<<"This is LVal, waiting to edit"<<endl;
	}
	else if(ctx->number())
	{
		auto num = visitNumber(ctx->number());//waiting
		auto testNum = any_cast<Value*>(num);
		cout<<"num: "<<testNum->name<<endl;
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
		module->createInteger(ctx->IntConst()->getText(),value);
		const std::string name = ctx->IntConst()->getText();
		auto cv = module->getInteger(name);
		cout<<"mynum: "<<(*cv)<<endl;
		//auto cv = (Value*)ConstantValue::get(value,name);
		//cout<<"mynum: "<<cv->name<<endl;
		return (Value*)ConstantValue::get(value,ctx->IntConst()->getText());
			
	}
	else if(ctx->FloatConst())
	{
		//return module->createGlobalValue(ctx->FloatConst()->getText(),Type::getFloatType());
		//return ctx->FloatConst();
		float value = atof(ctx->FloatConst()->getText().c_str());
		module->createFloat(ctx->FloatConst()->getText(),value);
		
		//auto name = ctx->FloatConst()->getText();
		//auto cv = (Value*)ConstantValue::get(value,name);
		//cout<<"mynum: "<<cv->name<<endl;
		return (Value*)ConstantValue::get(value,ctx->FloatConst()->getText());		
	}
	return (Value*)nullptr;
}

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
		
		return (Value*) builder.createCallInst(callee,args,entry,ctx->Identifier()->getText());
		
		
	} 
	else if(UnaryOp_child != nullptr)
	{
		Value* term = any_cast<Value *>(visitUnaryExp(ctx->unaryExp()));
		if(term->isInt())
		{
			if(ctx->unaryOp()->Add())
			{
			
				//create
				auto name = '+'+term->name;
				auto vp = module->getInteger(term->name);
				if(vp!=nullptr)
				{
					int v = *vp;
					v = v;
					module->createInteger(name,v);
				}
				auto unary = builder.createPosInst(term,name);
				cout<<"PositiveUnary: wait for edit!"<<endl;
				return (Value*)unary;
			}
			else if(ctx->unaryOp()->Sub())
			{
				// Type! Int or Float!
				auto name = '-'+term->name;
				auto vp = module->getInteger(term->name);
				if(vp!=nullptr)
				{
					int v = *vp;
					v = -v;
					module->createInteger(name,v);
				}
				auto unary = builder.createNegInst(term,name);
				cout<<"getunarysub"<<endl;
				return (Value*)unary;
			}
			else if(ctx->unaryOp()->Not())
			{
				// Type! Int or Float!
				auto name = '!'+term->name;
				auto vp = module->getInteger(term->name);
				if(vp!=nullptr)
				{
					int v = *vp;
					v = !v;
					module->createInteger(name,v);
				}
				auto unary = builder.createNotInst(term,name);
				cout<<"getunarynot"<<endl;
				return (Value*)unary;
			}
		}
		if(term->isFloat())
		{
			if(ctx->unaryOp()->Add())
			{
			
				//create
				auto name = '+'+term->name;
				auto vp = module->getFloat(term->name);
				if(vp!=nullptr)
				{
					float v = *vp;
					v = v;
					module->createFloat(name,v);
				}
				auto unary = builder.createFPosInst(term,name);
				cout<<"PositiveUnary: wait for edit!"<<endl;
				return (Value*)unary;
			}
			else if(ctx->unaryOp()->Sub())
			{
				// Type! Int or Float!
				auto name = '-'+term->name;
				auto vp = module->getFloat(term->name);
				if(vp!=nullptr)
				{
					float v = *vp;
					v = -v;
					module->createFloat(name,v);
				}
				auto unary = builder.createFNegInst(term,name);
				cout<<"getunarysub"<<endl;
				return (Value*)unary;
			}
			else if(ctx->unaryOp()->Not()) //question???
			{
				// Type! Int or Float!
				auto unary = builder.createNotInst(term);
				cout<<"getunarynot"<<endl;
				return (Value*)unary;
			}
		}
		
		
	}
	return nullptr;
}

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
			auto nameU = unaryterm->name;
			auto nameM = multerm->name;
			auto vpU = module->getInteger(nameU);
			auto vpM = module->getInteger(nameM);
			if(ctx->Mul())
			{
				auto name = nameM+'*'+nameU;
				if(vpU!=nullptr && vpM !=nullptr)
				{
					int u = *vpU;
					int m = *vpM;
					int v = u*m;
					module->createInteger(name,v);
				}
				auto mul = builder.createMulInst(multerm,unaryterm,name);
				cout<<"getmul"<<endl;
				return (Value*)mul;
			}
			else if(ctx->Div())
			{
				auto name = nameM+'/'+nameU;
				if(vpU!=nullptr && vpM !=nullptr)
				{
					int u = *vpU;
					int m = *vpM;
					int v = m/u;
					module->createInteger(name,v);
				}
				auto div = builder.createDivInst(multerm,unaryterm,name);
				cout<<"getdiv"<<endl;
				return (Value*)div;
			}
			else if(ctx->Mod())
			{
				auto name = nameM+'%'+nameU;
				if(vpU!=nullptr && vpM !=nullptr)
				{
					int u = *vpU;
					int m = *vpM;
					int v = m%u;
					module->createInteger(name,v);
				}
				auto mod = builder.createRemInst(multerm,unaryterm,name);
				cout<<"getmod"<<endl;
				return (Value*)mod;
			}
		}
		else if(unaryterm->isFloat() && multerm->isInt())
		{
			auto nameU = unaryterm->name;
			auto nameM = multerm->name;
			auto vpU = module->getFloat(nameU);
			auto vpM = module->getInteger(nameM);
			cout<<"floatunary!"<<endl;
			builder.createIToFInst(multerm);
			if(ctx->Mul())
			{
				auto name = nameM+'*'+nameU;
				if(vpU!=nullptr && vpM !=nullptr)
				{
					float u = *vpU;
					int m = *vpM;
					float v = u*m;
					module->createFloat(name,v);
				}
				auto mul = builder.createFMulInst(multerm,unaryterm,name);
				cout<<"getmul"<<endl;
				return (Value*)mul;
			}
			else if(ctx->Div())
			{
				auto name = nameM+'/'+nameU;
				if(vpU!=nullptr && vpM !=nullptr)
				{
					float u = *vpU;
					int m = *vpM;
					float v = m/u;
					module->createFloat(name,v);
				}
				auto div = builder.createFDivInst(multerm,unaryterm,name);
				cout<<"getdiv"<<endl;
				return (Value*)div;
			}
			else if(ctx->Mod())
			{
				cout<<"invalid operands of types ‘int’ and ‘float’ to binary ‘operator%’"<<endl;
			}
		}
		else if(unaryterm->isInt() && multerm->isFloat())
		{	
			auto nameU = unaryterm->name;
			auto nameM = multerm->name;
			auto vpU = module->getInteger(nameU);
			auto vpM = module->getFloat(nameM);
			cout<<"floatmul!"<<endl;
			builder.createIToFInst(unaryterm);
			if(ctx->Mul())
			{
				auto name = nameM+'*'+nameU;
				if(vpU!=nullptr && vpM !=nullptr)
				{
					int u = *vpU;
					float m = *vpM;
					float v = u*m;
					module->createFloat(name,v);
				}
				auto mul = builder.createFMulInst(multerm,unaryterm,name);
				cout<<"getmul"<<endl;
				return (Value*)mul;
			}
			else if(ctx->Div())
			{
				auto name = nameM+'/'+nameU;
				if(vpU!=nullptr && vpM !=nullptr)
				{
					int u = *vpU;
					float m = *vpM;
					float v = m/u;
					module->createFloat(name,v);
				}
				auto div = builder.createFDivInst(multerm,unaryterm,name);
				cout<<"getdiv"<<endl;
				return (Value*)div;
			}
			else if(ctx->Mod())
			{
				cout<<"invalid operands of types ‘int’ and ‘float’ to binary ‘operator%’"<<endl;
			}
		}
		else if(unaryterm->isFloat() && multerm->isFloat())
		{
			cout<<"floatmul!"<<endl;
			auto nameU = unaryterm->name;
			auto nameM = multerm->name;
			auto vpU = module->getFloat(nameU);
			auto vpM = module->getFloat(nameM);
			//builder.createIToFInst(multerm);
			if(ctx->Mul())
			{
				auto name = nameM+'*'+nameU;
				if(vpU!=nullptr && vpM !=nullptr)
				{
					float u = *vpU;
					float m = *vpM;
					float v = m*u;
					module->createFloat(name,v);
				}
				auto mul = builder.createFMulInst(multerm,unaryterm,name);
				cout<<"getmul"<<endl;
				return (Value*)mul;
			}
			else if(ctx->Div())
			{
				auto name = nameM+'/'+nameU;
				if(vpU!=nullptr && vpM !=nullptr)
				{
					float u = *vpU;
					float m = *vpM;
					float v = m/u;
					module->createFloat(name,v);
				}
				auto div = builder.createFDivInst(multerm,unaryterm,name);
				cout<<"getdiv"<<endl;
				return (Value*)div;
			}
			else if(ctx->Mod())
			{
				cout<<"invalid operands of types ‘int’ and ‘float’ to binary ‘operator%’"<<endl;
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
			auto nameA = addterm->name;
			auto nameM = multerm->name;
			auto vpA = module->getInteger(nameA);
			auto vpM = module->getInteger(nameM);
			if(ctx->Add())
			{
				auto name = nameA+'+'+nameM;
				cout<<"add code : "<<name<<endl;
				if(vpA!=nullptr && vpM !=nullptr)
				{
					cout<<"vpA and vpM is ok!"<<endl;
					int a = *vpA;
					int m = *vpM;
					int v = a+m;
					module->createInteger(name,v);
				}
				cout<<"getadd"<<endl;
				auto add = builder.createAddInst(addterm, multerm,name); //name????
			
				return (Value*)add;
			}
			else if(ctx->Sub())
			{
				auto name = nameA+'-'+nameM;
				if(vpA!=nullptr && vpM !=nullptr)
				{
					int a = *vpA;
					int m = *vpM;
					int v = a-m;
					module->createInteger(name,v);
				}
				auto sub = builder.createSubInst(addterm, multerm,name); //name????
				cout<<"getsub"<<endl;
				return (Value*)sub;
			}
		}
		else if(addterm->isFloat() && multerm->isInt())
		{
			auto nameA = addterm->name;
			auto nameM = multerm->name;
			auto vpA = module->getFloat(nameA);
			auto vpM = module->getInteger(nameM);
			cout<<"float!"<<endl;
			builder.createIToFInst(multerm);
			if(ctx->Add())
			{
				auto name = nameA+'+'+nameM;
				if(vpA!=nullptr && vpM !=nullptr)
				{
					float a = *vpA;
					int m = *vpM;
					float v = a+m;
					module->createFloat(name,v);
				}
				cout<<"getfloatadd"<<endl;
				auto add = builder.createFAddInst(addterm, multerm,name); //name????
				return (Value*)add;
			}
			else if(ctx->Sub())
			{
				auto name = nameA+'-'+nameM;
				if(vpA!=nullptr && vpM !=nullptr)
				{
					float a = *vpA;
					int m = *vpM;
					float v = a-m;
					module->createFloat(name,v);
				}
				auto sub = builder.createFSubInst(addterm, multerm,name); //name????
				cout<<"getfloatsub"<<endl;
				return (Value*)sub;
			}
		}
		else if(addterm->isInt() && multerm->isFloat())
		{
			auto nameA = addterm->name;
			auto nameM = multerm->name;
			auto vpA = module->getInteger(nameA);
			auto vpM = module->getFloat(nameM);
			cout<<"float!"<<endl;
			builder.createIToFInst(addterm);
			if(ctx->Add())
			{
				auto name = nameA+'+'+nameM;
				if(vpA!=nullptr && vpM !=nullptr)
				{
					int a = *vpA;
					float m = *vpM;
					float v = a+m;
					module->createFloat(name,v);
				}
				cout<<"getfloatadd"<<endl;
				auto add = builder.createFAddInst(addterm, multerm,name); //name????
			
				return (Value*)add;
			}
			else if(ctx->Sub())
			{
				auto name = nameA+'-'+nameM;
				if(vpA!=nullptr && vpM !=nullptr)
				{
					int a = *vpA;
					float m = *vpM;
					float v = a-m;
					module->createFloat(name,v);
				}
				auto sub = builder.createFSubInst(addterm, multerm,name); //name????
				cout<<"getfloatsub"<<endl;
				return (Value*)sub;
			}
		}
		else if(addterm->isFloat() && multerm->isFloat())
		{
			auto nameA = addterm->name;
			auto nameM = multerm->name;
			auto vpA = module->getFloat(nameA);
			auto vpM = module->getFloat(nameM);
			cout<<"float!"<<endl;
			//builder.createIToFInst(multerm);
			if(ctx->Add())
			{
				auto name = nameA+'+'+nameM;
				if(vpA!=nullptr && vpM !=nullptr)
				{
					float a = *vpA;
					float m = *vpM;
					float v = a+m;
					module->createFloat(name,v);
				}
				cout<<"getfloatadd"<<endl;
				auto add = builder.createFAddInst(addterm, multerm); //name????
			
				return (Value*)add;
			}
			else if(ctx->Sub())
			{
				auto name = nameA+'+'+nameM;
				if(vpA!=nullptr && vpM !=nullptr)
				{
					float a = *vpA;
					float m = *vpM;
					float v = a-m;
					module->createFloat(name,v);
				}
				auto sub = builder.createFSubInst(addterm, multerm,name); //name????
				cout<<"getfloatsub"<<endl;
				return (Value*)sub;
			}
		}
		
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
		return (visitLAndExp(ctx->lAndExp()));//waiting 
	}
	else if(Or_child !=nullptr)
	{
		Value* andterm = any_cast<Value *>(visitLAndExp(ctx->lAndExp()));
		Value* orterm = any_cast<Value *>(visitLOrExp(ctx->lOrExp()));
		//return builder.create... undefine,waiting
		auto lor = builder.createOrInst(andterm,orterm);
		return (Value *)lor;
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
		return (visitEqExp(ctx->eqExp()));//waiting 
	}
	else if(And_child !=nullptr)
	{
		Value* andterm = any_cast<Value *>(visitLAndExp(ctx->lAndExp()));
		Value* eqterm = any_cast<Value *>(visitEqExp(ctx->eqExp()));
		// return builder.create... undefine,waiting
		//return ....
		auto land = builder.createAndInst(andterm,eqterm);
		return (Value *)land;
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
		return (visitRelExp(ctx->relExp()));//waiting 
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
				return (Value *) builder.createICmpEQInst(eqterm,relterm);
			}
			else if(ctx->NonEqual())
			{
				return (Value *) builder.createICmpNEInst(eqterm,relterm);
			}
		}
		else if(eqterm->isInt() && relterm->isFloat())
		{
			builder.createIToFInst(eqterm);
			if(ctx->Equal())
			{
				return (Value *) builder.createFCmpEQInst(eqterm,relterm);
			}
			else if(ctx->NonEqual())
			{
				return (Value *) builder.createFCmpNEInst(eqterm,relterm);
			}
		}
		else if(eqterm->isFloat() && relterm->isInt())
		{
			builder.createIToFInst(relterm);
			if(ctx->Equal())
			{
				return (Value *) builder.createFCmpEQInst(eqterm,relterm);
			}
			else if(ctx->NonEqual())
			{
				return (Value *) builder.createFCmpNEInst(eqterm,relterm);
			}
		}
		else if(eqterm->isFloat() && relterm->isFloat())
		{
			//builder.createIToF(eqterm);
			if(ctx->Equal())
			{
				return (Value *) builder.createFCmpEQInst(eqterm,relterm);
			}
			else if(ctx->NonEqual())
			{
				return (Value *) builder.createFCmpNEInst(eqterm,relterm);
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
		return any_cast<Value *>(visitAddExp(ctx->addExp()));//waiting 
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
				return (Value *) builder.createICmpLTInst(relterm,addterm);
			}
			else if(ctx->GreaterThan())
			{
				return builder.createICmpGTInst(relterm,addterm);
			}
			else if(ctx->LessEqual())
			{
				return (Value *) builder.createICmpLEInst(relterm,addterm);
			}
			else if(ctx->GreaterEqual())
			{
				return (Value *) builder.createICmpGEInst(relterm,addterm);
			}	
		}
		else if(addterm->isFloat() && relterm->isInt())
		{
			builder.createIToFInst(relterm);
			if(ctx->LessThan())
			{
				return (Value *) builder.createFCmpLTInst(relterm,addterm);
			}
			else if(ctx->GreaterThan())
			{
				return (Value *) builder.createFCmpGTInst(relterm,addterm);
			}
			else if(ctx->LessEqual())
			{
				return (Value *) builder.createFCmpLEInst(relterm,addterm);
			}
			else if(ctx->GreaterEqual())
			{
				return (Value *) builder.createFCmpGEInst(relterm,addterm);
			}	
		}
		else if(addterm->isInt() && relterm->isFloat())
		{
			builder.createIToFInst(addterm);
			if(ctx->LessThan())
			{
				return (Value *) builder.createFCmpLTInst(relterm,addterm);
			}
			else if(ctx->GreaterThan())
			{
				return (Value *) builder.createFCmpGTInst(relterm,addterm);
			}
			else if(ctx->LessEqual())
			{
				return (Value *) builder.createFCmpLEInst(relterm,addterm);
			}
			else if(ctx->GreaterEqual())
			{
				return (Value *) builder.createFCmpGEInst(relterm,addterm);
			}	
		}
		else if(addterm->isFloat() && relterm->isFloat())
		{
			//builder.createIToFInst(relterm);
			if(ctx->LessThan())
			{
				return (Value *) builder.createFCmpLTInst(relterm,addterm);
			}
			else if(ctx->GreaterThan())
			{
				return (Value *) builder.createFCmpGTInst(relterm,addterm);
			}
			else if(ctx->LessEqual())
			{
				return (Value *) builder.createFCmpLEInst(relterm,addterm);
			}
			else if(ctx->GreaterEqual())
			{
				return (Value *) builder.createFCmpGEInst(relterm,addterm);
			}	
		}
		
			
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
