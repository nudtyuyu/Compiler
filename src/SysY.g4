grammar SysY;

/*===-------------------------------------------===*/
/* Lexer rules                                     */
/*===-------------------------------------------===*/

Comma: ',';
Semicolon: ';';
LeftBracket: '(';
RightBracket: ')';
LeftCurlyBracket: '{';
RightCurlyBracket: '}';
LeftSquareBracket: '[';
RightSquareBracket: ']';
Dot: '.';

fragment Decimal: [0-9];
fragment Octal: [0-7];
fragment Heximal: [0-9a-fA-F];
fragment NonZeroDecimal: [1-9];
fragment IdentifierNonDigit:[a-zA-Z_];
fragment Sign: [+-];
fragment ExpSign: [Ee];
fragment Digit: [0-9];
fragment HexPrefix: '0x' | '0X';

IntConst: NonZeroDecimal Decimal* | '0'
        | '0' Octal+
		| ('0x' | '0X') Heximal+;
FloatConst: ((((Digit+)? '.' Digit+) | (Digit+ '.')) 
	    (ExpSign Sign? Digit+)?)
	  | (Digit+ ExpSign Sign? Digit+)
	  | (HexPrefix 
	    (((Heximal+)? '.' Heximal+) | (Heximal+ '.'))
	    'P' Sign? Digit+)
	  | (HexPrefix
	    Heximal+
	    'P' Sign? Digit+);

Const: 'const';
Int: 'int';
Float: 'float';
Return: 'return';
Void: 'void';
If: 'if';
Else: 'else';
While: 'while';
Break: 'break';
Continue: 'continue';
Identifier: IdentifierNonDigit (IdentifierNonDigit | Decimal)*;
Add: '+';
Sub: '-';
Mul: '*';
Div: '/';
Mod: '%';
Equal: '==';
NonEqual: '!=';
LessThan: '<';
GreaterThan: '>';
LessEqual: '<=';
GreaterEqual: '>=';
Not: '!';
And: '&&';
Or: '||';
Assign: '=';


fragment ESC: '\\"' | '\\\\';

/*WS: [ \t\r\n] -> channel(HIDDEN);*/
WS: [ \t\r\n] -> skip;

LINE_COMMENT: '//' .*? '\r'? '\n' -> skip;
COMMENT: '/*' .*? '*/' -> skip;

/*===-------------------------------------------===*/
/* Syntax rules                                    */
/*===-------------------------------------------===*/


compUnit: (decl | funcDef)+ EOF;
decl: constDecl | varDecl;
constDecl: Const bType constDef (Comma constDef)* Semicolon;
bType: Int | Float;
constDef: Identifier (LeftSquareBracket constExp RightSquareBracket)* Assign constInitVal;
constInitVal: constExp | LeftCurlyBracket (constInitVal (Comma constInitVal)*)? RightCurlyBracket;
varDecl: bType varDef (Comma varDef)* Semicolon;
varDef: Identifier (LeftSquareBracket constExp RightSquareBracket)* 
	| Identifier (LeftSquareBracket constExp RightSquareBracket)* Assign initVal;
initVal: exp | LeftCurlyBracket (initVal (Comma initVal)*)? RightCurlyBracket;
funcDef: funcType Identifier LeftBracket (funcFParams)? RightBracket block;
funcType: Void | Int | Float;
funcFParams: funcFParam (Comma funcFParam)*;
funcFParam: bType Identifier 
	    (LeftSquareBracket RightSquareBracket(LeftSquareBracket exp RightSquareBracket)*)?;
block: LeftCurlyBracket (blockItem)* RightCurlyBracket;
blockItem: decl | stmt;
stmt: lVal'='exp Semicolon | (exp)? Semicolon | block
     |If LeftBracket cond RightBracket stmt (Else stmt)?
     |While LeftBracket cond RightBracket stmt
     |Break Semicolon | Continue Semicolon
     |Return (exp)? Semicolon;
exp: addExp;
cond: lOrExp;
lVal: Identifier (LeftSquareBracket exp RightSquareBracket)*;
primaryExp: LeftBracket exp RightBracket | lVal | number;
number: IntConst | FloatConst;
unaryExp: primaryExp | Identifier LeftBracket (funcRParams)? RightBracket
	 |unaryOp unaryExp;
unaryOp: Add | Sub | Not;
funcRParams: exp (Comma exp)*;
mulExp: unaryExp | mulExp (Mul | Div | Mod) unaryExp;
addExp: mulExp | addExp (Add | Sub) mulExp;
relExp: addExp | relExp 
        (LessThan | GreaterThan | LessEqual | GreaterEqual) addExp;
eqExp: relExp | eqExp (Equal | NonEqual) relExp;
lAndExp: eqExp | lAndExp And eqExp;
lOrExp: lAndExp | lOrExp Or lAndExp;
constExp: addExp;
