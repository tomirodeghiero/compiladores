%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "calc-sintaxis.tab.h"

extern void yyerror(const char *s);
%}

%option noyywrap
%option yylineno

letra [a-zA-Z]
digito [0-9]
numero {digito}+
id {letra}({letra}|{digito})*

%%

"int"                   { printf("INT : %s\n", yytext); return INT; }
"bool"                  { printf("BOOL : %s\n", yytext); return BOOL; }
"void"                  { printf("VOID : %s\n", yytext); return VOID; }
"return"                { printf("RETURN : %s\n", yytext); return RETURN; }
"main"                  { printf("MAIN : %s\n", yytext); return MAIN; }
{id}                    { printf("ID : %s\n", yytext); return ID; }
"-"                     { printf("MENOS : %s\n", yytext); return MENOS; }
"+"                     { printf("MAS : %s\n", yytext); return MAS; }
"("                     { printf("PARA : %s\n", yytext); return PARA; }
")"                     { printf("PARC : %s\n", yytext); return PARC; }
"{"                     { printf("LLAA : %s\n", yytext); return LLAA; }
"}"                     { printf("LLAC : %s\n", yytext); return LLAC; }
"["                     { printf("CORA : %s\n", yytext); return CORA; }
"]"                     { printf("CORC : %s\n", yytext); return CORC; }
"="                     { printf("IGUAL : %s\n", yytext); return IGUAL; }
"*"                     { printf("MULT : %s\n", yytext); return MULT; }
";"                     { printf("PYC : %s\n", yytext); return PYC; }
","                     { printf("COMA : %s\n", yytext); return COMA; }
{numero}                { printf("NUMERO : %s\n", yytext); return NUMERO; }
[ \t\n]+                ; /* Ignore whitespace */
.                       ; /* Ignore all other characters */

%%

void yyerror(const char *s) {
    fprintf(stderr, "-> ERROR Sintactico en la linea: %d: %s\n", yylineno, s);
}