#include "ASTPrinter.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;
using namespace antlr4;
#include "SysYLexer.h"
#include "SysYParser.h"
#include "ASTPrinter.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << "inputfile\n";
    return EXIT_FAILURE;
  }
  ifstream fin(argv[1]);
  if (not fin) {
    cerr << "Failed to open file " << argv[1];
    return EXIT_FAILURE;
  }
  ANTLRInputStream input(fin);
  SysYLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  SysYParser::CompUnitContext *comp = parser.compUnit();
  //SysYParser::FuncRParamsContext *params = parser.funcRParams();
  //SysYParser::FuncRParamsContext *params = comp->funcDef()->block()->exp()->addExp()->mulExp()->unaryExp->funcRParams();
  ASTPrinter printer;
  //printer.visitFuncRParams(params);
  printer.visitCompUnit(comp);

  return EXIT_SUCCESS;
}
