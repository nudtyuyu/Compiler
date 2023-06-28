#include <any>
#include <cstddef>
#include <iostream>
#include <memory>
#include <pthread.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <cstdio>
#include "SysYIRGenerator.h"
#include "debug.h"

using namespace std;

namespace sysy {

// Value *SysYIRGenerator::getElementPtr(Value *base, const std::vector<Value *> &dims, const std::vector<Value *> &indices) {
// 	size_t ndim = base->getNumDims();
// 	assert(ndim == indicies.size());
// 	Value *pOffset = builder.createAllocaInst(base->getType(), {}, newTemp());
// 	builder.createStoreInst(ConstantValue::get(0, "0"), pOffset);
// 	Value *offset = base;	///< int type

// 	offset = builder.createPAddInst(offset, indices[0], newTemp());
// 	for (size_t i = 1; i < ndim; ++i) {
// 		offset = builder.createPMulInst(base->getDim(i), offset, newTemp());
// 		offset = builder.createAddInst(offset, indices[i], newTemp());
// 	}

// 	Value *ptr = builder.createPAddInst(base, offset, newTemp());

// 	return ptr;
// }

any SysYIRGenerator::visitCompUnit(SysYParser::CompUnitContext *ctx)
{
	// cerr << "visit compunit" << endl;
	auto *unit = new Module;
	assert(unit);
	module.reset(unit);
	builder.setModule(unit);
	// ConstInitListName = 0;
	InitListName = 0;
	globalScope = false;

	for(auto decl:ctx->decl())
	{
		globalScope  = true;
		visitDecl(decl);
		globalScope = false;
	}
	for (auto *func : ctx->funcDef()) {
		visitFuncDef(func);
	}
	module->setSymTable(builder.getSymTable());
	return nullptr;
}


any SysYIRGenerator::visitDecl(SysYParser::DeclContext *ctx)
{
	// cerr << "visit decl" << endl;
	auto decl = (ctx->constDecl())?visitConstDecl(ctx->constDecl()):visitVarDecl(ctx->varDecl());
	return (decl);
}

any SysYIRGenerator::visitBType(SysYParser::BTypeContext *ctx)
{
	return ctx->Int() ? Type::getIntType() : Type::getFloatType();
}

any SysYIRGenerator::visitConstDecl(SysYParser::ConstDeclContext *ctx)
{
	// cerr << "visit const decl" << endl;
	vector <Value*> values;
	auto *type = any_cast<Type*>(visitBType(ctx->bType()));
	for(auto *constdef:ctx->constDef())
	{
		auto name = constdef->Identifier()->getText();
		vector <Value *> dims;
		vector <int> iDims;
		if(!constdef->constExp().empty())
		{	// array
			for(auto *constexp:constdef->constExp())
			{
				auto *dim = any_cast<Value*>(visitConstExp(constexp));
				assert(dim->isInt());
				dims.push_back(dim);
				iDims.push_back(dynamic_cast<ConstantValue *>(dim)->getInt());
			}
			nowElement = 0;
			auto *initValue = std::any_cast<Value *>(visitConstInitVal(constdef->constInitVal(), iDims, 0));
			if(globalScope) {
				auto *globalValue = module->createGlobalValue(name, Type::getPointerType(type), dims, initValue, true);
				builder.getSymTable()->insert(name, globalValue, dims, initValue, true, SymTableEntry::GLOBAL);
			} else {
				auto *allocaInst = builder.createAllocaInst(type, dims, name);
				builder.getSymTable()->insert(name, allocaInst, dims, initValue, true, SymTableEntry::LOCAL);
				// 待补充：store数组
				storeLocalArray(allocaInst, initValue);
			}
		}
		else
		{
			auto *initValue = any_cast<Value *>(visitConstInitVal(constdef->constInitVal(), iDims, 0));
			
			//builder.getSymTable()->insert(name, initValue, {}, initValue, true);
			if(globalScope) {
				auto *globalValue = module->createGlobalValue(name, type, {}, initValue, true);
				builder.getSymTable()->insert(name, globalValue, {}, initValue, true, SymTableEntry::GLOBAL);
			}
			else
			{
				auto *allocaInst = builder.createAllocaInst(type, {}, name);
				auto *storeInst = builder.createStoreInst(initValue, allocaInst,{},name);
				builder.getSymTable()->insert(name, allocaInst, {}, initValue, true, SymTableEntry::LOCAL);
			}
		}
	}
	return values;
}

// any SysYIRGenerator::GenerateZero(int Lay,string name)
// {
// 	//// cout<<"GenerateConstZero"<<endl;
// 	auto *nowArray = arrayTable.query(name);
// 	int Num_Layer = nowArray->dims.size();
// 	if(Lay>Num_Layer-1)
// 		Lay = Num_Layer-1;
// 	int num = nowArray->dims[Lay];
// 	bool isInt = nowArray->isInt;
// 	if(Lay>=Num_Layer-1)
// 	{
// 		if(isInt)
// 		{
// 			auto *constzero = (Value*)ConstantValue::get(0,"0");
// 			return constzero;
// 		}
// 		else
// 		{
// 			auto *constzero = (Value*)ConstantValue::get(0.0f,"0.0");
// 			return constzero;
// 		}
// 		return nullptr;
// 	}
// 	else
// 	{
// 		char Name[40];
// 		sprintf(Name,"ConstInitName%d",ConstInitListName);
// 		auto *values = module->createInitList(Name);
// 		ConstInitListName+=1;
		
// 		for(int i=0;i<num;i++)
// 		{
// 			//ConstLayer++;
// 			auto *value = any_cast<Value *>(GenerateZero(Lay+1,name));
// 			//ConstLayer--;
// 			if(value==nullptr)
// 			{
// 				// cout<<"constinitval is nullptr!!"<<endl;
// 				exit(-1);
// 			}
// 			if(values==nullptr)
// 			{
// 				// cout<<"initList==nullptr!"<<endl;
// 				exit(-1);
// 			}
// 			values->addOperand(value);
// 			//count++;
// 			//// cout<<"I have addOperandZero"<<endl;
// 		}
// 		return (Value *)values;
// 	}
// 	return nullptr;
	
// }

// void SysYIRGenerator::visitConstInitVal2(SysYParser::ConstInitValContext *ctx,InitList* values)
// {
// 	auto *nowArray = arrayTable.query(ConstArrayName);
// 	bool isInt = nowArray->isInt;
// 	int length = nowArray->dims.size();
// 	if(ctx->constExp())
// 	{
// 		auto *constexp = any_cast<Value *>(visitConstExp(ctx->constExp()));
		
// 		/*if(MyCount[ConstLayer]>nowArray->dims[ConstLayer])
// 		{
// 			cout<<"warning: excess elements in array initializer"<<endl;
// 			return;
// 		}*/
// 		values->addOperand(constexp);
// 		//MyCount++;
// 		for(int kk=0;kk<=ConstLayer;kk++)
// 			MyCount[kk]++;
// 		//int eleNum = values->getNumOperands();
// 		//return constexp;
// 	}
// 	else
// 	{
// 		//int MyCount = 0;
// 		MyCount[ConstLayer]=0;
// 		//int nowElement=0;
// 		int NumOfInit = ctx->constInitVal().size();
// 		for(int kk=0;kk<NumOfInit;kk++)
// 		{
// 			auto constInit = ctx->constInitVal(kk);
// 			if(constInit->LeftCurlyBracket())
// 			{
// 				ConstLayer++;
// 				MyCount[ConstLayer]=0;
// 				nowElement= MyCount[ConstLayer-1];
// 				visitConstInitVal2(constInit,values);
// 				//CurlyBracket = true;
// 			}
// 			else
// 			{
// 				visitConstInitVal2(constInit,values);
// 				if(kk+1<NumOfInit)
// 					continue;
// 			}
// 			if(ConstLayer>0 && nowElement%iCounts[length-ConstLayer-1]==0){
// 				int tmpm = MyCount[ConstLayer];
// 		//int goal = nowArray->dims[ConstLayer];
// 				int goal = iCounts[length-ConstLayer-1];
// 				if(tmpm>goal)
// 				{
// 					cout<<"warning: excess elements in array initializer"<<endl;
// 					return;
// 				}
// 				else if(tmpm<goal)
// 				{
// 					HalfInit = true;
// 					for(int l=tmpm+1;l<=goal;l++)
// 					{
// 						if(isInt)
// 						{
// 							auto *constzero = (Value*)ConstantValue::get(0,"0");
// 							values->addOperand(constzero);
// 							for(int jj=0;jj<=ConstLayer;jj++)
// 								MyCount[jj]++;
// 							//MyCount[ConstLayer]++;
// 						}
// 						else
// 						{
// 							auto *constzero = (Value*)ConstantValue::get(0.0f,"0.0");
// 							values->addOperand(constzero);
// 							for(int jj=0;jj<=ConstLayer;jj++)
// 								MyCount[jj]++;
// 							//MyCount[ConstLayer]++;
			
// 						}
// 					}
					
// 				}
// 			}
// 			if(constInit->LeftCurlyBracket())
// 			{
// 				ConstLayer--;
// 				//MyCount[ConstLayer]=0;
// 				//CurlyBracket = true;
// 			}
// 			if(values==nullptr)
// 			{
// 					// cout<<"initList==nullptr!"<<endl;
// 				exit(-1);
// 			}
// 			//values->addOperand(value);
// 			//count++;
// 			// cout<<"I have addOperand"<<endl;
// 		}
// 		if(ConstLayer==0)
// 		{
// 			int tmpm = MyCount[ConstLayer];
// 			//int goal = nowArray->dims[ConstLayer];
// 			int goal = iCounts[length-ConstLayer-1];
// 			if(tmpm>goal)
// 			{
// 				cout<<"warning: excess elements in array initializer"<<endl;
// 				return;
// 			}
// 			else if(tmpm<goal)
// 			{
// 				HalfInit = true;
// 				for(int l=tmpm+1;l<=goal;l++)
// 				{
// 					if(isInt)
// 					{
// 						auto *constzero = (Value*)ConstantValue::get(0,"0");
// 						values->addOperand(constzero);
// 						MyCount[ConstLayer]++;
// 					}
// 					else
// 					{
// 						auto *constzero = (Value*)ConstantValue::get(0.0f,"0.0");
// 						values->addOperand(constzero);
// 						MyCount[ConstLayer]++;
			
// 					}
// 				}
// 			}
		
// 		}
// 	}
// }

any SysYIRGenerator::visitConstInitVal(SysYParser::ConstInitValContext *ctx, const std::vector<int> &dims, int layer)
{
	// cerr<<"visit const init val"<<endl;
	if(ctx->constExp())
	{
		// scalar initialized by expression
		auto *constexp = any_cast<Value *>(visitConstExp(ctx->constExp()));
		nowElement++;
		return constexp;
	}
	else if (dims.empty())
	{
		// scalar initialized by list
		nowElement++;
		if (!ctx->constInitVal().empty()) {
			return any_cast<Value *>(visitConstInitVal(ctx->constInitVal().at(0), dims, layer + 1));
		} else {
			return (Value *)ConstantValue::get(0, "0");
		}
	}
	else
	{
		// array initialized by list
		std::vector<Value *> values;
		std::vector<int> indices;
		int startElement = nowElement;
		int nextElement = 1;
		for (size_t i = layer; i < dims.size(); ++i) {
			nextElement *= dims[i];
		}
		nextElement += startElement;

		for(auto *constInitVal:ctx->constInitVal())
		{
			if(nowElement >= nextElement) {
				cerr <<"warning: excess elements in array initializer"<<endl;
				break;
			}

			int curLayer = dims.size(), blockSize = 1;
			while (curLayer > layer + 1 && nowElement % (blockSize * dims[curLayer - 1]) == 0) {
				blockSize *= dims[curLayer - 1];
				curLayer--;
			}
			indices.push_back(nowElement);
			auto *value = any_cast<Value *>(visitConstInitVal(constInitVal, dims, curLayer));
			values.push_back(value);
		}
		auto *initValue = module->createInitList(values, indices, "initlist" + to_string(InitListName++));
		nowElement = nextElement;
		// auto *nowArray = arrayTable.query(ConstArrayName);
		// int num = dims[layer];
		// int higher = (ConstLayer+1>=nowArray->dims.size())?ConstLayer+1:nowArray->dims.size()-1;
		// int higherNum = nowArray->dims[higher];
		// bool isInt = nowArray->isInt;
		
		// for(int i=count+1;i<=num;i++)
		// {
		// 	// cout<<"GenerateConstZero"<<endl;
		// 	auto *gZero = any_cast<Value*>(GenerateZero(ConstLayer+1,ConstArrayName));
			
		// 	values->addOperand(gZero);
		// 	/*if(isInt)
		// 	{
		// 		int z = 0;
		// 		Value* zero = (Value*)ConstantValue::get(z);
		// 		values->addOperand(zero);
		// 	}
		// 	else
		// 	{
		// 		float z = 0.0;
		// 		Value* zero = (Value*)ConstantValue::get(z);
		// 		values->addOperand(zero);
		// 	}*/
		// }
		return (Value *)initValue;
	}
	return nullptr;
}


any SysYIRGenerator::visitVarDecl(SysYParser::VarDeclContext *ctx)
{
	// cerr << "visit var decl: " << ctx->getText() << endl;
	vector <Value*> values;
	auto *type = any_cast<Type *>(visitBType(ctx->bType()));
	for(auto *vardef:ctx->varDef())
	{
		auto name = vardef->Identifier()->getText();
		vector <Value *> dims;
		vector <int> iDims;
		if(!vardef->constExp().empty())
		{	// array
			for(auto constexp:vardef->constExp())
			{
				auto *dim = any_cast<Value*>(visitConstExp(constexp));
				dims.push_back(dim);
				iDims.push_back(dynamic_cast<ConstantValue *>(dim)->getInt());
			}
			if(vardef->Assign())
			{
				nowElement = 0;
				auto *initValue = any_cast<Value *>(visitInitVal(vardef->initVal(), iDims, 0));
				if(globalScope)
				{
					auto *globalValue = module->createGlobalValue(name, Type::getPointerType(type), dims, initValue, false);
					builder.getSymTable()->insert(name, globalValue, dims, initValue, false, SymTableEntry::GLOBAL);
				} else {
					auto *allocaInst = builder.createAllocaInst(type, dims, name);
					builder.getSymTable()->insert(name, allocaInst, dims, initValue, false, SymTableEntry::LOCAL);
					// 待补充：store数组
					storeLocalArray(allocaInst, initValue);
				}
			}
			else
			{
				if(globalScope) {
					auto *globalValue = module->createGlobalValue(name, Type::getPointerType(type), dims, nullptr, false);
					builder.getSymTable()->insert(name, globalValue, dims, {}, false, SymTableEntry::GLOBAL);
				} else {	
					auto *allocaInst = builder.createAllocaInst(type, dims, name);
					builder.getSymTable()->insert(name, allocaInst, dims, nullptr, false, SymTableEntry::LOCAL);
				}
			}
		}
		else
		{
			if(vardef->Assign())
			{
				auto *initValue = any_cast<Value *>(visitInitVal(vardef->initVal(), iDims, 0));
				if(globalScope) {
					auto *globalValue = module->createGlobalValue(name, Type::getPointerType(type), {}, initValue, false);
					builder.getSymTable()->insert(name, globalValue, {}, initValue, false, SymTableEntry::GLOBAL);
				} else {
					auto *allocaInst = builder.createAllocaInst(type, {}, name);
					builder.getSymTable()->insert(name, allocaInst, dims, initValue, false, SymTableEntry::LOCAL);
					auto *storeInst = builder.createStoreInst(initValue, allocaInst,{},name);
				}
			}
			else
			{
				if (globalScope) {
					if(type->isInt())
					{
						auto *initValue = ConstantValue::get(0, "0");
						auto *globalValue = module->createGlobalValue(name, Type::getPointerType(type), {}, nullptr, false);
						builder.getSymTable()->insert(name, globalValue, {}, nullptr, false, SymTableEntry::GLOBAL);
					}
					else if(type->isFloat())
					{
						auto *initValue = ConstantValue::get(0.0f, "0.0");
						auto *globalValue = module->createGlobalValue(name, Type::getPointerType(type), {}, nullptr, false);
						builder.getSymTable()->insert(name, globalValue, {}, nullptr, false, SymTableEntry::GLOBAL);
					}
				} else {
					auto *allocaInst = builder.createAllocaInst(type, {}, name);
					builder.getSymTable()->insert(name, allocaInst, {}, nullptr, false, SymTableEntry::LOCAL);
				}
			}
		}
		
	}
	return values;
}

void SysYIRGenerator::storeLocalArray(Value *base, Value *initVal) {
	auto *initList = dynamic_cast<InitList *>(initVal);
	auto indices = initList->getIndices();
	for (size_t i = 0; i < indices.size(); ++i) {
		Value *value = initList->getOperand(i);
		if (value->isInitList())
			storeLocalArray(base, value);
		else if (value->isFloat() || value->isInt()) {
			auto *addr = builder.createPAddInst(base, ConstantValue::get(indices[i] * 4, to_string(indices[i] * 4)), newTemp());
			builder.createStoreInst(value, addr);
		} else {
			cout << "storeLocalArray: initial value of error type\n";
		}
	}
}

// void SysYIRGenerator::visitInitVal2(SysYParser::InitValContext *ctx,InitList* values)
// {
// 	auto *nowArray = arrayTable.query(ArrayName);
// 	bool isInt = nowArray->isInt;
// 	int length = nowArray->dims.size();
// 	if(ctx->exp())
// 	{
// 		auto *exp = any_cast<Value *>(visitExp(ctx->exp()));
		
// 		/*if(MyCount[ConstLayer]>nowArray->dims[ConstLayer])
// 		{
// 			cout<<"warning: excess elements in array initializer"<<endl;
// 			return;
// 		}*/
// 		values->addOperand(exp);
// 		//MyCount++;
// 		for(int kk=0;kk<=Layer;kk++)
// 			MyCount[kk]++;
// 		//int eleNum = values->getNumOperands();
// 		//return constexp;
// 	}
// 	else
// 	{
// 		//int MyCount = 0;
// 		MyCount[Layer]=0;
// 		//int nowElement=0;
// 		int NumOfInit = ctx->initVal().size();
// 		for(int kk=0;kk<NumOfInit;kk++)
// 		{
// 			auto constInit = ctx->initVal(kk);
// 			if(constInit->LeftCurlyBracket())
// 			{
// 				Layer++;
// 				MyCount[Layer]=0;
// 				nowElement= MyCount[Layer-1];
// 				visitInitVal2(constInit,values);
// 				//CurlyBracket = true;
// 			}
// 			else
// 			{
// 				visitInitVal2(constInit,values);
// 				if(kk+1<NumOfInit)
// 					continue;
// 			}
// 			if(Layer>0 && nowElement%iCounts[length-Layer-1]==0){
// 				int tmpm = MyCount[Layer];
// 		//int goal = nowArray->dims[ConstLayer];
// 				int goal = iCounts[length-Layer-1];
// 				if(tmpm>goal)
// 				{
// 					cout<<"warning: excess elements in array initializer"<<endl;
// 					return;
// 				}
// 				else if(tmpm<goal)
// 				{
// 					HalfInit = true;
// 					for(int l=tmpm+1;l<=goal;l++)
// 					{
// 						if(isInt)
// 						{
// 							auto *constzero = (Value*)ConstantValue::get(0,"0");
// 							values->addOperand(constzero);
// 							for(int jj=0;jj<=Layer;jj++)
// 								MyCount[jj]++;
// 							//MyCount[ConstLayer]++;
// 						}
// 						else
// 						{
// 							auto *constzero = (Value*)ConstantValue::get(0.0f,"0.0");
// 							values->addOperand(constzero);
// 							for(int jj=0;jj<=Layer;jj++)
// 								MyCount[jj]++;
// 							//MyCount[ConstLayer]++;
			
// 						}
// 					}
					
// 				}
// 			}
// 			if(constInit->LeftCurlyBracket())
// 			{
// 				Layer--;
// 				//MyCount[ConstLayer]=0;
// 				//CurlyBracket = true;
// 			}
// 			if(values==nullptr)
// 			{
// 				// cout<<"initList==nullptr!"<<endl;
// 				exit(-1);
// 			}
// 			//values->addOperand(value);
// 			//count++;
// 			// cout<<"I have addOperand"<<endl;
// 		}
// 		if(Layer==0)
// 		{
// 			int tmpm = MyCount[Layer];
// 			//int goal = nowArray->dims[ConstLayer];
// 			int goal = iCounts[length-Layer-1];
// 			if(tmpm>goal)
// 			{
// 				cout<<"warning: excess elements in array initializer"<<endl;
// 				return;
// 			}
// 			else if(tmpm<goal)
// 			{
// 				HalfInit = true;
// 				for(int l=tmpm+1;l<=goal;l++)
// 				{
// 					if(isInt)
// 					{
// 						auto *constzero = (Value*)ConstantValue::get(0,"0");
// 						values->addOperand(constzero);
// 						MyCount[Layer]++;
// 					}
// 					else
// 					{
// 						auto *constzero = (Value*)ConstantValue::get(0.0f,"0.0");
// 						values->addOperand(constzero);
// 						MyCount[Layer]++;
			
// 					}
// 				}
// 			}
		
// 		}
// 	}
// }



any SysYIRGenerator::visitInitVal(SysYParser::InitValContext *ctx, const std::vector<int> &dims, int layer)
{
	// cerr << "visit init val: " << ctx->getText() << endl;
	// cerr << "    at layer " << layer << endl;
	// cerr << "    nowElement = " << nowElement << endl;
	if(ctx->exp())
	{
		// scalar initialized by expression
		auto exp = any_cast<Value *>(visitExp(ctx->exp()));
		nowElement++;
		return exp;
	}
	else if (dims.empty())
	{
		// scalar initialized by list
		nowElement++;
		if (!ctx->initVal().empty()) {
			return any_cast<Value *>(visitInitVal(ctx->initVal().at(0), dims, layer + 1));
		} else {
			return (Value *)ConstantValue::get(0, "0");
		}
	}
	else
	{
		// array initialized by list
		// char Name[40];
		// sprintf(Name,"InitList%d",InitListName);
		// auto *initList = module->createInitList(Name);
		// InitListName++;
		// int count=0;
		vector<Value *> values;
		std::vector<int> indices;
		int startElement = nowElement;
		int nextElement = 1;
		for (size_t i = layer; i < dims.size(); ++i) {
			nextElement *= dims[i];
		}
		nextElement += startElement;
		// cerr << "    nextElement = " << nextElement << endl;

		for(auto *initVal : ctx->initVal())
		{
			if(nowElement >= nextElement) {
				cerr << "warning: excess elements in array initializer" << endl;
				break;
			}
			int curLayer = dims.size(), blockSize = 1;
			while (curLayer > layer + 1 && nowElement % (blockSize * dims[curLayer - 1]) == 0) {
				blockSize *= dims[curLayer - 1];
				curLayer--;
			}
			indices.push_back(nowElement);
			auto *value = any_cast<Value*>(visitInitVal(initVal, dims, curLayer));
			values.push_back(value);
		}
		auto *initValue = module->createInitList(values, indices, "initlist" + to_string(InitListName++));
		nowElement = nextElement;
		// auto *nowArray = arrayTable.query(ArrayName);
		// int num = nowArray->dims[Layer];
		// //bool isInt = nowArray->isInt;
		// int higher = (Layer+1>=nowArray->dims.size())?Layer+1:nowArray->dims.size()-1;
		// int higherNum = nowArray->dims[higher];
		// bool isInt = nowArray->isInt;
		// if(count>num)
		// {
		// 	int full = 0;
		// 	//int index = 0;
		// 	char Name[40];
		// 	sprintf(Name,"InitList%d",InitListName);
		// 	auto *merge = module->createInitList(Name);
		// 	InitListName+=1;
		// 	//int Enumber = getNumOperands
		// 	int count2=count;
		// 	for(int k=0;k<count2;k++)
		// 	{
		// 		auto item = dynamic_cast<InitList*>(initList->getOperand(k));
		// 		//int itemopnum = item->getNumOperands();
		// 		//cout<<"itemopnum: "<<itemopnum<<endl;
		// 		if(item!=nullptr && item->getNumOperands()==higherNum)
		// 		{
		// 			if(full>0 && full<higherNum)
		// 			{
		// 				for(int j=full+1;j<=higherNum;j++)
		// 				{
		// 					auto *gZero = any_cast<Value*>(GenerateZero(Layer+1,ArrayName));
		// 					merge->addOperand(gZero);		
		// 				}
		// 				initList->setOperand(k-1,(Value*)merge);
		// 				for(int j=0;j<higherNum-1;j++)
		// 				{
		// 					initList->removeOperand(k-2-j);
		// 				}
		// 				count2-=higherNum-1;
		// 				k-=higherNum-1;
		// 				full = 0;
		// 				char Name[40];
		// 				sprintf(Name,"InitList%d",InitListName);
		// 				merge = module->createInitList(Name);
		// 				InitListName+=1;
		// 				}
		// 			else
		// 				;
		// 		}
		// 		else
		// 		{
		// 			merge->addOperand(initList->getOperand(k));
		// 			full++;
		// 		}
		// 		if(full==higherNum)
		// 		{
		// 			initList->setOperand(k,(Value*)merge);
		// 			for(int j=0;j<full-1;j++)
		// 			{
		// 				initList->removeOperand(k-1-j);
		// 			}
		// 			count2-=full-1;
		// 			k-=higherNum-1;
		// 			full = 0;
		// 			char Name[40];
		// 			sprintf(Name,"InitList%d",InitListName);
		// 			merge = module->createInitList(Name);
		// 			InitListName+=1;
		// 			//continue;
		// 		}
		// 	}
		// }
		// for(int i=count+1;i<=num;i++)
		// {
		// 	// cout<<"GenerateConstZero"<<endl;
		// 	auto *gZero = any_cast<Value*>(GenerateZero(Layer+1,ArrayName));
			
		// 	initList->addOperand(gZero);
		// }
		return (Value *)initValue;
	}
}

any SysYIRGenerator::visitFuncDef(SysYParser::FuncDefContext *ctx) {
	// cerr << "visit func def " + ctx->Identifier()->getText() << endl;
	Type *retType = any_cast<Type *>(visitFuncType(ctx->funcType()));
	vector<Type *> paramTypes;
	vector<string> paramNames;
	vector<vector<Value *> > paramDims;
	if(ctx->funcFParams())
	{
		for (auto *fparam : ctx->funcFParams()->funcFParam()) {
			Type *type = any_cast<Type *>(visitBType(fparam->bType()));
			vector<Value *> dims;
			if (!fparam->LeftSquareBracket().empty()) { // array argument
				dims.emplace_back(ConstantValue::get(0, "0"));
				paramTypes.emplace_back(Type::getPointerType(type));
				for (auto *exp : fparam->exp()) {
					dims.emplace_back(any_cast<Value *>(visitExp(exp)));
				}
			} else {
				paramTypes.emplace_back(type);
			}
			paramNames.emplace_back(fparam->Identifier()->getText());
			paramDims.push_back(dims);
		}
	}
	
	Type *funcType = Type::getFunctionType(retType, paramTypes);
	auto *function = module->createFunction(ctx->Identifier()->getText(), funcType);
	auto *entry = function->getEntryBlock();
	builder.setPosition(entry, entry->end());
	for (size_t i = 0; i < paramTypes.size(); ++i) {
		auto *argument = entry->createArgument(paramTypes[i], paramDims[i], paramNames[i]);
		function->getSymTable()->insert(paramNames[i], argument, paramDims[i], nullptr, false, SymTableEntry::ARGUMENT);
	}
	visitBlock(ctx->block());

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
	// cerr << "visit block " << ctx->getSourceInterval().toString() << "\n";
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
	// cerr << "visit stmt:" << ctx->getText() << endl;
	Value *ret = nullptr;
	if (ctx->lVal() != nullptr) {
		auto lval = any_cast<pair<Value *, Value *>>(visitLVal(ctx->lVal()));
		assert(lval.first != nullptr);				///< 赋值号左侧必须是地址
		Value *exp = any_cast<Value *>(visitExp(ctx->exp()));
		ret = builder.createStoreInst(exp, lval.first);
	} else if (ctx->Return() != nullptr) {
		Value *value = nullptr;
		if (ctx->exp() != nullptr)
			value = any_cast<Value *>(visitExp(ctx->exp()));
		ret = builder.createReturnInst(value);
	} else if (ctx->If() != nullptr) {
		auto *curBlock = builder.getBasicBlock();   						///< 当前所在基本块
		auto *func = curBlock->getParent();         						///< 当前所在函数
		auto *thenBlock = func->addBasicBlock(builder.getSymTable(), newBlockName() + "if_then"); 	///< then分支基本块
		auto *elseBlock = func->addBasicBlock(builder.getSymTable(), newBlockName() + "if_else");   ///< else分支基本块（可能没有，但构造指令时不能为nullptr）
		auto *exitBlock = func->addBasicBlock(builder.getSymTable(), newBlockName()); 				///< if-then-else结束后的部分
		auto *cond = any_cast<Value *>(visitCond(ctx->cond()));

		ret = builder.createCondBrInst(cond, thenBlock, elseBlock, {}, {});

		// translate thenblock
		curBlock->getSuccessors().push_back(thenBlock);
		thenBlock->getPredecessors().push_back(curBlock);
		builder.setPosition(thenBlock, thenBlock->end());
		visitStmt(ctx->stmt()[0]);
		builder.createUncondBrInst(exitBlock, {});
		auto *exitThenBlock = builder.getBasicBlock();

		// translate elseblock
		curBlock->getSuccessors().push_back(elseBlock);
		elseBlock->getPredecessors().push_back(curBlock);
		builder.setPosition(elseBlock, elseBlock->end());
		if (ctx->Else() != nullptr) {
			visitStmt(ctx->stmt()[1]);
		}
		builder.createUncondBrInst(exitBlock, {});
		auto *exitElseBlock = builder.getBasicBlock();

		// prepare to translate exitblock
		exitThenBlock->getSuccessors().push_back(exitBlock);
		exitBlock->getPredecessors().push_back(exitThenBlock);
		exitElseBlock->getSuccessors().push_back(exitBlock);
		exitBlock->getPredecessors().push_back(exitElseBlock);
		builder.setPosition(exitBlock, exitBlock->end());
	} else if (ctx->While() != nullptr) {
		auto *curBlock = builder.getBasicBlock();		///< 当前所在基本块
		auto *func = curBlock->getParent();				///< 当前所在函数
		auto *entryBlock = func->addBasicBlock(builder.getSymTable(), newBlockName() + "while_cond"); 	///< 循环入口，即循环条件计算和判断
		auto *bodyBlock = func->addBasicBlock(builder.getSymTable(), newBlockName() + "while_body"); 	///< while循环体块
		auto *exitBlock = func->addBasicBlock(builder.getSymTable(), newBlockName());  					///< while循环结束后的部分

		// translate cond/entry block
		curBlock->getSuccessors().push_back(entryBlock);
		entryBlock->getPredecessors().push_back(curBlock);
		builder.setPosition(entryBlock, entryBlock->end());
		auto *cond = any_cast<Value *>(visitCond(ctx->cond()));
		ret = builder.createCondBrInst(cond, bodyBlock, exitBlock, {}, {});
		auto *exitEntryBlock = builder.getBasicBlock();

		// translate inner block
		exitEntryBlock->getSuccessors().push_back(bodyBlock);
		bodyBlock->getPredecessors().push_back(exitEntryBlock);
		builder.setPosition(bodyBlock, bodyBlock->end());
		loopEntry.push_back(entryBlock);
		loopExit.push_back(exitBlock);
		visitStmt(ctx->stmt()[0]);
		builder.createUncondBrInst(entryBlock, {});
		loopExit.pop_back();
		loopEntry.pop_back();
		auto *exitbodyBlock = builder.getBasicBlock();

		// prepare to translate exitblock
		exitEntryBlock->getSuccessors().push_back(exitBlock);
		exitBlock->getPredecessors().push_back(exitEntryBlock);
		exitbodyBlock->getSuccessors().push_back(exitBlock);
		exitBlock->getPredecessors().push_back(exitbodyBlock);
		builder.setPosition(exitBlock, exitBlock->end());
	} else if (ctx->Break()) {
		std::vector<Value *> args; ///< 留空
		ret = builder.createUncondBrInst(loopExit.back(), args);
	} else if (ctx->Continue()) {
		std::vector<Value *> args; ///< 留空
		ret = builder.createUncondBrInst(loopEntry.back(), args);
	} else if (ctx->block()) {
		auto *predBlock = builder.getBasicBlock();
		auto *newBlock = predBlock->getParent()->addBasicBlock(new SymTable(predBlock->getSymTable()), newBlockName());
		auto *succBlock = predBlock->getParent()->addBasicBlock(predBlock->getSymTable(), newBlockName());

		// translate block
		predBlock->getSuccessors().push_back(newBlock);
		newBlock->getPredecessors().push_back(predBlock);
		builder.setPosition(newBlock, newBlock->end());
		ret = any_cast<Value *>(visitBlock(ctx->block()));
		auto *exitNewBlock = builder.getBasicBlock();

		// translate succBlock
		exitNewBlock->getSuccessors().push_back(succBlock);
		succBlock->getPredecessors().push_back(exitNewBlock);
		builder.setPosition(succBlock, succBlock->end());
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
	// cerr << "visit lval: " << ctx->getText() << endl;
	auto name = ctx->Identifier()->getText();
	auto *entry = builder.getSymTable()->query(ctx->Identifier()->getText());
	/*if(entry==nullptr)
	{
		entry = module->getGlobalValue(ctx->Identifier()->getText());
	}*/
	auto dims = entry->getDims();
	
	if (!ctx->exp().empty()) {
		// array
		vector<Value *> indices;
		bool constflag = true;

		for (auto *exp : ctx->exp()) {
			auto *index = any_cast<Value *>(visitExp(exp));
			auto *constIndex = dynamic_cast<ConstantValue *>(index);
			if(constIndex != nullptr) {
				indices.push_back(constIndex);
			} else {
				constflag = false;
				indices.push_back(index);
			}
		}
		if (constflag) {
			// const indices
			int offset = 0;
			assert(indices.size() == dims.size());
			for(size_t i = 0; i < dims.size(); ++i) {
				int index = dynamic_cast<ConstantValue *>(indices[i])->getInt();
				int boundary = dynamic_cast<ConstantValue *>(dims[i])->getInt();
				offset = offset * boundary + index;
			}
			offset = offset * 4;
			if (entry->isConstant()){
				InitList *initList = dynamic_cast<InitList *>(entry->getInitValue());
				return make_pair((Value *)nullptr, initList->getElement(offset));
			} else {
				Value *address = entry->getValue();
				if (entry->isArgument()) {
					address = builder.createLoadInst(address, {}, newTemp());
				}
				address = builder.createPAddInst(address, ConstantValue::get(offset, to_string(offset)), newTemp());
				return make_pair(address, (Value *)nullptr);
			}
		} else {
			// varieble indices
			Value *address = entry->getValue();
			Value *offset = ConstantValue::get(0, "0");
			if (entry->isArgument()) {
				address = builder.createLoadInst(address, {}, newTemp());
			}
			for (size_t i = 0; i < dims.size(); ++i) {
				auto *boundary = dynamic_cast<ConstantValue *>(dims[i]);
				offset = builder.createPMulInst(offset, boundary, newTemp());
				offset = builder.createPAddInst(offset, indices[i], newTemp());
			}
			offset = builder.createMulInst(offset, ConstantValue::get(4, "4"), newTemp());
			address = builder.createAddInst(address, offset, newTemp());
			return make_pair(address, (Value *)nullptr);
		}
	} else {
		// basic type varieble
		if (entry->isConstant()) {
			return make_pair((Value *)nullptr, entry->getValue());
		} else {
			return make_pair(entry->getValue(), (Value *)nullptr);
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
			if(globalScope && builder.getBasicBlock()==nullptr)
			{
				cout<<"error: initializer element is not constant"<<endl;
        		exit(0);
			}
			auto *loadValue = builder.createLoadInst(ptr,{},newTemp());
			// auto *loadValue = builder.createLoadInst(ptr,{},ptr->getName());
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
		//cout<<"vlaue: "<<value<<endl;
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
	// cerr<<"visit const exp"<<endl;
	return visitAddExp(ctx->addExp());
}

} // namespace sysy
