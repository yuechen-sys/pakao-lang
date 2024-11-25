%option noyywrap

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%{
#include <iostream>
#include <cstdio>
#include "node.hpp"
#include "semic_grammar.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext)
#define TOKEN(t) ( yylval.token = t)

extern "C" int yylex();
int column = 0;
void count();


int check_type()
{
/*
* pseudo code --- this is what it should check
*
*	if (yytext == type_name)
*		return(TYPE_NAME);
*
*	return(IDENTIFIER);
*/

/*
*	it actually will only return IDENTIFIER
*/

	return(IDENTIFIER);
}
%}

%%

"break"			{ count(); return(BREAK); }
"case"			{ count(); return(CASE); }
"char"			{ count(); return(CHAR); }
"continue"		{ count(); return(CONTINUE); }
"default"		{ count(); return(DEFAULT); }
"do"			{ count(); return(DO); }
"double"		{ count(); return(DOUBLE); }
"else"			{ count(); return(ELSE); }
"float"			{ count(); return(FLOAT); }
"for"			{ count(); return(FOR); }
"if"			{ count(); return(IF); }
"int"			{ count(); return(INT); }
"long"			{ count(); return(LONG); }
"return"		{ count(); return(RETURN); }
"short"			{ count(); return(SHORT); }
"signed"		{ count(); return(SIGNED); }
"sizeof"		{ count(); return(SIZEOF); }
"switch"		{ count(); return(SWITCH); }
"unsigned"		{ count(); return(UNSIGNED); }
"void"			{ count(); return(VOID); }
"while"			{ count(); return(WHILE); }

{L}({L}|{D})*		{ SAVE_TOKEN; count(); return(IDENTIFIER); }

0[xX]{H}+{IS}?		{ count(); return(CONSTANT); }
0{D}+{IS}?		{ count(); return(CONSTANT); }
{D}+{IS}?		{ count(); return(CONSTANT); }
L?'(\\.|[^\\'])+'	{ count(); return(CONSTANT); }

{D}+{E}{FS}?		{ count(); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ count(); return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ count(); return(CONSTANT); }

L?\"(\\.|[^\\"])*\"	{ count(); return(STRING_LITERAL); }

">>="			{ count(); return(TOKEN(RIGHT_ASSIGN)); }
"<<="			{ count(); return(TOKEN(LEFT_ASSIGN)); }
"+="			{ count(); return(TOKEN(ADD_ASSIGN)); }
"-="			{ count(); return(TOKEN(SUB_ASSIGN)); }
"*="			{ count(); return(TOKEN(MUL_ASSIGN)); }
"/="			{ count(); return(TOKEN(DIV_ASSIGN)); }
"%="			{ count(); return(TOKEN(MOD_ASSIGN)); }
"&="			{ count(); return(TOKEN(AND_ASSIGN)); }
"^="			{ count(); return(TOKEN(XOR_ASSIGN)); }
"|="			{ count(); return(TOKEN(OR_ASSIGN)); }
">>"			{ count(); return(TOKEN(RIGHT_OP)); }
"<<"			{ count(); return(TOKEN(LEFT_OP)); }
"++"			{ count(); return(TOKEN(INC_OP)); }
"--"			{ count(); return(TOKEN(DEC_OP)); }
"->"			{ count(); return(TOKEN(PTR_OP)); }
"&&"			{ count(); return(TOKEN(AND_OP)); }
"||"			{ count(); return(TOKEN(OR_OP)); }
"<="			{ count(); return(TOKEN(LE_OP)); }
">="			{ count(); return(TOKEN(GE_OP)); }
"=="			{ count(); return(TOKEN(EQ_OP)); }
"!="			{ count(); return(TOKEN(NE_OP)); }
";"			{ count(); return(';'); }
("{"|"<%")		{ count(); return('{'); }
("}"|"%>")		{ count(); return('}'); }
","			{ count(); return(','); }
":"			{ count(); return(':'); }
"="			{ count(); return('='); }
"("			{ count(); return('('); }
")"			{ count(); return(')'); }
("["|"<:")		{ count(); return('['); }
("]"|":>")		{ count(); return(']'); }
"."			{ count(); return('.'); }
"&"			{ count(); return('&'); }
"!"			{ count(); return('!'); }
"~"			{ count(); return('~'); }
"-"			{ count(); return(TOKEN(NEG_OP)); }
"+"			{ count(); return(TOKEN(ADD_OP)); }
"*"			{ count(); return(TOKEN(MUL_OP)); }
"/"			{ count(); return(TOKEN(DIV_OP)); }
"%"			{ count(); return(TOKEN(MOD_OP)); }
"<"			{ count(); return(TOKEN(LT_OP)); }
">"			{ count(); return(TOKEN(GT_OP)); }
"^"			{ count(); return('^'); }
"|"			{ count(); return('|'); }
"?"			{ count(); return('?'); }

[ \t\v\n\f]		{ count(); }
.			{ /* ignore bad characters */ }

%%

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;
	ECHO;
}
