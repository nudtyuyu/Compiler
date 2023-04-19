#include <any>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>
#include <cstdio>
#include "SysYIRGenerator.h"
#include "debug.h"

using namespace std;

namespace sysy {

Value *SysYIRGenerator::getElementPtr(AllocaInst *base, std::vector<Value *> indices) {
	int ndim = base->getNumDims();
	assert(ndim == indicies.size());
	Value *pOffset = builder.createAllocaInst(Type::getPointerType(Type::getIntType()), {}, newTemp());	///< pointer type
	builder.createStoreInst(ConstantValue::get(0, "0"), pOffset);
	Value *offset = builder.createLoadInst(pOffset, {}, newTemp());	///< int type

	offset = builder.createPAddInst(offset, indices[0], newTemp());
	for (int i = 1; i < ndim; ++i) {
		offset = builder.createPMulInst(base->getDim(i), offset, newTemp());
		offset = builder.createAddInst(offset, indices[i], newTemp());
	}

	Value *ptr = builder.createPAddInst(base, offset, newTemp());

	return ptr;
}

// any getInitValues(vector<vector<Value*> >Values,vector<int>Dims,int dim,int flag)
// {
// 	if(dim==Dims.size())
// 	{
// 		int exist = Values.size();
// 		vector <int> InitValue;
// 		vector <float> InitValue2;
// 		for(int i=0;i<exist;i++)
// 		{
// 			if(flag==0)
// 			{
// 				auto *v = module->getInteger(Values[i]->name);
// 				InitValue.push_back(*v);
				
// 			}
// 			else if(flag==1)
// 			{
// 				auto *v = module->getFloat(Values[i]->name);
// 				InitValue2.push_back(*v);
// 			}
			
// 		}
// 		if(exist<Dims[dim-1])
// 		{
// 			int number = Dims[dim-1];
// 			for(int i=exist;i<=number;i++)
// 			{
// 				if(flag==0)
// 				{
				
// 					InitValue.push_back(0);
// 					return InitValue;
				
// 				}
// 				else if(flag==1)
// 				{
				
// 					InitValue2.push_back(0);
// 					return InitValue2;
// 				}
// 			}
// 		}
// 	}
// 	else
// 	{
// 		vector <vector<int> > InitValue;
// 		vector <vector<float> > InitValue2;
// 		int L = Dims[dim-1];
// 		for(int i=0;i<L;i++)
// 		{
// 			if(flag==0)
// 			{
// 				auto res = any_cast<vector<int> >(AssignArray(Values[i],Dims,dim+1));
// 				InitValue.push_back(res);
// 				return InitValue;
// 			}
// 			else if(flag==1)
// 			{
// 				auto res = any_cast<vector<float> >(AssignArray(Values[i],Dims,dim+1));
// 				InitValue2.push_back(res);
// 				return InitValue2;
// 			}
			
// 		}
// 	}
// 	return nullptr;
	
// }



any SysYIRGenerator::visitCompUnit(SysYParser::CompUnitContext *ctx)
{
	auto Unit = new Module;
	assert(Unit);
	module.reset(Unit);
	ConstInitListName = 0;
	InitListName = 0;
	symTable.newTable();
	arrayTable.newTable();
	for(auto decl:ctx->decl())
	{
		GlobalVal  = true;
		visitDecl(decl);
	}
	for (auto *func : ctx->funcDef())
		visitFuncDef(func);
	
	return nullptr;
}


any SysYIRGenerator::visitDecl(SysYParser::DeclContext *ctx)
{
	auto decl = (ctx->constDecl())?visitConstDecl(ctx->constDecl()):visitVarDecl(ctx->varDecl());
	GlobalVal = false;
	LocalVal = false;
	return (decl);
}

any SysYIRGenerator::visitBType(SysYParser::BTypeContext *ctx)
{
	// cout<<"visitBType"<<endl;
	return ctx->Int() ? Type::getIntType() : Type::getFloatType();
}

any SysYIRGenerator::visitConstDecl(SysYParser::ConstDeclContext *ctx)
{
	// cout<<"visitConstDecl"<<endl;
	vector <Value*> values;
	auto type = any_cast<Type*>(visitBType(ctx->bType()));
	for(auto constdef:ctx->constDef())
	{
		auto name = constdef->Identifier()->getText();
		vector <Value *> dims;
		vector <vector<int> > idata;
		vector <vector<float> > fdata;
		if(!constdef->constExp().empty())
		{
			iDims.clear();
			
			for(auto constexp:constdef->constExp())
			{
				auto dim = any_cast<Value*>(visitConstExp(constexp));
				auto * dimp = module->getInteger(dim->name);
				if(dimp!=nullptr)
				{
					//Dims.push_back(*dimp);
					if(dim->isInt())
					{
						// cout<<"dim is Int"<<endl;
						iDims.push_back(*dimp);
					}
				}
				
				dims.push_back(dim);
			
			}
			auto *alloca = builder.createAllocaInst(type,dims,name);
			if(type->isInt())
				arrayTable.insert(name,AEntry(alloca,name,iDims,true,true));
			else if(type->isFloat())
			{
				arrayTable.insert(name,AEntry(alloca,name,iDims,true,false));
			}
			
			//DimNum = dims.size();
			ConstLayer = 0;
			ConstArrayName = name;
			if(constdef->Assign())
			{
				auto *initVal = any_cast<Value *>(visitConstInitVal(constdef->constInitVal()));
				if (initVal->isInitList()) {
					// array
					auto *myA = arrayTable.query(name);
					myA->value = dynamic_cast<InitList *>(initVal);
					//auto* test = myA->value->getElement({1,1,2});
					//cout<<test->name<<endl;
					
					
						//arrayTable.insert(name, AEntry(alloca, dynamic_cast<InitList *>(initVal), dims,true,true));
					
				} else {
					// idata = any_cast<vector <vector<int> > >(getInitValue(value,Dims,1,0));
					symTable.insert(name, Entry(alloca));
					auto *store = builder.createStoreInst(initVal, alloca);
				}
					
				/*if(type->isInt())
				{
					int length = value.size();
					for(int i=0;i<length;i++)
					{
						auto v = module->getInteger(value[i]->name);
						if(v==nullptr)
						{
							// cout<<"Const Array has non-constant initial value!!!!"<<endl;
							exit(-1);
						}
						idata.push_back(*v);
						
					}
				}
				if(type->isFloat())
				{
					int length = value.size();
					for(int i=0;i<length;i++)
					{
						auto v = module->getFloat(value[i]->name);
						if(v==nullptr)
						{
							// cout<<"Const Array has non-constant initial value!!!!"<<endl;
							exit(-1);
						}
						fdata.push_back(*v);
						
					}
				}*/
				//Init List???
				
			}
			else
			{
				auto *initVal = any_cast<Value *>(GenerateZero(0,name));
				auto *myA = arrayTable.query(name);
				myA->value = dynamic_cast<InitList *>(initVal);
			}
			// // cout<<"Put constant array into arrayTable"<<endl;
			// values.push_back(alloca);
			// int btype;
			// if(type->isInt())
			// {
			// 	btype = 0;
			// 	auto data = idata;
			// 	AEntry entry(alloca,btype,0,dims,data);
			// 	arrayTable.insert(name,entry);
			// }
			// else if(type->isFloat())
			// {
			// 	btype = 1;
			// 	auto data = fdata;
			// 	AEntry entry(alloca,btype,0,dims,data);
			// 	arrayTable.insert(name,entry);
			// }
			// cout<<"insert arrayTable successfully!"<<endl;
		}
		else
		{
			auto alloca = builder.createAllocaInst(type,{},name);
			if(constdef->Assign())
			{
				auto value = any_cast<Value *>(visitConstInitVal(constdef->constInitVal()));
				symTable.insert(name, Entry(value));
				// // cout<<"The Initial Value's Name: "<<value->name<<endl;
				// auto vvi = module->getInteger(value->name);
				// auto vvf = module->getFloat(value->name);
				// if(type->isInt())
				// {
				// 	// cout<<"The Initial Value's value: "<<(*vvi)<<endl;
				// 	initV = *vvi;
				// }
				// else if(type->isFloat())
				// {
				// 	// cout<<"The Initial Value's value: "<<(*vvf)<<endl;
				// 	initV2 = *vvf;
				// }
				// auto store = builder.createStoreInst(value,alloca);
			}
			// const 声明必须有初值
			// else
			// {
			// 	if(type->isInt())
			// 	{
			// 		int a = 0;
			// 		initV = 0;
			// 		Value* value = (Value*)ConstantValue::get(a);
			// 		auto store = builder.createStoreInst(value,alloca);
			// 	}
			// 	else if(type->isFloat())
			// 	{
			// 		initV2 = 0.0;
			// 		float a = 0.0;
			// 		Value* value = (Value*)ConstantValue::get(a);
			// 		auto store = builder.createStoreInst(value,alloca);
			// 	}
			
			// }
			// values.push_back(alloca);
			// if(type->isInt())
			// {
			// 	Entry entry(alloca,0,initV);
			// 	symTable.insert(name,entry);
			// }
			// else if(type->isFloat())
			// {
			// 	Entry entry(alloca,0,initV2);
			// 	symTable.insert(name,entry);
			// }
		}
	}
	return values;
}

any SysYIRGenerator::GenerateZero(int Lay,string name)
{
	//// cout<<"GenerateConstZero"<<endl;
	auto *nowArray = arrayTable.query(name);
	int Num_Layer = nowArray->dims.size();
	if(Lay>Num_Layer-1)
		Lay = Num_Layer-1;
	int num = nowArray->dims[Lay];
	bool isInt = nowArray->isInt;
	if(Lay>=Num_Layer-1)
	{
		if(isInt)
		{
			auto *constzero = (Value*)ConstantValue::get(0,"0");
			return constzero;
		}
		else
		{
			auto *constzero = (Value*)ConstantValue::get(0.0f,"0.0");
			return constzero;
		}
		return nullptr;
	}
	else
	{
		char Name[40];
		sprintf(Name,"ConstInitName%d",ConstInitListName);
		auto *values = module->createInitList(Name);
		ConstInitListName+=1;
		
		for(int i=0;i<num;i++)
		{
			//ConstLayer++;
			auto *value = any_cast<Value *>(GenerateZero(Lay+1,name));
			//ConstLayer--;
			if(value==nullptr)
			{
				// cout<<"constinitval is nullptr!!"<<endl;
				exit(0);
			}
			if(values==nullptr)
			{
				// cout<<"initList==nullptr!"<<endl;
				exit(0);
			}
			values->addOperand(value);
			//count++;
			//// cout<<"I have addOperandZero"<<endl;
		}
		return (Value *)values;
	}
	return nullptr;
	
}

any SysYIRGenerator::visitConstInitVal(SysYParser::ConstInitValContext *ctx)
{
	// cout<<"visitConstInitVal"<<endl;
	if(ctx->constExp())
	{
		auto *constexp = any_cast<Value *>(visitConstExp(ctx->constExp()));
		return constexp;
	}
	else
	{
		char Name[40];
		sprintf(Name,"ConstInitList%d",ConstInitListName);
		auto *values = module->createInitList(Name);
		ConstInitListName+=1;
		// cout<<"constInitValSize: "<<ctx->constInitVal().size()<<endl;
		int count=0;
		for(auto constinit:ctx->constInitVal())
		{
			ConstLayer++;
			auto *value = any_cast<Value *>(visitConstInitVal(constinit));
			ConstLayer--;
			if(value==nullptr)
			{
				// cout<<"constinitval is nullptr!!"<<endl;
				exit(0);
			}
			if(values==nullptr)
			{
				// cout<<"initList==nullptr!"<<endl;
				exit(0);
			}
			values->addOperand(value);
			count++;
			// cout<<"I have addOperand"<<endl;
		}
		auto *nowArray = arrayTable.query(ConstArrayName);
		int num = nowArray->dims[ConstLayer];
		bool isInt = nowArray->isInt;
		for(int i=count;i<=num;i++)
		{
			// cout<<"GenerateConstZero"<<endl;
			auto *gZero = any_cast<Value*>(GenerateZero(ConstLayer+1,ConstArrayName));
			
			values->addOperand(gZero);
			/*if(isInt)
			{
				int z = 0;
				Value* zero = (Value*)ConstantValue::get(z);
				values->addOperand(zero);
			}
			else
			{
				float z = 0.0;
				Value* zero = (Value*)ConstantValue::get(z);
				values->addOperand(zero);
			}*/
		}
		return (Value *)values;
	}
	return nullptr;
}


any SysYIRGenerator::visitVarDecl(SysYParser::VarDeclContext *ctx)
{
	// cout<<"visitVarDecl"<<endl;
	vector <Value*> values;
	auto *type = any_cast<Type *>(visitBType(ctx->bType()));
	for(auto *vardef:ctx->varDef())
	{
		auto name = vardef->Identifier()->getText();
		vector <Value *> dims;
		if(!vardef->constExp().empty())
		{
			for(auto constexp:vardef->constExp())
			{
				dims.push_back(any_cast<Value*>(visitConstExp(constexp)));
			}
			auto *alloca = builder.createAllocaInst(type,dims,name);
			if(type->isInt())
				arrayTable.insert(name,AEntry(alloca,name,dims,false,true));
			else if(type->isFloat())
			{
				arrayTable.insert(name,AEntry(alloca,name,dims,false,false));
			}
			if(vardef->Assign())
			{
				Layer = 0;
				ArrayName = name;
				auto *initVal = any_cast<Value *>(visitInitVal(vardef->initVal()));
				assert(initVal->getType() == Type::getInitListType());
				// 这里no要调用AssignArray
				auto *myA = arrayTable.query(name);
				myA->value = dynamic_cast<InitList *>(initVal);
				/*if(type->isInt())
					arrayTable.insert(name, AEntry(alloca, dynamic_cast<InitList *>(initVal), dims ,false,true));
				else if(type->isFloat())
					arrayTable.insert(name, AEntry(alloca, dynamic_cast<InitList *>(initVal), dims ,false,false));*/
			}
			else
			{
				auto *initVal = any_cast<Value *>(GenerateZero(0,name));
				auto *myA = arrayTable.query(name);
				myA->value = dynamic_cast<InitList *>(initVal);
			}
			values.push_back(alloca);
		}
		else
		{
			auto alloca = builder.createAllocaInst(type,{},name);
			if(vardef->Assign())
			{
				auto *initVal = any_cast<Value *>(visitInitVal(vardef->initVal()));
				// cout<<"The Initial Value's Name: "<<initVal->name<<endl;
				// cout<<"getVardefAssignInitVal"<<endl;
				auto *store = builder.createStoreInst(initVal, alloca);
				// cout<<"createStoreInst"<<endl;
			}
			else
			{
				if(type->isInt())
				{
					auto *store = builder.createStoreInst(ConstantValue::get(0, "0"), alloca);
				}
				else if(type->isFloat())
				{
					auto *store = builder.createStoreInst(ConstantValue::get(0.0f, "0.0"), alloca);
				}
			
			}
			values.push_back(alloca);
			symTable.insert(name, Entry(alloca));
		}
		
	}
	return values;
}

any SysYIRGenerator::visitInitVal(SysYParser::InitValContext *ctx)
{
	// cout<<"visitInitVal"<<endl;
	if(ctx->exp())
	{
		auto exp = any_cast<Value *>(visitExp(ctx->exp()));
		return exp;
	}
	else
	{
		char Name[40];
		sprintf(Name,"InitList%d",InitListName);
		auto *initList = module->createInitList(Name);
		InitListName++;
		int count=0;
		for(auto initv:ctx->initVal())
		{
			Layer++;
			auto *initval = any_cast<Value*>(visitInitVal(initv));
			Layer--;
			if(initval==nullptr)
			{
				// cout<<"initval is nullptr!!"<<endl;
				exit(0);
			}
			initList->addOperand(initval);
			count++;
		}
		auto *nowArray = arrayTable.query(ArrayName);
		int num = nowArray->dims[Layer];
		bool isInt = nowArray->isInt;
		for(int i=count;i<=num;i++)
		{
			// cout<<"GenerateConstZero"<<endl;
			auto *gZero = any_cast<Value*>(GenerateZero(Layer+1,ArrayName));
			
			initList->addOperand(gZero);
			/*int inner
			for(int j=0;j<)
			if(isInt)
			{
				int z = 0;
				Value* zero = (Value*)ConstantValue::get(z);
				initList->addOperand(zero);
			}
			else
			{
				float z = 0.0;
				Value* zero = (Value*)ConstantValue::get(z);
				initList->addOperand(zero);
			}*/
		}
		return (Value *)initList;
	}
	return nullptr;
}

any SysYIRGenerator::visitFuncDef(SysYParser::FuncDefContext *ctx) {
	// cout << debug::change_color("creating function " + ctx->Identifier()->getText(), debug::yellow) << endl;
	Type *retType = any_cast<Type *>(visitFuncType(ctx->funcType()));
	vector<Type *> paramTypes;
	vector<string> paramNames;
	if(ctx->funcFParams())
	{
		for (auto *fparam : ctx->funcFParams()->funcFParam()) {
			paramTypes.emplace_back(any_cast<Type *>(visitBType(fparam->bType())));
			paramNames.emplace_back(fparam->Identifier()->getText());
		}
	}
	
	Type *funcType = Type::getFunctionType(retType, paramTypes);
	auto *function = module->createFunction(ctx->Identifier()->getText(), funcType);
	auto *entry = function->getEntryBlock();
	builder.setPosition(entry, entry->end());
	symTable.newTable();
	for (int i = 0; i < paramTypes.size(); ++i) {
		auto *arg = entry->createArgument(paramTypes[i], paramNames[i]);
		auto *ptr = builder.createAllocaInst(paramTypes[i], {}, newTemp());
		builder.createStoreInst(arg, ptr);
		symTable.insert(paramNames[i], Entry(ptr));
	}
	// symTable.view();
	visitBlock(ctx->block());
	symTable.destroyTop();
	// cout << debug::change_color("function " + ctx->Identifier()->getText() + " created", debug::yellow) << endl;

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
	return (Value *)nullptr;
}

any SysYIRGenerator::visitBlockItem(SysYParser::BlockItemContext *ctx) {
	if (ctx->decl() != nullptr)
		visitDecl(ctx->decl());
	else visitStmt(ctx->stmt());
	return (Value *)nullptr;
}

any SysYIRGenerator::visitStmt(SysYParser::StmtContext *ctx) {
	Value *ret = nullptr;
	if (ctx->lVal() != nullptr) {
		auto lval = any_cast<pair<Value *, Value *>>(visitLVal(ctx->lVal()));
		assert(lval.first != nullptr);				///< 赋值号左侧必须是变量
		Value *exp = any_cast<Value *>(visitExp(ctx->exp()));
		ret = builder.createStoreInst(exp, lval.first);
	} else if (ctx->Return() != nullptr) {
		Value *value = nullptr;
		if (ctx->exp() != nullptr)
			value = any_cast<Value *>(visitExp(ctx->exp()));
		ret = builder.createReturnInst(value);
	} else if (ctx->If() != nullptr) {
		auto *block = builder.getBasicBlock();   						///< 当前所在基本块
		auto *func = block->getParent();         						///< 当前所在函数
		auto *thenBlock = func->addBasicBlock(newBlockName()); 			///< then分支基本块
		auto *elseBlock = func->addBasicBlock(newBlockName());   		///< else分支基本块（可能没有，但构造指令时不能为nullptr）
		auto *exitBlock = func->addBasicBlock(newBlockName()); 			///< if-then-else结束后的部分
		auto *cond = any_cast<Value *>(visitCond(ctx->cond()));

		ret = builder.createCondBrInst(cond, thenBlock, elseBlock, {}, {});

		// translate thenblock
		block->getSuccessors().push_back(thenBlock);
		thenBlock->getPredecessors().push_back(block);
		builder.setPosition(thenBlock, thenBlock->end());
		visitStmt(ctx->stmt()[0]);
		builder.createUncondBrInst(exitBlock, {});
		builder.setPosition(block, block->end());

		// translate elseblock
		block->getSuccessors().push_back(elseBlock);
		elseBlock->getPredecessors().push_back(block);
		builder.setPosition(elseBlock, elseBlock->end());
		if (ctx->Else() != nullptr) {
			visitStmt(ctx->stmt()[1]);
		}
		builder.createUncondBrInst(exitBlock, {});
		builder.setPosition(block, block->end());

		// prepare to translate exitblock
		block->getSuccessors().push_back(exitBlock);
		exitBlock->getPredecessors().push_back(block);
		builder.setPosition(exitBlock, exitBlock->end());
	} else if (ctx->While() != nullptr) {
		auto *block = builder.getBasicBlock();    ///< 当前所在基本块
		auto *func = block->getParent();          ///< 当前所在函数
		auto *entryBlock = func->addBasicBlock(newBlockName()); ///< 循环入口，即循环条件计算和判断
		auto *innerBlock = func->addBasicBlock(newBlockName()); ///< while循环体块
		auto *exitBlock = func->addBasicBlock(newBlockName());  ///< while循环结束后的部分

		block->getSuccessors().push_back(entryBlock);
		entryBlock->getPredecessors().push_back(block);
		entryBlock->getSuccessors().push_back(innerBlock);
		innerBlock->getPredecessors().push_back(entryBlock);
		entryBlock->getSuccessors().push_back(exitBlock);
		exitBlock->getPredecessors().push_back(entryBlock);
		innerBlock->getSuccessors().push_back(entryBlock);
		entryBlock->getPredecessors().push_back(innerBlock);

		// translate cond/entry block
		builder.setPosition(entryBlock, entryBlock->end());
		auto *cond = any_cast<Value *>(visitCond(ctx->cond()));
		ret = builder.createCondBrInst(cond, innerBlock, exitBlock, {}, {});

		// translate inner block
		builder.setPosition(innerBlock, innerBlock->end());
		loopEntry.push_back(entryBlock);
		loopExit.push_back(exitBlock);
		visitStmt(ctx->stmt()[0]);
		builder.createUncondBrInst(entryBlock, {});
		loopExit.pop_back();
		loopEntry.pop_back();

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
	// cout<<"visitExp"<<endl;
	return any_cast<Value *>(visitAddExp(ctx->addExp()));
}

any SysYIRGenerator::visitCond(SysYParser::CondContext* ctx)
{
	// cout<<"visitCond"<<endl; 
	return any_cast<Value *>(visitLOrExp(ctx->lOrExp()));
}

any SysYIRGenerator::visitLVal(SysYParser::LValContext *ctx) {
	auto name = ctx->Identifier()->getText();
	vector<Value *> exps;
	bool constflag = true;
	// cout << "visitLVal: " << ctx->getText() << endl;

	for (auto *exp : ctx->exp()) {
		auto *e = any_cast<Value *>(visitExp(exp));
		auto *pInteger = module->getInteger(e->getName());
		if(pInteger!=nullptr)
		{
			/*char num[50]={0};
			snprintf(num,sizeof(num),"%d",*pInteger);
			name = name + "[" + num + "]";*/
			exps.push_back(ConstantValue::get(*pInteger, e->getName()));
		} else {
			//name = name + "[" + e->name + "]";
			constflag = false;
			exps.push_back(e);
		}   
	}
	// cout<<"LVal Name: "<<ctx->getText()<<endl;

	if (!ctx->exp().empty()) {
		// array
		auto *entry = arrayTable.query(ctx->Identifier()->getText());
		int ndim = entry->dims.size();
		Value *pOffset = builder.createAllocaInst(Type::getPointerType(Type::getIntType()), {}, newTemp());
		builder.createStoreInst(ConstantValue::get(0, "0"), pOffset);
		Value *offset = builder.createLoadInst(pOffset, {}, newTemp());

		if (ndim != 0) {
			offset = builder.createPAddInst(offset, exps[0], newTemp());
		}
		for (int i = 1; i < ndim; ++i) {
			stringstream ss;
			ss << entry->dims[i];
			offset = builder.createPMulInst(ConstantValue::get(entry->dims[i], ss.str()), offset, newTemp());
			offset = builder.createPAddInst(offset, exps[i], newTemp());
		}

		Value *ptr = builder.createPAddInst(entry->base, offset, newTemp());
		if (constflag && entry->isConst) {
			vector<int> indices;
			for (auto *index : exps) {
				indices.push_back(dynamic_cast<ConstantValue *>(index)->getInt());
			}
			auto *value = entry->value->getElement(indices);
			return make_pair((Value *)nullptr, value);
		} else {
			return make_pair(ptr, (Value *)nullptr);
		}
	} else {
		auto *entry = symTable.query(name);
		if (entry->isConst) {
			return make_pair((Value *)nullptr, entry->value);
		} else {
			return make_pair(entry->value, (Value *)nullptr);
		}
	}
}

any SysYIRGenerator::visitPrimaryExp(SysYParser::PrimaryExpContext *ctx)
{
	// cout<<"visitPrimaryExp"<<endl;
	if(ctx->LeftBracket())
	{
		return any_cast<Value *>(visitExp(ctx->exp()));
	}
	else if(ctx->lVal())
	{
		// cout<<"get LVal"<<endl;
		auto lval = any_cast<pair<Value*,Value*>>(visitLVal(ctx->lVal()));//waiting
		auto *ptr = lval.first;
		auto *value = lval.second;
		// cout<<"get addr"<<endl;
		// symTable.view();
		// arrayTable.view();
		auto name = (ptr != nullptr ? ptr->name : value->name);
		if(ptr!=nullptr)
		{
			// var
			auto *baseType = dynamic_cast<PointerType *>(ptr->getType())->getBaseType();
			// cout<<"ValType: "<<baseType<<endl;
			auto *loadValue = builder.createLoadInst(ptr,{},newTemp());
			// cout<<"load value"<<endl;
			return (Value *)loadValue;
		}
		else
		{
			// const
			return (Value *)value;
		}
	}
	else if(ctx->number())
	{
		auto *num = any_cast<Value *>(visitNumber(ctx->number()));//waiting
		// cout<<"num: "<<num->getName()<<endl;
		// cout<<"getnum"<<endl;
		return num;
	}
	return (Value *)nullptr;
	
}

any SysYIRGenerator::visitNumber(SysYParser::NumberContext *ctx)
{
	// cout<<"visitNumber: "<<ctx->getText()<<endl;
	//return ctx->IntConst() ? Type::getInt() : Type::getFloat();
	if(ctx->IntConst())
	{
		//return module->createGlobalValue(ctx->IntConst()->getText(),Type::getIntType());	
		//return ctx->IntConst();
		//int value = atoi(ctx->IntConst()->getText().c_str());
		int value = strtol(ctx->IntConst()->getText().c_str(),NULL,0);
		module->createInteger(ctx->IntConst()->getText(),value);
		const std::string name = ctx->IntConst()->getText();
		auto cv = module->getInteger(name);
		// cout<<"mynum: "<<(*cv)<<endl;
		//auto cv = (Value*)ConstantValue::get(value,name);
		//// cout<<"mynum: "<<cv->name<<endl;
		return (Value*)ConstantValue::get(value,ctx->IntConst()->getText());
	}
	else if(ctx->FloatConst())
	{
		//return module->createGlobalValue(ctx->FloatConst()->getText(),Type::getFloatType());
		//return ctx->FloatConst();
		float value = strtof(ctx->FloatConst()->getText().c_str(),NULL);
		if(ctx->FloatConst()->getText().substr(0,2)=="0x" || ctx->FloatConst()->getText().substr(0,2)=="0X")
			value = 0.0;
		module->createFloat(ctx->FloatConst()->getText(),value);
		
		//auto name = ctx->FloatConst()->getText();
		//auto cv = (Value*)ConstantValue::get(value,name);
		//// cout<<"mynum: "<<cv->name<<endl;
		cout<<"vlaue: "<<value<<endl;
		return (Value*)ConstantValue::get(value,ctx->FloatConst()->getText());		
	}
	return (Value*)nullptr;
}

any SysYIRGenerator::visitUnaryExp(SysYParser::UnaryExpContext *ctx)
{
	// cout<<"visitUnaryExp"<<endl;
	if(ctx->primaryExp() != nullptr)
	{
		auto *pri = any_cast<Value *>(visitPrimaryExp(ctx->primaryExp()));
		// cout<<"got visitPrimaryExp"<<endl;
		return pri;
	}
	else if(ctx->Identifier() != nullptr)
	{
		// cout<<"function calling!"<<endl;
		auto *callee = module->getFunction(ctx->Identifier()->getText());
		// cout << callee->name << endl;
		vector <Value*> args;
		auto *entry = callee->getEntryBlock();
		// cout<<"entry yes!"<<endl;
		auto *parent = entry->getParent();
		//auto parent = BasicBlock::getParent();
		if (ctx->funcRParams() != nullptr)
		{
			for(auto param:ctx->funcRParams()->exp())
			{
				// cout<<"param"<<endl;
				args.push_back(any_cast<Value *>(visitExp(param)));
			}
		}
		
		return (Value*) builder.createCallInst(callee,args,entry, newTemp());
	} 
	else if(ctx->unaryOp() != nullptr)
	{
		Value* term = any_cast<Value *>(visitUnaryExp(ctx->unaryExp()));
		if(term->isInt())
		{
			if(ctx->unaryOp()->Add())
			{
			
				//create
				auto name = '+'+term->name;
				auto *vp = module->getInteger(term->name);
				if(vp!=nullptr)
				{
					int v = *vp;
					v = v;
					module->createInteger(name,v);
				}
				auto *unary = builder.createPosInst(term,newTemp());
				// cout<<"PositiveUnary: wait for edit!"<<endl;
				return (Value*)unary;
			}
			else if(ctx->unaryOp()->Sub())
			{
				// Type! Int or Float!
				auto name = '-'+term->name;
				auto *vp = module->getInteger(term->name);
				if(vp!=nullptr)
				{
					int v = *vp;
					v = -v;
					module->createInteger(name,v);
				}
				auto *unary = builder.createNegInst(term, newTemp());
				// cout<<"getunarysub"<<endl;
				return (Value*)unary;
			}
			else if(ctx->unaryOp()->Not())
			{
				// Type! Int or Float!
				auto name = '!'+term->name;
				auto *vp = module->getInteger(term->name);
				if(vp!=nullptr)
				{
					int v = *vp;
					v = !v;
					module->createInteger(name,v);
				}
				auto *unary = builder.createNotInst(term,newTemp());
				// cout<<"getunarynot"<<endl;
				return (Value*)unary;
			}
		}
		if(term->isFloat())
		{
			if(ctx->unaryOp()->Add())
			{
			
				//create
				auto name = '+'+term->name;
				auto *vp = module->getFloat(term->name);
				if(vp!=nullptr)
				{
					float v = *vp;
					v = v;
					module->createFloat(name,v);
				}
				auto *unary = builder.createFPosInst(term,newTemp());
				// cout<<"PositiveUnary: wait for edit!"<<endl;
				return (Value*)unary;
			}
			else if(ctx->unaryOp()->Sub())
			{
				// Type! Int or Float!
				auto name = '-'+term->name;
				auto *vp = module->getFloat(term->name);
				if(vp!=nullptr)
				{
					float v = *vp;
					v = -v;
					module->createFloat(name,v);
				}
				auto *unary = builder.createFNegInst(term,newTemp());
				// cout<<"getunarysub"<<endl;
				return (Value*)unary;
			}
			else if(ctx->unaryOp()->Not()) //question???
			{
				// Type! Int or Float!
				auto *unary = builder.createNotInst(term, newTemp());
				// cout<<"getunarynot"<<endl;
				return (Value*)unary;
			}
		}
	}
	return (Value *)nullptr;
}

any SysYIRGenerator::visitMulExp(SysYParser::MulExpContext *ctx)
{
	auto *child = ctx->children[0];
	auto *Mul_child = dynamic_cast<SysYParser::MulExpContext *>(child);
	auto *Unary_child = dynamic_cast<SysYParser::UnaryExpContext *>(child);
	//auto mul;
	// cout<<"visitMulExp"<<endl;
	if(Unary_child != nullptr)
	{
		auto *unary = any_cast<Value *>(visitUnaryExp(ctx->unaryExp()));
		// cout<<"getmulunary"<<endl;
		return unary;
	}
	else if(Mul_child !=nullptr)
	{
		Value* unarrayTableerm = any_cast<Value *>(visitUnaryExp(ctx->unaryExp()));
		Value* multerm = any_cast<Value *>(visitMulExp(ctx->mulExp()));
		// Type tansfer
		if(unarrayTableerm->isInt() && multerm->isInt())
		{
			auto nameU = unarrayTableerm->name;
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
				auto mul = builder.createMulInst(multerm,unarrayTableerm,newTemp());
				// cout<<"getmul"<<endl;
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
				auto div = builder.createDivInst(multerm,unarrayTableerm,newTemp());
				// cout<<"getdiv"<<endl;
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
				auto mod = builder.createRemInst(multerm,unarrayTableerm,newTemp());
				// cout<<"getmod"<<endl;
				return (Value*)mod;
			}
		}
		else if(unarrayTableerm->isFloat() && multerm->isInt())
		{
			auto nameU = unarrayTableerm->name;
			auto nameM = multerm->name;
			auto vpU = module->getFloat(nameU);
			auto vpM = module->getInteger(nameM);
			// cout<<"floatunary!"<<endl;
			multerm = builder.createIToFInst(multerm, newTemp());
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
				auto mul = builder.createFMulInst(multerm,unarrayTableerm,newTemp());
				// cout<<"getmul"<<endl;
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
				auto div = builder.createFDivInst(multerm,unarrayTableerm,newTemp());
				// cout<<"getdiv"<<endl;
				return (Value*)div;
			}
			else if(ctx->Mod())
			{
				// cout<<"invalid operands of types ‘int’ and ‘float’ to binary ‘operator%’"<<endl;
			}
		}
		else if(unarrayTableerm->isInt() && multerm->isFloat())
		{	
			auto nameU = unarrayTableerm->name;
			auto nameM = multerm->name;
			auto vpU = module->getInteger(nameU);
			auto vpM = module->getFloat(nameM);
			// cout<<"floatmul!"<<endl;
			unarrayTableerm = builder.createIToFInst(unarrayTableerm, newTemp());
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
				auto mul = builder.createFMulInst(multerm,unarrayTableerm,newTemp());
				// cout<<"getmul"<<endl;
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
				auto div = builder.createFDivInst(multerm,unarrayTableerm,newTemp());
				// cout<<"getdiv"<<endl;
				return (Value*)div;
			}
			else if(ctx->Mod())
			{
				// cout<<"invalid operands of types ‘int’ and ‘float’ to binary ‘operator%’"<<endl;
			}
		}
		else if(unarrayTableerm->isFloat() && multerm->isFloat())
		{
			auto nameU = unarrayTableerm->name;
			auto nameM = multerm->name;
			auto vpU = module->getFloat(nameU);
			auto vpM = module->getFloat(nameM);
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
				auto mul = builder.createFMulInst(multerm,unarrayTableerm,newTemp());
				// cout<<"getmul"<<endl;
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
				auto div = builder.createFDivInst(multerm,unarrayTableerm,newTemp());
				// cout<<"getdiv"<<endl;
				return (Value*)div;
			}
			else if(ctx->Mod())
			{
				// cout<<"invalid operands of types ‘int’ and ‘float’ to binary ‘operator%’"<<endl;
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
	// cout<<"visitAddExp"<<endl;
	if(Mul_child != nullptr)
	{
		auto* mul = any_cast<Value *>(visitMulExp(ctx->mulExp()));
		// cout<<"getaddmul"<<endl;
		return mul;
	}
	else if(Add_child != nullptr)
	{
		//type transfer
		Value* addterm = any_cast<Value *>(visitAddExp(ctx->addExp()));
		// cout<<"visitadd:getaddterm"<<endl;
		Value* multerm = any_cast<Value *>(visitMulExp(ctx->mulExp())); //wait
		// cout<<"visitadd:getmulterm"<<endl;
		if(addterm->isInt() && multerm->isInt())
		{
			auto nameA = addterm->name;
			auto nameM = multerm->name;
			auto vpA = module->getInteger(nameA);
			auto vpM = module->getInteger(nameM);
			if(ctx->Add())
			{
				auto name = nameA+'+'+nameM;
				// cout<<"add code : "<<name<<endl;
				if(vpA!=nullptr && vpM !=nullptr)
				{
					// cout<<"vpA and vpM is ok!"<<endl;
					int a = *vpA;
					int m = *vpM;
					int v = a+m;
					module->createInteger(name,v);
				}
				// cout<<"getadd"<<endl;
				auto add = builder.createAddInst(addterm, multerm,newTemp());
			
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
				auto sub = builder.createSubInst(addterm, multerm, newTemp());
				// cout<<"getsub"<<endl;
				return (Value*)sub;
			}
		}
		else if(addterm->isFloat() && multerm->isInt())
		{
			auto nameA = addterm->name;
			auto nameM = multerm->name;
			auto vpA = module->getFloat(nameA);
			auto vpM = module->getInteger(nameM);
			// cout<<"float!"<<endl;
			multerm = builder.createIToFInst(multerm, newTemp());
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
				// cout<<"getfloatadd"<<endl;
				auto add = builder.createFAddInst(addterm, multerm,newTemp());
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
				auto sub = builder.createFSubInst(addterm, multerm,newTemp());
				// cout<<"getfloatsub"<<endl;
				return (Value*)sub;
			}
		}
		else if(addterm->isInt() && multerm->isFloat())
		{
			auto nameA = addterm->name;
			auto nameM = multerm->name;
			auto vpA = module->getInteger(nameA);
			auto vpM = module->getFloat(nameM);
			// cout<<"float!"<<endl;
			addterm = builder.createIToFInst(addterm, newTemp());
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
				// cout<<"getfloatadd"<<endl;
				auto add = builder.createFAddInst(addterm, multerm,newTemp());
			
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
				auto sub = builder.createFSubInst(addterm, multerm,newTemp());
				return (Value*)sub;
			}
		}
		else if(addterm->isFloat() && multerm->isFloat())
		{
			auto nameA = addterm->name;
			auto nameM = multerm->name;
			auto vpA = module->getFloat(nameA);
			auto vpM = module->getFloat(nameM);
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
				auto add = builder.createFAddInst(addterm, multerm, newTemp());
			
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
				auto sub = builder.createFSubInst(addterm, multerm,newTemp());
				// cout<<"getfloatsub"<<endl;
				return (Value*)sub;
			}
		}
		
	}
	return (Value *)nullptr;
}

any SysYIRGenerator::visitLOrExp(SysYParser::LOrExpContext* ctx)
{
	if(ctx->lOrExp() !=nullptr)
	{
		Value* andterm = any_cast<Value *>(visitLAndExp(ctx->lAndExp()));
		Value* orterm = any_cast<Value *>(visitLOrExp(ctx->lOrExp()));
		//return builder.create... undefine,waiting
		auto *lor = builder.createOrInst(andterm,orterm, newTemp());
		return (Value *)lor;
	}
	else if(ctx->lAndExp() != nullptr)
	{
		return any_cast<Value *>(visitLAndExp(ctx->lAndExp()));//waiting 
	}

	return (Value *)nullptr;
}

any SysYIRGenerator::visitLAndExp(SysYParser::LAndExpContext* ctx)
{
	auto *child = ctx->children[0];
	auto *And_child = dynamic_cast<SysYParser::LAndExpContext *>(child);
	auto *Eq_child = dynamic_cast<SysYParser::EqExpContext *>(child);
	if(Eq_child != nullptr)
	{
		return any_cast<Value *>(visitEqExp(ctx->eqExp()));//waiting 
	}
	else if(And_child !=nullptr)
	{
		Value* andterm = any_cast<Value *>(visitLAndExp(ctx->lAndExp()));
		Value* eqterm = any_cast<Value *>(visitEqExp(ctx->eqExp()));
		// return builder.create... undefine,waiting
		//return ....
		auto land = builder.createAndInst(andterm,eqterm, newTemp());
		return (Value *)land;
	}
	return (Value *)nullptr;
}

any SysYIRGenerator::visitEqExp(SysYParser::EqExpContext* ctx)
{
	auto *child = ctx->children[0];
	auto *Eq_child = dynamic_cast<SysYParser::EqExpContext *>(child);
	auto *Rel_child = dynamic_cast<SysYParser::RelExpContext *>(child);
	if(Rel_child != nullptr)
	{
		return any_cast<Value *>(visitRelExp(ctx->relExp())); 
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
				return (Value *) builder.createICmpEQInst(eqterm,relterm, newTemp());
			}
			else if(ctx->NonEqual())
			{
				return (Value *) builder.createICmpNEInst(eqterm,relterm, newTemp());
			}
		}
		else if(eqterm->isInt() && relterm->isFloat())
		{
			eqterm = builder.createIToFInst(eqterm, newTemp());
			if(ctx->Equal())
			{
				return (Value *) builder.createFCmpEQInst(eqterm,relterm, newTemp());
			}
			else if(ctx->NonEqual())
			{
				return (Value *) builder.createFCmpNEInst(eqterm,relterm, newTemp());
			}
		}
		else if(eqterm->isFloat() && relterm->isInt())
		{
			relterm = builder.createIToFInst(relterm, newTemp());
			if(ctx->Equal())
			{
				return (Value *) builder.createFCmpEQInst(eqterm,relterm, newTemp());
			}
			else if(ctx->NonEqual())
			{
				return (Value *) builder.createFCmpNEInst(eqterm,relterm, newTemp());
			}
		}
		else if(eqterm->isFloat() && relterm->isFloat())
		{
			if(ctx->Equal())
			{
				return (Value *) builder.createFCmpEQInst(eqterm,relterm, newTemp());
			}
			else if(ctx->NonEqual())
			{
				return (Value *) builder.createFCmpNEInst(eqterm,relterm, newTemp());
			}
		}
	}
	return (Value *)nullptr;
}


any SysYIRGenerator::visitRelExp(SysYParser::RelExpContext* ctx)
{
	auto *child = ctx->children[0];
	auto *Add_child = dynamic_cast<SysYParser::AddExpContext *>(child);
	auto *Rel_child = dynamic_cast<SysYParser::RelExpContext *>(child);
	if(Add_child != nullptr)
	{
		return any_cast<Value *>(visitAddExp(ctx->addExp()));
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
				return (Value *) builder.createICmpLTInst(relterm,addterm, newTemp());
			}
			else if(ctx->GreaterThan())
			{
				return (Value *)builder.createICmpGTInst(relterm,addterm, newTemp());
			}
			else if(ctx->LessEqual())
			{
				return (Value *) builder.createICmpLEInst(relterm,addterm, newTemp());
			}
			else if(ctx->GreaterEqual())
			{
				return (Value *) builder.createICmpGEInst(relterm,addterm, newTemp());
			}	
		}
		else if(addterm->isFloat() && relterm->isInt())
		{
			relterm = builder.createIToFInst(relterm, newTemp());
			if(ctx->LessThan())
			{
				return (Value *) builder.createFCmpLTInst(relterm,addterm, newTemp());
			}
			else if(ctx->GreaterThan())
			{
				return (Value *) builder.createFCmpGTInst(relterm,addterm, newTemp());
			}
			else if(ctx->LessEqual())
			{
				return (Value *) builder.createFCmpLEInst(relterm,addterm, newTemp());
			}
			else if(ctx->GreaterEqual())
			{
				return (Value *) builder.createFCmpGEInst(relterm,addterm, newTemp());
			}	
		}
		else if(addterm->isInt() && relterm->isFloat())
		{
			addterm = builder.createIToFInst(addterm, newTemp());
			if(ctx->LessThan())
			{
				return (Value *) builder.createFCmpLTInst(relterm,addterm, newTemp());
			}
			else if(ctx->GreaterThan())
			{
				return (Value *) builder.createFCmpGTInst(relterm,addterm, newTemp());
			}
			else if(ctx->LessEqual())
			{
				return (Value *) builder.createFCmpLEInst(relterm,addterm, newTemp());
			}
			else if(ctx->GreaterEqual())
			{
				return (Value *) builder.createFCmpGEInst(relterm,addterm, newTemp());
			}	
		}
		else if(addterm->isFloat() && relterm->isFloat())
		{
			if(ctx->LessThan())
			{
				return (Value *) builder.createFCmpLTInst(relterm,addterm, newTemp());
			}
			else if(ctx->GreaterThan())
			{
				return (Value *) builder.createFCmpGTInst(relterm,addterm, newTemp());
			}
			else if(ctx->LessEqual())
			{
				return (Value *) builder.createFCmpLEInst(relterm,addterm, newTemp());
			}
			else if(ctx->GreaterEqual())
			{
				return (Value *) builder.createFCmpGEInst(relterm,addterm, newTemp());
			}	
		}
		
			
	}
	return (Value *)nullptr;
}

any SysYIRGenerator::visitConstExp(SysYParser::ConstExpContext *ctx)
{
	// cout<<"visitConstExp"<<endl;
	return visitAddExp(ctx->addExp());
}

} // namespace sysy
