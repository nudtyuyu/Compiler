#include "tree/ParseTreeWalker.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;
#include "SysYLexer.h"
#include "SysYParser.h"
using namespace antlr4;
// #include "SysYFormatter.h"
#include "SysYIRGenerator.h"
#include "backend/codegen.hpp"
using namespace sysy;
using backend::CodeGen;

int main(int argc, char **argv) {
  if (argc > 3) {
    cerr << "Usage: " << argv[0] << " inputfile [ir]\n";
    return EXIT_FAILURE;
  }
  bool genir = false;
  if(argc > 2){
    genir = true;
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
  //auto moduleAST = parser.module();
  
  auto compile = parser.compUnit();

  sysy::SysYIRGenerator generator;
  generator.visitCompUnit(compile);

  //SysYIRGenerator generator;
  //generator.visitModule(moduleAST);
  auto moduleIR = generator.get();
  //only generate SysY IR code
  if(genir){
    //moduleIR->print(cout);
    generator.generateCode(std::cout);
    return EXIT_SUCCESS;
  }

  CodeGen codegen(moduleIR);
  string asmCode = codegen.code_gen();
  cout << asmCode << endl;;
  
  return EXIT_SUCCESS;
}
