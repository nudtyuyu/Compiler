#include <algorithm>
#include <iostream>
using namespace std;
#include "ASTPrinter.h"
#include "SysYParser.h"

long long Indent = 0;

/*any ASTPrinter::visitNumber(SysYParser::NumberContext *ctx) {
  cout << ctx->IntConst()->getText();
  return nullptr;
}*/

any ASTPrinter::visitString(SysYParser::StringContext *ctx) {
  cout << ctx->String()->getText();
  return nullptr;
}

/*any ASTPrinter::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {
  if (ctx->funcRParam().empty())
    return nullptr;
  auto numParams = ctx->funcRParam().size();
  ctx->funcRParam(0)->accept(this);
  for (int i = 1; i < numParams; ++i) {
    cout << ", ";
    ctx->funcRParam(i)->accept(this);
  }
  if(ctx->exp().empty())
  	return nullptr;
  auto numParams = ctx->exp().size();
  ctx->exp(0)->accept(this);
  for(int i=1;i<numParams;i++)
  {
  	cout << ", ";
  	ctx-> exp(i)->accept(this); //get i-th params
  }
  cout << '\n';
  return nullptr;
}*/

any ASTPrinter::visitFuncType(SysYParser::FuncTypeContext *ctx)
{
	//cout<<"visitFuncType"<<endl;
	//cout<<"index"<<i;
	if(ctx->Void())
		cout<<ctx->Void()->getText();
	else if(ctx->Int())
		cout<<ctx->Int()->getText();
	else if(ctx->Float())
		cout<<ctx->Float()->getText();
	return nullptr;
}

any ASTPrinter::visitLVal(SysYParser::LValContext *ctx)
{
	cout<<ctx->Identifier()->getText();
	if(ctx->exp().empty())
		return nullptr;
	int numexp = ctx->exp().size();
	for(int i=0;i<numexp;i++)
	{
		cout<<ctx->LeftSquareBracket(i)->getText();
		visitExp(ctx->exp(i));
		cout<<ctx->RightSquareBracket(i)->getText();
	}
	return nullptr;
}

any ASTPrinter::visitNumber(SysYParser::NumberContext *ctx)
{
	if(ctx->IntConst())
		cout<<ctx->IntConst()->getText();
	else if(ctx->FloatConst())
		cout<<ctx->FloatConst()->getText();
	return nullptr;
}

any ASTPrinter::visitPrimaryExp(SysYParser::PrimaryExpContext *ctx)
{
	auto * child = ctx->children[0];
	auto *L_children = dynamic_cast<SysYParser::LValContext *>(child);
	auto *N_children = dynamic_cast<SysYParser::NumberContext *>(child);
	if(ctx->LeftBracket())
	{
		cout<<ctx->LeftBracket()->getText();
		visitExp(ctx->exp());
		cout<<ctx->RightBracket()->getText();
	}
	else if(L_children != nullptr)
	{
		visitLVal(ctx->lVal()); 
	}
	else if(N_children != nullptr)
	{
		visitNumber(ctx->number()); 
	}
	return nullptr;
	
}

any ASTPrinter::visitUnaryOp(SysYParser::UnaryOpContext *ctx)
{
	if(ctx->Add())
	{
		
		cout<<ctx->Add()->getText();
		
	}
	else if(ctx->Sub())
	{
		
		cout<<ctx->Sub()->getText();
		
	}
	else if(ctx->Not())
		cout<<ctx->Not()->getText();
	return nullptr;
}

any ASTPrinter::visitFuncRParams(SysYParser::FuncRParamsContext *ctx)
{
	int numexp = ctx->exp().size();
	if(numexp==1)
	{
		visitExp(ctx->exp(0));
	}
	else
	{
		visitExp(ctx->exp(0));
		for(int i=1;i<numexp;i++)
		{
			cout<<ctx->Comma(i-1)->getText();
			cout<<" ";
			visitExp(ctx->exp(i));
		}
	}
	return nullptr;
}

any ASTPrinter::visitUnaryExp(SysYParser::UnaryExpContext *ctx)
{
	auto *child = ctx->children[0];
	auto *pri_children = dynamic_cast<SysYParser::PrimaryExpContext *>(child);
	auto *unary_children = dynamic_cast<SysYParser::UnaryOpContext *>(child);
	if(pri_children !=nullptr)
	{
		visitPrimaryExp(ctx->primaryExp()); 
	}
	else if(ctx->Identifier())
	{
		cout<<ctx->Identifier()->getText();
		cout<<ctx->LeftBracket()->getText();
		if(ctx->funcRParams())
			visitFuncRParams(ctx->funcRParams());
		cout<<ctx->RightBracket()->getText();
	}
	else if(unary_children != nullptr)
	{
		//cout<<"(";
		visitUnaryOp(ctx->unaryOp());
		visitUnaryExp(ctx->unaryExp()); 
		//cout<<")";
	}
	return nullptr;
}

any ASTPrinter::visitMulExp(SysYParser::MulExpContext *ctx)
{
	auto *child  = ctx->children[0];
	auto *unary_children = dynamic_cast<SysYParser::UnaryExpContext *>(child);
	auto *mul_children = dynamic_cast<SysYParser::MulExpContext *>(child);
	if(mul_children!=nullptr)
	{
		visitMulExp(ctx->mulExp());
		if(ctx->Mul())
			cout<<" "<<ctx->Mul()->getText()<<" ";
		else if(ctx->Div())
			cout<<" "<<ctx->Div()->getText()<<" ";
		else if(ctx->Mod())
			cout<<" "<<ctx->Mod()->getText()<<" ";
		visitUnaryExp(ctx->unaryExp());
	}
	else if(mul_children==nullptr && unary_children!=nullptr)
	{
		visitUnaryExp(ctx->unaryExp());
	}
	return nullptr;
}

any ASTPrinter::visitAddExp(SysYParser::AddExpContext *ctx)
{
	//cout<<"visitAddExp"<<endl;
	//cout<<"vistAddExp"<<endl;
	auto *child = ctx->children[0];
	auto *add_child = dynamic_cast<SysYParser::AddExpContext *>(child);
	auto *mul_child = dynamic_cast<SysYParser::MulExpContext *>(child);
	if(add_child != nullptr)
	{	
		//cout<<"addExp!"<<endl;
		visitAddExp(ctx->addExp());
		if(ctx->Add())
			cout<<" "<<ctx->Add()->getText()<<" ";
		else if(ctx->Sub())
			cout<<" "<<ctx->Sub()->getText()<<" ";
		visitMulExp(ctx->mulExp());
	}
	else if(add_child == nullptr && mul_child != nullptr)
	{
		//cout<<"mulExp!"<<endl;
		visitMulExp(ctx->mulExp());
	}
	return nullptr;
}


any ASTPrinter::visitExp(SysYParser::ExpContext *ctx)
{
	//cout<<"visitExp"<<endl;
	/*if(ctx->addExp())
		cout<<"I have addExp"<<endl;
	ctx->addExp()->accept(this);*/
	//cout<<ctx->addExp();
	visitAddExp(ctx->addExp());
	return nullptr;
}


any ASTPrinter::visitBType(SysYParser::BTypeContext *ctx)
{
	//cout<<"visitBType"<<endl;
	if(ctx->Int())
	{
		//cout<<" it is INT"<<endl;
		cout<<ctx->Int()->getText();
	}
	else if(ctx->Float())
	{
		//cout<<" it is FLOAT"<<endl;
		cout<<ctx->Float()->getText();
	}
	//cout<<"???"<<endl;
	return nullptr;
}

any ASTPrinter::visitFuncFParam(SysYParser::FuncFParamContext *ctx)
{
	//cout<<"visitFuncFParam"<<endl;
	visitBType(ctx->bType()); 
	cout<<" ";
	cout<<ctx->Identifier()->getText();
	if(ctx->LeftSquareBracket().empty())
	{
		//cout<<"SquareBracket: "<<0<<endl;
		return nullptr;
	}
	int numBracket = ctx->LeftSquareBracket().size();
	if(numBracket==1)
	{
		cout<<ctx->LeftSquareBracket(0)->getText();
		cout<<ctx->RightSquareBracket(0)->getText();
	}
	else
	{
		cout<<ctx->LeftSquareBracket(0)->getText();
		cout<<ctx->RightSquareBracket(0)->getText();
		for(int i=1;i<numBracket;i++)
		{
			cout<<ctx->LeftSquareBracket(i)->getText();
			visitExp(ctx->exp(i-1)); // exp waiting to define
			//cout<<"Exp"<<endl;
			cout<<ctx->RightSquareBracket(i)->getText();
		}
	}
	return nullptr;
}

any ASTPrinter::visitFuncFParams(SysYParser::FuncFParamsContext *ctx)
{
	//cout<<"visitFuncFParams"<<endl;
	if(ctx->funcFParam().empty())
	{
		//cout<<"no param"<<endl;
		return nullptr;
	}
	int numParam = ctx->funcFParam().size();
	//cout<<"numParam: "<<numParam<<endl;
	visitFuncFParam(ctx->funcFParam(0));
	for(int i=1;i<numParam;i++)
	{
		cout<<ctx->Comma(i-1)->getText();
		cout<<" ";
		visitFuncFParam(ctx->funcFParam(i));
		
	}
	return nullptr;
}

any ASTPrinter::visitConstInitVal(SysYParser::ConstInitValContext *ctx)
{
	visitConstExp(ctx->constExp());
	return nullptr;
}


any ASTPrinter::visitConstExp(SysYParser::ConstExpContext *ctx)
{
	visitAddExp(ctx->addExp());
	return nullptr;
}

any ASTPrinter::visitConstDef(SysYParser::ConstDefContext *ctx)
{
	cout<<ctx->Identifier()->getText();
	int numexp = ctx->constExp().size();
	for(int i=0;i<numexp;i++)
	{
		cout<<ctx->LeftSquareBracket(i)->getText();
		visitConstExp(ctx->constExp(i));
		cout<<ctx->RightSquareBracket(i)->getText();
	}
	cout<<" ";
	cout<<ctx->Assign()->getText();
	cout<<" ";
	visitConstInitVal(ctx->constInitVal());
	return nullptr;
}

any ASTPrinter::visitConstDecl(SysYParser::ConstDeclContext *ctx)
{
	cout<<ctx->Const()->getText();
	cout<<" ";
	visitBType(ctx->bType());
	cout<<" ";
	int numdef = ctx->constDef().size();
	if(numdef==1)
		visitConstDef(ctx->constDef(0));
	else if(numdef>1)
	{
		visitConstDef(ctx->constDef(0));
		for(int i=1;i<numdef;i++)
		{
			cout<<ctx->Comma(i-1)->getText()<<" ";
			visitConstDef(ctx->constDef(i));
		}
	}
	cout<<ctx->Semicolon()->getText();
	return nullptr;
	
}

any ASTPrinter::visitInitVal(SysYParser::InitValContext *ctx)
{
	auto *child = ctx->children[0];
	auto *exp_child = dynamic_cast<SysYParser::ExpContext *>(child);
	if(exp_child != nullptr)
	{
		visitExp(ctx->exp());
	}
	else if(ctx->LeftCurlyBracket())
	{
		cout<<ctx->LeftCurlyBracket()->getText();
		int numv = ctx->initVal().size();
		if(numv==1)
		{
			visitInitVal(ctx->initVal(0));
		}
		else if(numv>1)
		{
			visitInitVal(ctx->initVal(0));
			for(int i=1;i<numv;i++)
			{
				cout<<ctx->Comma(i-1)->getText();
				cout<<" ";
				visitInitVal(ctx->initVal(i));
			}
		}
		cout<<ctx->RightCurlyBracket()->getText();
	}
	return nullptr;
	
}

any ASTPrinter::visitVarDef(SysYParser::VarDefContext *ctx)
{
	cout<<ctx->Identifier()->getText();
	int numexp = ctx->constExp().size();
	for(int i=0;i<numexp;i++)
	{
		cout<<ctx->LeftSquareBracket(i)->getText();
		visitConstExp(ctx->constExp(i));
		cout<<ctx->RightSquareBracket(i)->getText();
	}
	if(ctx->Assign())
	{
		cout<<" ";
		cout<<ctx->Assign()->getText();
		cout<<" ";
		visitInitVal(ctx->initVal());
	}
	return nullptr;
}

any ASTPrinter::visitVarDecl(SysYParser::VarDeclContext *ctx)
{
	visitBType(ctx->bType());
	cout<<" ";
	int numdef = ctx->varDef().size();
	if(numdef==1)
		visitVarDef(ctx->varDef(0));
	else if(numdef>1)
	{
		visitVarDef(ctx->varDef(0));
		for(int i=1;i<numdef;i++)
		{
			cout<<ctx->Comma(i-1)->getText()<<" ";
			visitVarDef(ctx->varDef(i));
		}
	}
	cout<<ctx->Semicolon()->getText();
	return nullptr;
}

any ASTPrinter::visitDecl(SysYParser::DeclContext *ctx)
{
	auto *child = ctx->children[0];
	auto *const_child = dynamic_cast<SysYParser::ConstDeclContext *>(child);
	auto *var_child = dynamic_cast<SysYParser::VarDeclContext *>(child);
	if(const_child != nullptr)
	{
		visitConstDecl(ctx->constDecl());
	}
	else if(var_child != nullptr)
	{
		visitVarDecl(ctx->varDecl());
	}
	return nullptr;
	
}

any ASTPrinter::visitRelExp(SysYParser::RelExpContext *ctx)
{
	auto *child = ctx->children[0];
	auto *Rel_child = dynamic_cast<SysYParser::RelExpContext *>(child);
	auto *add_child = dynamic_cast<SysYParser::AddExpContext *>(child);
	if(add_child != nullptr)
	{
		visitAddExp(ctx->addExp());
	}
	else if(Rel_child != nullptr)
	{
		visitRelExp(ctx->relExp());
		if(ctx->LessThan())
			cout<<" "<<ctx->LessThan()->getText()<<" ";
		else if(ctx->GreaterThan())
			cout<<" "<<ctx->GreaterThan()->getText()<<" ";
		else if(ctx->LessEqual())
			cout<<" "<<ctx->LessEqual()->getText()<<" ";
		else if(ctx->GreaterEqual())
			cout<<" "<<ctx->GreaterEqual()->getText()<<" ";
		visitAddExp(ctx->addExp());
	}
	return nullptr;
}

any ASTPrinter::visitEqExp(SysYParser::EqExpContext *ctx)
{
	auto *child = ctx->children[0];
	auto *Rel_child = dynamic_cast<SysYParser::RelExpContext *>(child);
	auto *Eq_child = dynamic_cast<SysYParser::EqExpContext *>(child);
	if(Rel_child != nullptr)
	{
		visitRelExp(ctx->relExp());
	}
	else if(Eq_child != nullptr)
	{
		visitEqExp(ctx->eqExp());
		if(ctx->Equal())
			cout<<" "<<ctx->Equal()->getText()<<" ";
		else if(ctx->NonEqual())
			cout<<" "<<ctx->NonEqual()->getText()<<" ";
		visitRelExp(ctx->relExp());
	}
	return nullptr;
}

any ASTPrinter::visitLAndExp(SysYParser::LAndExpContext *ctx)
{
	auto *child = ctx->children[0];
	auto *and_child = dynamic_cast<SysYParser::LAndExpContext *>(child);
	auto *Eq_child = dynamic_cast<SysYParser::EqExpContext *>(child);
	if(Eq_child != nullptr)
	{
		visitEqExp(ctx->eqExp());
	}
	else if(and_child != nullptr)
	{
		visitLAndExp(ctx->lAndExp());
		cout<<" "<<ctx->And()->getText()<<" ";
		visitEqExp(ctx->eqExp());
	}
	return nullptr;
}

any ASTPrinter::visitLOrExp(SysYParser::LOrExpContext *ctx)
{
	auto *child = ctx->children[0];
	auto *and_child = dynamic_cast<SysYParser::LAndExpContext *>(child);
	auto *or_child = dynamic_cast<SysYParser::LOrExpContext *>(child);
	if(and_child != nullptr)
	{
		visitLAndExp(ctx->lAndExp());
	}
	else if(or_child != nullptr)
	{
		visitLOrExp(ctx->lOrExp());
		cout<<" "<<ctx->Or()->getText()<<" ";
		visitLAndExp(ctx->lAndExp());
	}
	return nullptr;
}

any ASTPrinter::visitCond(SysYParser::CondContext *ctx)
{
	visitLOrExp(ctx->lOrExp());
	return nullptr;
}

any ASTPrinter::visitStmt(SysYParser::StmtContext *ctx)
{
	auto *child = ctx->children[0];
	auto *LVal_child = dynamic_cast<SysYParser::LValContext *>(child);
	auto *Block_child = dynamic_cast<SysYParser::BlockContext *>(child);
	auto *Exp_child = dynamic_cast<SysYParser::ExpContext *>(child);
	if(LVal_child != nullptr)
	{
		visit(ctx->lVal());
		cout<<" "<<ctx->Assign()->getText()<<" ";
		visitExp(ctx->exp());
		cout<<ctx->Semicolon()->getText();
	}
	else if(Exp_child !=nullptr)
	{
		visitExp(ctx->exp());
		cout<<ctx->Semicolon()->getText();
	}
	else if(Block_child !=nullptr)
	{
		Indent++;
		visitBlock(ctx->block());
		Indent--;
	}
	else if(ctx->If())
	{
		cout<<ctx->If()->getText();
		cout<<ctx->LeftBracket()->getText();
		visitCond(ctx->cond());
		cout<<ctx->RightBracket()->getText();
		auto *st = ctx->stmt(0)->children[0];
		auto *S_child = dynamic_cast<antlr4::tree::TerminalNode *>(st);
		auto *L_child = dynamic_cast<SysYParser::LValContext *>(st);
		auto *E_child = dynamic_cast<SysYParser::ExpContext *>(st);
		if(E_child != nullptr || L_child !=nullptr || S_child!=nullptr)
		{
			cout<<"{"<<endl;
			Indent++;
			for(int i=0;i<Indent;i++)
			{
				cout<<"\t";
			}
			visit(ctx->stmt(0));
			cout<<"\n";
			for(int i=1;i<Indent;i++)
			{
				cout<<"\t";
			}
			cout<<"}";
			Indent--;
		}
		else
			visitStmt(ctx->stmt(0));
		if(ctx->Else())
		{
			cout<<"\n";
			for(int i=0;i<Indent;i++)
			{
				cout<<"\t";
			}
			cout<<ctx->Else()->getText();
			auto *st = ctx->stmt(1)->children[0];
			auto *S_child = dynamic_cast<antlr4::tree::TerminalNode *>(st);
			auto *L_child = dynamic_cast<SysYParser::LValContext *>(st);
			auto *E_child = dynamic_cast<SysYParser::ExpContext *>(st);
			if(E_child != nullptr || L_child !=nullptr || S_child!=nullptr)
			{
				cout<<"{"<<endl;
				Indent++;
				for(int i=0;i<Indent;i++)
				{
					cout<<"\t";
				}	
				visit(ctx->stmt(1));
				cout<<"\n";
				for(int i=1;i<Indent;i++)
				{
					cout<<"\t";
				}
				cout<<"}";
				Indent--;
			}
			else
				visitStmt(ctx->stmt(1));
		}
	}
	else if(ctx->While())
	{
		cout<<ctx->While()->getText();
		cout<<ctx->LeftBracket()->getText();
		visitCond(ctx->cond());
		cout<<ctx->RightBracket()->getText();
		auto *st = ctx->stmt(0)->children[0];
		auto *S_child = dynamic_cast<antlr4::tree::TerminalNode *>(st);
		auto *L_child = dynamic_cast<SysYParser::LValContext *>(st);
		auto *E_child = dynamic_cast<SysYParser::ExpContext *>(st);
		if(E_child != nullptr || L_child !=nullptr || S_child!=nullptr)
		{
			cout<<"{"<<endl;
			Indent++;
			for(int i=0;i<Indent;i++)
			{
				cout<<"\t";
			}
			visit(ctx->stmt(0));
			cout<<"\n";
			for(int i=1;i<Indent;i++)
			{
				cout<<"\t";
			}
			cout<<"}";
			Indent--;
		}
		else
			visitStmt(ctx->stmt(0));
	}
	else if(ctx->Break())
	{
		cout<<ctx->Break()->getText();
		cout<<ctx->Semicolon()->getText();
	}
	else if(ctx->Continue())
	{
		cout<<ctx->Continue()->getText();
		cout<<ctx->Semicolon()->getText();
	}
	else if(ctx->Return())
	{
		cout<<ctx->Return()->getText();
		cout<<" ";
		if(ctx->exp())
			visitExp(ctx->exp());
		cout<<ctx->Semicolon()->getText();
	}
	else
	{
		cout<<ctx->Semicolon()->getText();
	}
	return nullptr;
	
}

any ASTPrinter::visitBlockItem(SysYParser::BlockItemContext *ctx)
{
	auto *child = ctx->children[0];
	auto *decl_child = dynamic_cast<SysYParser::DeclContext *>(child);
	auto *stmt_child = dynamic_cast<SysYParser::StmtContext *>(child);
	if(decl_child!=nullptr)
	{
		//cout<<"This is Decl";
		visitDecl(ctx->decl()); ///waiting;
	}
	else if(stmt_child!=nullptr)
	{
		//cout<<"This is Stmt";
		visitStmt(ctx->stmt()); //waiting;
	}
	return nullptr;
}

any ASTPrinter::visitBlock(SysYParser::BlockContext *ctx)
{
	cout<<ctx->LeftCurlyBracket()->getText()<<endl;
	for(int i=0;i<Indent;i++)
	{
		cout<<"\t";
	}
	if(ctx->blockItem().empty())
		;
	int numb = ctx->blockItem().size();
	if(numb==1)
	{
		visitBlockItem(ctx->blockItem(0));
	}
	else if(numb>1)
	{
		visitBlockItem(ctx->blockItem(0));
		for(int i=1;i<numb;i++)
		{
			cout<<"\n";
			for(int i=0;i<Indent;i++)
			{
				cout<<"\t";
			}
			visitBlockItem(ctx->blockItem(i));
		}
	}
	cout<<"\n";
	for(int i=1;i<Indent;i++)
	{
		cout<<"\t";
	}
	cout<<ctx->RightCurlyBracket()->getText();
	return nullptr;
}

any ASTPrinter::visitFuncDef(SysYParser::FuncDefContext *ctx)
{
	//cout<<"visitFuncDef"<<endl;
	visitFuncType(ctx->funcType());
	cout<<' ';
	cout<<ctx->Identifier()->getText();
	//cout<<' ';
	cout<<ctx->LeftBracket()->getText();
	if(ctx->funcFParams())
		visitFuncFParams(ctx->funcFParams());
	cout<<ctx->RightBracket()->getText();
	Indent++;
	visitBlock(ctx->block());
	Indent--;
	return nullptr;
	
}

any ASTPrinter::visitCompUnit(SysYParser::CompUnitContext *ctx)
{
	if(ctx->decl().empty() && ctx->funcDef().empty())
		return nullptr;
	auto numDecl = ctx->decl().size();
	auto numFuncDef = ctx->funcDef().size();
	//if(!ctx->decl().empty)
	//cout<<"begin!"<<endl;
	//cout<<"numDecl: "<<numDecl<<endl;
	//cout<<"numFuncDef: "<<numFuncDef<<endl;
	for(int i=0;i<numDecl;i++)
	{
		//cout<<"i"<<i<<endl;
		//ctx->decl(i)->accept(this);
		visitDecl(ctx->decl(i));
		cout<<endl;
		
	}
	cout<<endl;
	for(int i=0;i<numFuncDef;i++)
	{
		//cout<<"i "<<i<<endl;
		//ctx->funcDef(i)->accept(this);
		visitFuncDef(ctx->funcDef(i));
		cout<<'\n'<<endl;
	}
	return nullptr;
}
