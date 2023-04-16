//#include "ASTPrinter.h"
#include "tree/ParseTreeWalker.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include "SysYLexer.h"
#include "SysYParser.h"
using namespace antlr4;
#include "SysYFormatter.h"
#include "SysYIRGenerator.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << "inputfile\n";
    return EXIT_FAILURE;
  }
  std::ifstream fin(argv[1]);
  if (not fin) {
    std::cerr << "Failed to open file " << argv[1];
    return EXIT_FAILURE;
  }
  ANTLRInputStream input(fin);
  SysYLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  //auto module = parser.module();
  auto compile = parser.compUnit();
  //auto exp = parser.exp();

  sysy::SysYIRGenerator generator;
  //generator.visitModule();
  generator.visitCompUnit(compile);
  //auto IR = generator.visitExp(exp);

  std::stringstream IRCode;
  generator.generateCode(IRCode);
  std::cout << "\n\nIR code:\n" << IRCode.str();

  return EXIT_SUCCESS;
}
