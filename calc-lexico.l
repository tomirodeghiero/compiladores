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
"&&"                    { printf("AND : %s\n", yytext); return OP_AND; }
"||"                    { printf("OR : %s\n", yytext); return OP_OR; }
"=="                    { printf("IGUAL : %s\n", yytext); return OP_IGUAL; }
"true"                  { printf("TRUE : %s\n", yytext); return TRUE; }
"false"                 { printf("FALSE : %s\n", yytext); return FALSE; }
{id}                    { printf("ID : %s\n", yytext); return ID; }
"-"                     { printf("MENOS : %s\n", yytext); return OP_RESTA; }
"+"                     { printf("MAS : %s\n", yytext); return OP_SUMA; }
"("                     { printf("PARA : %s\n", yytext); return PARA; }
")"                     { printf("PARC : %s\n", yytext); return PARC; }
"{"                     { printf("LLAA : %s\n", yytext); return LLAA; }
"}"                     { printf("LLAC : %s\n", yytext); return LLAC; }
"["                     { printf("CORA : %s\n", yytext); return CORA; }
"]"                     { printf("CORC : %s\n", yytext); return CORC; }
"="                     { printf("ASIGN : %s\n", yytext); return OP_ASIGN; }
"*"                     { printf("OP_MULT : %s\n", yytext); return OP_MULT; }
"/"                     { printf("DIV : %s\n", yytext); return OP_DIV; }
";"                     { printf("PYC : %s\n", yytext); return PYC; }
","                     { printf("COMA : %s\n", yytext); return COMA; }
">"                     { printf("MAYOR : %s\n", yytext); return OP_MAYOR; }
"<"                     { printf("MENOR : %s\n", yytext); return OP_MENOR; }
{numero}                { printf("NUMERO : %s\n", yytext); return NUMERO; }
[ \t\n]+                ; /* Ignore whitespace */
.                       ; /* Ignore all other characters */

%%

void yyerror(const char *s) {
    fprintf(stderr, "-> ERROR Sintactico en la linea: %d: %s\n", yylineno, s);
}