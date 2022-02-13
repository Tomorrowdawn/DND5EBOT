%option noyywrap
%{
#include<stdio.h>
#include<stdlib.h>
#include "dice.h"
#include "parser.tab.h"
void pass_node(const char *text){
    yylval.node = Dnode_new(text);
}

extern void yyerror(const char* s);

%}

delim [\t ]
ws {delim}+
letter	[A-Za-z_]
digit		[0-9]
INTEGER {digit}+

%%
{ws} {}
{INTEGER} {pass_node(yytext);return number;}
\+ 	{pass_node(yytext); return ADD;}
\- {pass_node(yytext);return SUB;}
\* {pass_node(yytext);return MUL;}
"/" {pass_node(yytext);return DIV;}
[(] {return LPAREN;}
[)] {return RPAREN;}
\n {return END;}
gep {pass_node(yytext);return GEP;}
[Kk] {pass_node(yytext);return K;}
[Ll] {pass_node(yytext);return L;}
[dD] {return DICE;}
. {yyerror(yytext);}
%%