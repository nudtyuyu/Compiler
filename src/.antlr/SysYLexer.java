// Generated from /home/rhein-e/workspace/course/并行编译/Compiler/src/SysY.g4 by ANTLR 4.9.2
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class SysYLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.9.2", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		Comma=1, Semicolon=2, LeftBracket=3, RightBracket=4, LeftCurlyBracket=5, 
		RightCurlyBracket=6, LeftSquareBracket=7, RightSquareBracket=8, Dot=9, 
		IntConst=10, FloatConst=11, Const=12, Int=13, Float=14, Return=15, Void=16, 
		If=17, Else=18, While=19, Break=20, Continue=21, Identifier=22, Add=23, 
		Sub=24, Mul=25, Div=26, Mod=27, Equal=28, NonEqual=29, LessThan=30, GreaterThan=31, 
		LessEqual=32, GreaterEqual=33, Not=34, And=35, Or=36, Assign=37, String=38, 
		WS=39, LINE_COMMENT=40, COMMENT=41;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"Comma", "Semicolon", "LeftBracket", "RightBracket", "LeftCurlyBracket", 
			"RightCurlyBracket", "LeftSquareBracket", "RightSquareBracket", "Dot", 
			"Decimal", "Octal", "Heximal", "NonZeroDecimal", "IdentifierNonDigit", 
			"Sign", "ExpSign", "Digit", "HexPrefix", "IntConst", "FloatConst", "Const", 
			"Int", "Float", "Return", "Void", "If", "Else", "While", "Break", "Continue", 
			"Identifier", "Add", "Sub", "Mul", "Div", "Mod", "Equal", "NonEqual", 
			"LessThan", "GreaterThan", "LessEqual", "GreaterEqual", "Not", "And", 
			"Or", "Assign", "String", "ESC", "WS", "LINE_COMMENT", "COMMENT"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "','", "';'", "'('", "')'", "'{'", "'}'", "'['", "']'", "'.'", 
			null, null, "'const'", "'int'", "'float'", "'return'", "'void'", "'if'", 
			"'else'", "'while'", "'break'", "'continue'", null, "'+'", "'-'", "'*'", 
			"'/'", "'%'", "'=='", "'!='", "'<'", "'>'", "'<='", "'>='", "'!'", "'&&'", 
			"'||'", "'='"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, "Comma", "Semicolon", "LeftBracket", "RightBracket", "LeftCurlyBracket", 
			"RightCurlyBracket", "LeftSquareBracket", "RightSquareBracket", "Dot", 
			"IntConst", "FloatConst", "Const", "Int", "Float", "Return", "Void", 
			"If", "Else", "While", "Break", "Continue", "Identifier", "Add", "Sub", 
			"Mul", "Div", "Mod", "Equal", "NonEqual", "LessThan", "GreaterThan", 
			"LessEqual", "GreaterEqual", "Not", "And", "Or", "Assign", "String", 
			"WS", "LINE_COMMENT", "COMMENT"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}


	public SysYLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "SysY.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2+\u01a3\b\1\4\2\t"+
		"\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13"+
		"\t\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22\t\22"+
		"\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\4\30\t\30\4\31\t\31"+
		"\4\32\t\32\4\33\t\33\4\34\t\34\4\35\t\35\4\36\t\36\4\37\t\37\4 \t \4!"+
		"\t!\4\"\t\"\4#\t#\4$\t$\4%\t%\4&\t&\4\'\t\'\4(\t(\4)\t)\4*\t*\4+\t+\4"+
		",\t,\4-\t-\4.\t.\4/\t/\4\60\t\60\4\61\t\61\4\62\t\62\4\63\t\63\4\64\t"+
		"\64\3\2\3\2\3\3\3\3\3\4\3\4\3\5\3\5\3\6\3\6\3\7\3\7\3\b\3\b\3\t\3\t\3"+
		"\n\3\n\3\13\3\13\3\f\3\f\3\r\3\r\3\16\3\16\3\17\3\17\3\20\3\20\3\21\3"+
		"\21\3\22\3\22\3\23\3\23\3\23\3\23\5\23\u0090\n\23\3\24\3\24\7\24\u0094"+
		"\n\24\f\24\16\24\u0097\13\24\3\24\3\24\3\24\6\24\u009c\n\24\r\24\16\24"+
		"\u009d\3\24\3\24\3\24\3\24\5\24\u00a4\n\24\3\24\6\24\u00a7\n\24\r\24\16"+
		"\24\u00a8\5\24\u00ab\n\24\3\25\6\25\u00ae\n\25\r\25\16\25\u00af\5\25\u00b2"+
		"\n\25\3\25\3\25\6\25\u00b6\n\25\r\25\16\25\u00b7\3\25\6\25\u00bb\n\25"+
		"\r\25\16\25\u00bc\3\25\3\25\5\25\u00c1\n\25\3\25\3\25\5\25\u00c5\n\25"+
		"\3\25\6\25\u00c8\n\25\r\25\16\25\u00c9\5\25\u00cc\n\25\3\25\6\25\u00cf"+
		"\n\25\r\25\16\25\u00d0\3\25\3\25\5\25\u00d5\n\25\3\25\6\25\u00d8\n\25"+
		"\r\25\16\25\u00d9\3\25\3\25\6\25\u00de\n\25\r\25\16\25\u00df\5\25\u00e2"+
		"\n\25\3\25\3\25\6\25\u00e6\n\25\r\25\16\25\u00e7\3\25\6\25\u00eb\n\25"+
		"\r\25\16\25\u00ec\3\25\3\25\5\25\u00f1\n\25\3\25\3\25\5\25\u00f5\n\25"+
		"\3\25\6\25\u00f8\n\25\r\25\16\25\u00f9\3\25\3\25\6\25\u00fe\n\25\r\25"+
		"\16\25\u00ff\3\25\3\25\5\25\u0104\n\25\3\25\6\25\u0107\n\25\r\25\16\25"+
		"\u0108\5\25\u010b\n\25\3\26\3\26\3\26\3\26\3\26\3\26\3\27\3\27\3\27\3"+
		"\27\3\30\3\30\3\30\3\30\3\30\3\30\3\31\3\31\3\31\3\31\3\31\3\31\3\31\3"+
		"\32\3\32\3\32\3\32\3\32\3\33\3\33\3\33\3\34\3\34\3\34\3\34\3\34\3\35\3"+
		"\35\3\35\3\35\3\35\3\35\3\36\3\36\3\36\3\36\3\36\3\36\3\37\3\37\3\37\3"+
		"\37\3\37\3\37\3\37\3\37\3\37\3 \3 \3 \7 \u0149\n \f \16 \u014c\13 \3!"+
		"\3!\3\"\3\"\3#\3#\3$\3$\3%\3%\3&\3&\3&\3\'\3\'\3\'\3(\3(\3)\3)\3*\3*\3"+
		"*\3+\3+\3+\3,\3,\3-\3-\3-\3.\3.\3.\3/\3/\3\60\3\60\3\60\7\60\u0175\n\60"+
		"\f\60\16\60\u0178\13\60\3\60\3\60\3\61\3\61\3\61\3\61\5\61\u0180\n\61"+
		"\3\62\3\62\3\62\3\62\3\63\3\63\3\63\3\63\7\63\u018a\n\63\f\63\16\63\u018d"+
		"\13\63\3\63\5\63\u0190\n\63\3\63\3\63\3\63\3\63\3\64\3\64\3\64\3\64\7"+
		"\64\u019a\n\64\f\64\16\64\u019d\13\64\3\64\3\64\3\64\3\64\3\64\5\u0176"+
		"\u018b\u019b\2\65\3\3\5\4\7\5\t\6\13\7\r\b\17\t\21\n\23\13\25\2\27\2\31"+
		"\2\33\2\35\2\37\2!\2#\2%\2\'\f)\r+\16-\17/\20\61\21\63\22\65\23\67\24"+
		"9\25;\26=\27?\30A\31C\32E\33G\34I\35K\36M\37O Q!S\"U#W$Y%[&]\'_(a\2c)"+
		"e*g+\3\2\n\3\2\62;\3\2\629\5\2\62;CHch\3\2\63;\5\2C\\aac|\4\2--//\4\2"+
		"GGgg\5\2\13\f\17\17\"\"\2\u01c0\2\3\3\2\2\2\2\5\3\2\2\2\2\7\3\2\2\2\2"+
		"\t\3\2\2\2\2\13\3\2\2\2\2\r\3\2\2\2\2\17\3\2\2\2\2\21\3\2\2\2\2\23\3\2"+
		"\2\2\2\'\3\2\2\2\2)\3\2\2\2\2+\3\2\2\2\2-\3\2\2\2\2/\3\2\2\2\2\61\3\2"+
		"\2\2\2\63\3\2\2\2\2\65\3\2\2\2\2\67\3\2\2\2\29\3\2\2\2\2;\3\2\2\2\2=\3"+
		"\2\2\2\2?\3\2\2\2\2A\3\2\2\2\2C\3\2\2\2\2E\3\2\2\2\2G\3\2\2\2\2I\3\2\2"+
		"\2\2K\3\2\2\2\2M\3\2\2\2\2O\3\2\2\2\2Q\3\2\2\2\2S\3\2\2\2\2U\3\2\2\2\2"+
		"W\3\2\2\2\2Y\3\2\2\2\2[\3\2\2\2\2]\3\2\2\2\2_\3\2\2\2\2c\3\2\2\2\2e\3"+
		"\2\2\2\2g\3\2\2\2\3i\3\2\2\2\5k\3\2\2\2\7m\3\2\2\2\to\3\2\2\2\13q\3\2"+
		"\2\2\rs\3\2\2\2\17u\3\2\2\2\21w\3\2\2\2\23y\3\2\2\2\25{\3\2\2\2\27}\3"+
		"\2\2\2\31\177\3\2\2\2\33\u0081\3\2\2\2\35\u0083\3\2\2\2\37\u0085\3\2\2"+
		"\2!\u0087\3\2\2\2#\u0089\3\2\2\2%\u008f\3\2\2\2\'\u00aa\3\2\2\2)\u010a"+
		"\3\2\2\2+\u010c\3\2\2\2-\u0112\3\2\2\2/\u0116\3\2\2\2\61\u011c\3\2\2\2"+
		"\63\u0123\3\2\2\2\65\u0128\3\2\2\2\67\u012b\3\2\2\29\u0130\3\2\2\2;\u0136"+
		"\3\2\2\2=\u013c\3\2\2\2?\u0145\3\2\2\2A\u014d\3\2\2\2C\u014f\3\2\2\2E"+
		"\u0151\3\2\2\2G\u0153\3\2\2\2I\u0155\3\2\2\2K\u0157\3\2\2\2M\u015a\3\2"+
		"\2\2O\u015d\3\2\2\2Q\u015f\3\2\2\2S\u0161\3\2\2\2U\u0164\3\2\2\2W\u0167"+
		"\3\2\2\2Y\u0169\3\2\2\2[\u016c\3\2\2\2]\u016f\3\2\2\2_\u0171\3\2\2\2a"+
		"\u017f\3\2\2\2c\u0181\3\2\2\2e\u0185\3\2\2\2g\u0195\3\2\2\2ij\7.\2\2j"+
		"\4\3\2\2\2kl\7=\2\2l\6\3\2\2\2mn\7*\2\2n\b\3\2\2\2op\7+\2\2p\n\3\2\2\2"+
		"qr\7}\2\2r\f\3\2\2\2st\7\177\2\2t\16\3\2\2\2uv\7]\2\2v\20\3\2\2\2wx\7"+
		"_\2\2x\22\3\2\2\2yz\7\60\2\2z\24\3\2\2\2{|\t\2\2\2|\26\3\2\2\2}~\t\3\2"+
		"\2~\30\3\2\2\2\177\u0080\t\4\2\2\u0080\32\3\2\2\2\u0081\u0082\t\5\2\2"+
		"\u0082\34\3\2\2\2\u0083\u0084\t\6\2\2\u0084\36\3\2\2\2\u0085\u0086\t\7"+
		"\2\2\u0086 \3\2\2\2\u0087\u0088\t\b\2\2\u0088\"\3\2\2\2\u0089\u008a\t"+
		"\2\2\2\u008a$\3\2\2\2\u008b\u008c\7\62\2\2\u008c\u0090\7z\2\2\u008d\u008e"+
		"\7\62\2\2\u008e\u0090\7Z\2\2\u008f\u008b\3\2\2\2\u008f\u008d\3\2\2\2\u0090"+
		"&\3\2\2\2\u0091\u0095\5\33\16\2\u0092\u0094\5\25\13\2\u0093\u0092\3\2"+
		"\2\2\u0094\u0097\3\2\2\2\u0095\u0093\3\2\2\2\u0095\u0096\3\2\2\2\u0096"+
		"\u00ab\3\2\2\2\u0097\u0095\3\2\2\2\u0098\u00ab\7\62\2\2\u0099\u009b\7"+
		"\62\2\2\u009a\u009c\5\27\f\2\u009b\u009a\3\2\2\2\u009c\u009d\3\2\2\2\u009d"+
		"\u009b\3\2\2\2\u009d\u009e\3\2\2\2\u009e\u00ab\3\2\2\2\u009f\u00a0\7\62"+
		"\2\2\u00a0\u00a4\7z\2\2\u00a1\u00a2\7\62\2\2\u00a2\u00a4\7Z\2\2\u00a3"+
		"\u009f\3\2\2\2\u00a3\u00a1\3\2\2\2\u00a4\u00a6\3\2\2\2\u00a5\u00a7\5\31"+
		"\r\2\u00a6\u00a5\3\2\2\2\u00a7\u00a8\3\2\2\2\u00a8\u00a6\3\2\2\2\u00a8"+
		"\u00a9\3\2\2\2\u00a9\u00ab\3\2\2\2\u00aa\u0091\3\2\2\2\u00aa\u0098\3\2"+
		"\2\2\u00aa\u0099\3\2\2\2\u00aa\u00a3\3\2\2\2\u00ab(\3\2\2\2\u00ac\u00ae"+
		"\5#\22\2\u00ad\u00ac\3\2\2\2\u00ae\u00af\3\2\2\2\u00af\u00ad\3\2\2\2\u00af"+
		"\u00b0\3\2\2\2\u00b0\u00b2\3\2\2\2\u00b1\u00ad\3\2\2\2\u00b1\u00b2\3\2"+
		"\2\2\u00b2\u00b3\3\2\2\2\u00b3\u00b5\7\60\2\2\u00b4\u00b6\5#\22\2\u00b5"+
		"\u00b4\3\2\2\2\u00b6\u00b7\3\2\2\2\u00b7\u00b5\3\2\2\2\u00b7\u00b8\3\2"+
		"\2\2\u00b8\u00c1\3\2\2\2\u00b9\u00bb\5#\22\2\u00ba\u00b9\3\2\2\2\u00bb"+
		"\u00bc\3\2\2\2\u00bc\u00ba\3\2\2\2\u00bc\u00bd\3\2\2\2\u00bd\u00be\3\2"+
		"\2\2\u00be\u00bf\7\60\2\2\u00bf\u00c1\3\2\2\2\u00c0\u00b1\3\2\2\2\u00c0"+
		"\u00ba\3\2\2\2\u00c1\u00cb\3\2\2\2\u00c2\u00c4\5!\21\2\u00c3\u00c5\5\37"+
		"\20\2\u00c4\u00c3\3\2\2\2\u00c4\u00c5\3\2\2\2\u00c5\u00c7\3\2\2\2\u00c6"+
		"\u00c8\5#\22\2\u00c7\u00c6\3\2\2\2\u00c8\u00c9\3\2\2\2\u00c9\u00c7\3\2"+
		"\2\2\u00c9\u00ca\3\2\2\2\u00ca\u00cc\3\2\2\2\u00cb\u00c2\3\2\2\2\u00cb"+
		"\u00cc\3\2\2\2\u00cc\u010b\3\2\2\2\u00cd\u00cf\5#\22\2\u00ce\u00cd\3\2"+
		"\2\2\u00cf\u00d0\3\2\2\2\u00d0\u00ce\3\2\2\2\u00d0\u00d1\3\2\2\2\u00d1"+
		"\u00d2\3\2\2\2\u00d2\u00d4\5!\21\2\u00d3\u00d5\5\37\20\2\u00d4\u00d3\3"+
		"\2\2\2\u00d4\u00d5\3\2\2\2\u00d5\u00d7\3\2\2\2\u00d6\u00d8\5#\22\2\u00d7"+
		"\u00d6\3\2\2\2\u00d8\u00d9\3\2\2\2\u00d9\u00d7\3\2\2\2\u00d9\u00da\3\2"+
		"\2\2\u00da\u010b\3\2\2\2\u00db\u00f0\5%\23\2\u00dc\u00de\5\31\r\2\u00dd"+
		"\u00dc\3\2\2\2\u00de\u00df\3\2\2\2\u00df\u00dd\3\2\2\2\u00df\u00e0\3\2"+
		"\2\2\u00e0\u00e2\3\2\2\2\u00e1\u00dd\3\2\2\2\u00e1\u00e2\3\2\2\2\u00e2"+
		"\u00e3\3\2\2\2\u00e3\u00e5\7\60\2\2\u00e4\u00e6\5\31\r\2\u00e5\u00e4\3"+
		"\2\2\2\u00e6\u00e7\3\2\2\2\u00e7\u00e5\3\2\2\2\u00e7\u00e8\3\2\2\2\u00e8"+
		"\u00f1\3\2\2\2\u00e9\u00eb\5\31\r\2\u00ea\u00e9\3\2\2\2\u00eb\u00ec\3"+
		"\2\2\2\u00ec\u00ea\3\2\2\2\u00ec\u00ed\3\2\2\2\u00ed\u00ee\3\2\2\2\u00ee"+
		"\u00ef\7\60\2\2\u00ef\u00f1\3\2\2\2\u00f0\u00e1\3\2\2\2\u00f0\u00ea\3"+
		"\2\2\2\u00f1\u00f2\3\2\2\2\u00f2\u00f4\7R\2\2\u00f3\u00f5\5\37\20\2\u00f4"+
		"\u00f3\3\2\2\2\u00f4\u00f5\3\2\2\2\u00f5\u00f7\3\2\2\2\u00f6\u00f8\5#"+
		"\22\2\u00f7\u00f6\3\2\2\2\u00f8\u00f9\3\2\2\2\u00f9\u00f7\3\2\2\2\u00f9"+
		"\u00fa\3\2\2\2\u00fa\u010b\3\2\2\2\u00fb\u00fd\5%\23\2\u00fc\u00fe\5\31"+
		"\r\2\u00fd\u00fc\3\2\2\2\u00fe\u00ff\3\2\2\2\u00ff\u00fd\3\2\2\2\u00ff"+
		"\u0100\3\2\2\2\u0100\u0101\3\2\2\2\u0101\u0103\7R\2\2\u0102\u0104\5\37"+
		"\20\2\u0103\u0102\3\2\2\2\u0103\u0104\3\2\2\2\u0104\u0106\3\2\2\2\u0105"+
		"\u0107\5#\22\2\u0106\u0105\3\2\2\2\u0107\u0108\3\2\2\2\u0108\u0106\3\2"+
		"\2\2\u0108\u0109\3\2\2\2\u0109\u010b\3\2\2\2\u010a\u00c0\3\2\2\2\u010a"+
		"\u00ce\3\2\2\2\u010a\u00db\3\2\2\2\u010a\u00fb\3\2\2\2\u010b*\3\2\2\2"+
		"\u010c\u010d\7e\2\2\u010d\u010e\7q\2\2\u010e\u010f\7p\2\2\u010f\u0110"+
		"\7u\2\2\u0110\u0111\7v\2\2\u0111,\3\2\2\2\u0112\u0113\7k\2\2\u0113\u0114"+
		"\7p\2\2\u0114\u0115\7v\2\2\u0115.\3\2\2\2\u0116\u0117\7h\2\2\u0117\u0118"+
		"\7n\2\2\u0118\u0119\7q\2\2\u0119\u011a\7c\2\2\u011a\u011b\7v\2\2\u011b"+
		"\60\3\2\2\2\u011c\u011d\7t\2\2\u011d\u011e\7g\2\2\u011e\u011f\7v\2\2\u011f"+
		"\u0120\7w\2\2\u0120\u0121\7t\2\2\u0121\u0122\7p\2\2\u0122\62\3\2\2\2\u0123"+
		"\u0124\7x\2\2\u0124\u0125\7q\2\2\u0125\u0126\7k\2\2\u0126\u0127\7f\2\2"+
		"\u0127\64\3\2\2\2\u0128\u0129\7k\2\2\u0129\u012a\7h\2\2\u012a\66\3\2\2"+
		"\2\u012b\u012c\7g\2\2\u012c\u012d\7n\2\2\u012d\u012e\7u\2\2\u012e\u012f"+
		"\7g\2\2\u012f8\3\2\2\2\u0130\u0131\7y\2\2\u0131\u0132\7j\2\2\u0132\u0133"+
		"\7k\2\2\u0133\u0134\7n\2\2\u0134\u0135\7g\2\2\u0135:\3\2\2\2\u0136\u0137"+
		"\7d\2\2\u0137\u0138\7t\2\2\u0138\u0139\7g\2\2\u0139\u013a\7c\2\2\u013a"+
		"\u013b\7m\2\2\u013b<\3\2\2\2\u013c\u013d\7e\2\2\u013d\u013e\7q\2\2\u013e"+
		"\u013f\7p\2\2\u013f\u0140\7v\2\2\u0140\u0141\7k\2\2\u0141\u0142\7p\2\2"+
		"\u0142\u0143\7w\2\2\u0143\u0144\7g\2\2\u0144>\3\2\2\2\u0145\u014a\5\35"+
		"\17\2\u0146\u0149\5\35\17\2\u0147\u0149\5\25\13\2\u0148\u0146\3\2\2\2"+
		"\u0148\u0147\3\2\2\2\u0149\u014c\3\2\2\2\u014a\u0148\3\2\2\2\u014a\u014b"+
		"\3\2\2\2\u014b@\3\2\2\2\u014c\u014a\3\2\2\2\u014d\u014e\7-\2\2\u014eB"+
		"\3\2\2\2\u014f\u0150\7/\2\2\u0150D\3\2\2\2\u0151\u0152\7,\2\2\u0152F\3"+
		"\2\2\2\u0153\u0154\7\61\2\2\u0154H\3\2\2\2\u0155\u0156\7\'\2\2\u0156J"+
		"\3\2\2\2\u0157\u0158\7?\2\2\u0158\u0159\7?\2\2\u0159L\3\2\2\2\u015a\u015b"+
		"\7#\2\2\u015b\u015c\7?\2\2\u015cN\3\2\2\2\u015d\u015e\7>\2\2\u015eP\3"+
		"\2\2\2\u015f\u0160\7@\2\2\u0160R\3\2\2\2\u0161\u0162\7>\2\2\u0162\u0163"+
		"\7?\2\2\u0163T\3\2\2\2\u0164\u0165\7@\2\2\u0165\u0166\7?\2\2\u0166V\3"+
		"\2\2\2\u0167\u0168\7#\2\2\u0168X\3\2\2\2\u0169\u016a\7(\2\2\u016a\u016b"+
		"\7(\2\2\u016bZ\3\2\2\2\u016c\u016d\7~\2\2\u016d\u016e\7~\2\2\u016e\\\3"+
		"\2\2\2\u016f\u0170\7?\2\2\u0170^\3\2\2\2\u0171\u0176\7$\2\2\u0172\u0175"+
		"\5a\61\2\u0173\u0175\13\2\2\2\u0174\u0172\3\2\2\2\u0174\u0173\3\2\2\2"+
		"\u0175\u0178\3\2\2\2\u0176\u0177\3\2\2\2\u0176\u0174\3\2\2\2\u0177\u0179"+
		"\3\2\2\2\u0178\u0176\3\2\2\2\u0179\u017a\7$\2\2\u017a`\3\2\2\2\u017b\u017c"+
		"\7^\2\2\u017c\u0180\7$\2\2\u017d\u017e\7^\2\2\u017e\u0180\7^\2\2\u017f"+
		"\u017b\3\2\2\2\u017f\u017d\3\2\2\2\u0180b\3\2\2\2\u0181\u0182\t\t\2\2"+
		"\u0182\u0183\3\2\2\2\u0183\u0184\b\62\2\2\u0184d\3\2\2\2\u0185\u0186\7"+
		"\61\2\2\u0186\u0187\7\61\2\2\u0187\u018b\3\2\2\2\u0188\u018a\13\2\2\2"+
		"\u0189\u0188\3\2\2\2\u018a\u018d\3\2\2\2\u018b\u018c\3\2\2\2\u018b\u0189"+
		"\3\2\2\2\u018c\u018f\3\2\2\2\u018d\u018b\3\2\2\2\u018e\u0190\7\17\2\2"+
		"\u018f\u018e\3\2\2\2\u018f\u0190\3\2\2\2\u0190\u0191\3\2\2\2\u0191\u0192"+
		"\7\f\2\2\u0192\u0193\3\2\2\2\u0193\u0194\b\63\2\2\u0194f\3\2\2\2\u0195"+
		"\u0196\7\61\2\2\u0196\u0197\7,\2\2\u0197\u019b\3\2\2\2\u0198\u019a\13"+
		"\2\2\2\u0199\u0198\3\2\2\2\u019a\u019d\3\2\2\2\u019b\u019c\3\2\2\2\u019b"+
		"\u0199\3\2\2\2\u019c\u019e\3\2\2\2\u019d\u019b\3\2\2\2\u019e\u019f\7,"+
		"\2\2\u019f\u01a0\7\61\2\2\u01a0\u01a1\3\2\2\2\u01a1\u01a2\b\64\2\2\u01a2"+
		"h\3\2\2\2\'\2\u008f\u0095\u009d\u00a3\u00a8\u00aa\u00af\u00b1\u00b7\u00bc"+
		"\u00c0\u00c4\u00c9\u00cb\u00d0\u00d4\u00d9\u00df\u00e1\u00e7\u00ec\u00f0"+
		"\u00f4\u00f9\u00ff\u0103\u0108\u010a\u0148\u014a\u0174\u0176\u017f\u018b"+
		"\u018f\u019b\3\b\2\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}