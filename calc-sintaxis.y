%{

#include <stdlib.h>
#include <stdio.h>

%}
 
%token INT
%token BOOL
%token VOID
%token RETURN
%token ID
%token MENOS
%token MAS
%token PARA
%token PARC
%token LLAA
%token LLAC
%token CORA
%token CORC
%token IGUAL
%token MULT
%token PYC
%token NUMERO
%token MAIN
%token COMA
    
%left MAS MENOS 
%left MULT
%right IGUAL
 
%%
 
prog: TIPOM MAIN PARA PARC LLAA CODIGO LLAC { printf("No hay errores \n"); } 
    ;

TIPOM: INT
    | BOOL
    | VOID
    ;

CODIGO: /* empty */
    | DECLARACION CODIGO
    | SENTENCIA CODIGO
    ;

DECLARACION: TIPO VARS PYC
    ;

VARS: VAR
    | VAR COMA VARS
    ;

VAR: ID
    | ID IGUAL E
    ;

TIPO: INT
    | BOOL
    ;

SENTENCIA: ID IGUAL E PYC
    | RETURN E PYC
    | RETURN ID PYC
    | RETURN PYC
    ;

E:  E MAS E
    | E MULT E
    | E MENOS E
    | PARA E PARC
    | ID
    | NUMERO
    ;

%%


