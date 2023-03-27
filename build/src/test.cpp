#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;
#include "SysYLexer.h"
#include "SysYParser.h"
using namespace antlr4;

int main(int argc,char **argv)
{
	if(argc !=2)
	{
		cerr<<"Usage: "<<argv[0] <<"inputfile\n";
		return EXIT_FAILURE;
	}
	ifstream fin(argv[1]);
	ANTLRInputStream input(fin);
	SysYLexer lexer(&input);
	CommonTokenStream tokens(&lexer);
	SysYParser parser(&tokens);
	SysYParser::CompUnitContext *comp = parser.compUnit();
	return EXIT_SUCCESS;
}
