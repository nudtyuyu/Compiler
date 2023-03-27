
// Generated from SysY.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"




class  SysYLexer : public antlr4::Lexer {
public:
  enum {
    Comma = 1, Semicolon = 2, LeftBracket = 3, RightBracket = 4, LeftCurlyBracket = 5, 
    RightCurlyBracket = 6, LeftSquareBracket = 7, RightSquareBracket = 8, 
    Dot = 9, IntConst = 10, FloatConst = 11, Const = 12, Int = 13, Float = 14, 
    Return = 15, Void = 16, If = 17, Else = 18, While = 19, Break = 20, 
    Continue = 21, Identifier = 22, Add = 23, Sub = 24, Mul = 25, Div = 26, 
    Mod = 27, Equal = 28, NonEqual = 29, LessThan = 30, GreaterThan = 31, 
    LessEqual = 32, GreaterEqual = 33, Not = 34, And = 35, Or = 36, Assign = 37, 
    String = 38, WS = 39, LINE_COMMENT = 40, COMMENT = 41
  };

  explicit SysYLexer(antlr4::CharStream *input);

  ~SysYLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

