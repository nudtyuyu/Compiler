grammar IR;

/*===-------------------------------------------===*/
/* Lexer rules                                     */
/*===-------------------------------------------===*/

// fragments

// keywords
CONST: 'const';
GLOBAL: 'global';
INT: 'int';
FLOAT: 'float';
VOID: 'void';

// instructions
NOT: 'not';
AND: 'and';
OR: 'or';
ADD: 'add';
SUB: 'sub';
MUL: 'mul';
DIV: 'div';
REM: 'rem';
NEG: 'neg';
CMPEQ: 'cmpeq';
CMPNE: 'cmpne';
CMPLT: 'cmplt';
CMPGT: 'cmpgt';
CMPLE: 'cmple';
CMPGE: 'cmpge';
FADD: 'fadd';
FSUB: 'fsub';
FMUL: 'fmul';
FDIV: 'fdiv';
FREM: 'frem';
FNEG: 'fneg';
FCMPEQ: 'fcmpeq';
FCMPNE: 'fcmpne';
FCMPLT: 'fcmplt';
FCMPGT: 'fcmpgt';
FCMPLE: 'fcmple';
FCMPGE: 'fcmpge';
FTOI: 'ftoi';
ITOF: 'itof';
CALL: 'call';
BR: 'br';
CONDBR: 'condbr';
RETURN: 'return';
ALLOCA: 'alloca';
LOAD: 'load';
STORE: 'store';

// punctuations
BIND: '=';
COMMA: ',';
COLON: ':';
SEMICOLON: ';';
LPAREN: '(';
RPAREN: ')';
LBRACKET: '[';
RBRACKET: ']';
LBRACE: '{';
RBRACE: '}';
// PERCENT: '%'; AT: '@';

// identifier
fragment ALPHA: [a-zA-Z];
fragment ALPHANUM: [a-zA-Z0-9];
fragment NONDIGIT: [a-zA-Z_];
LOCALID: '%' (ALPHANUM | '_')+;
GLOBALID: '@' (ALPHANUM | '_')+;
BLOCKID: '^' NONDIGIT (ALPHANUM | '_')*;

// literals
fragment DecDigit: [0-9];
fragment OctDigit: [0-7];
fragment HexDigit: [0-9a-fA-F];
fragment OctPrefix: '0';
fragment HexPrefix: '0' [xX];
fragment NonZeroDecDigit: [1-9];
fragment Sign: [+-];
fragment DecFractional: DecDigit* '.' DecDigit+ | DecDigit+ '.';
fragment Exponent: [eE] Sign? DecDigit+;
fragment DecFloat: DecFractional Exponent? | DecDigit+ Exponent;
fragment HexFractional: HexDigit* '.' HexDigit+ | HexDigit+ '.';
fragment BinExponent: [pP] Sign? DecDigit+;
fragment HexFloat:
	HexPrefix HexFractional BinExponent
	| HexDigit+ BinExponent;

ILITERAL:
	NonZeroDecDigit DecDigit*
	| OctPrefix OctDigit*
	| HexPrefix HexDigit+;

FLITERAL: DecFloat | HexFloat;

// string
fragment ESC: '\\"' | '\\\\';
STRING: '"' (ESC | .)*? '"';

// white space and comments
WS: [ \t\r\n] -> skip;
LINECOMMENT: '//' .*? '\r'? '\n' -> skip;
BLOCKCOMMENT: '/*' .*? '*/' -> skip;

/*===-------------------------------------------===*/
/* Syntax rules                                    */
/*===-------------------------------------------===*/

module: (global | func)+;

global: GLOBALID BIND (GLOBAL | CONST) btype initValue;

btype: INT | FLOAT;

initValue:
	constant						# scalarInitValue
	| LBRACE (constant+)? RBRACE	# arrayInitValue;

constant: GLOBALID | number;

number: ILITERAL | FLITERAL;

func: funcType GLOBALID LPAREN typeList? RPAREN funcBody;

funcType: VOID | INT | FLOAT;

typeList: btype (COMMA btype)*;

funcBody: LBRACE block+ RBRACE;

block: BLOCKID paramList? COLON instructionList;

paramList: LPAREN param (COMMA param)* RPAREN;

param: btype COLON LOCALID;

instructionList: (instruction SEMICOLON)+;

instruction:
	LOCALID BIND NOT value																# notInst
	| LOCALID BIND AND value COMMA value												# andInst
	| LOCALID BIND OR value COMMA value													# orInst
	| LOCALID BIND ADD value COMMA value												# addInst
	| LOCALID BIND SUB value COMMA value												# subInst
	| LOCALID BIND MUL value COMMA value												# mulInst
	| LOCALID BIND DIV value COMMA value												# divInst
	| LOCALID BIND REM value COMMA value												# divInst
	| LOCALID BIND NEG value															# negInst
	| LOCALID BIND CMPEQ value COMMA value												# cmpeqInst
	| LOCALID BIND CMPNE value COMMA value												# cmpneInst
	| LOCALID BIND CMPLT value COMMA value												# cmpltInst
	| LOCALID BIND CMPGT value COMMA value												# cmpgtInst
	| LOCALID BIND CMPLE value COMMA value												# cmpleInst
	| LOCALID BIND CMPGE value COMMA value												# cmpgeInst
	| LOCALID BIND FADD value COMMA value												# faddInst
	| LOCALID BIND FSUB value COMMA value												# fsubInst
	| LOCALID BIND FMUL value COMMA value												# fmulInst
	| LOCALID BIND FDIV value COMMA value												# fdivInst
	| LOCALID BIND FREM value COMMA value												# fdivInst
	| LOCALID BIND FNEG value															# fnegInst
	| LOCALID BIND FCMPEQ value COMMA value												# fcmpeqInst
	| LOCALID BIND FCMPNE value COMMA value												# fcmpneInst
	| LOCALID BIND FCMPLT value COMMA value												# fcmpltInst
	| LOCALID BIND FCMPGT value COMMA value												# fcmpgtInst
	| LOCALID BIND FCMPLE value COMMA value												# fcmpleInst
	| LOCALID BIND FCMPGE value COMMA value												# fcmpgeInst
	| LOCALID BIND FTOI value															# ftoiInst
	| LOCALID BIND ITOF value															# itofInst
	| LOCALID BIND ALLOCA btype COMMA value?											# allocaInst
	| LOCALID BIND LOAD btype value?													# loadInst
	| STORE value COMMA value															# storeInst
	| (LOCALID BIND)? CALL GLOBALID LPAREN argList? RPAREN								# callInst
	| BR BLOCKID LPAREN argList? RPAREN													# brInst
	| CONDBR value BLOCKID COMMA LPAREN argList? RPAREN COMMA LPAREN argList? RPAREN	# condbrInst
	| RETURN value?																		# returnInst;

value: LOCALID | GLOBALID | BLOCKID | number | string;

argList: value (COMMA value)*;

string: STRING;
