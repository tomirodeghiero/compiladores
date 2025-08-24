%code requires {
    #include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex(void);
extern void yyerror(const char *s);
%}

%union {
    Nodo *nodo;
    char *str;
    int num;
}

%token <str> ID
%token <num> NUMERO
%token INT BOOL VOID TRUE FALSE RETURN MAIN
%token PARA PARC LLAA LLAC CORA CORC PYC COMA
%token OP_RESTA OP_SUMA OP_MAYOR OP_MENOR OP_AND OP_OR OP_DIV OP_MULT OP_IGUAL OP_ASIGN

%type <nodo> prog CODIGO SENTENCIA E EB

%left OP_OR
%left OP_AND
%nonassoc OP_IGUAL OP_MAYOR OP_MENOR
%left OP_SUMA OP_RESTA
%left OP_MULT OP_DIV

%%

prog: TIPOM MAIN PARA PARC LLAA CODIGO LLAC {
        printf("No hay errores\n");
        imprimir_nodo($6, 0);
        nodo_libre($6);
    };

TIPOM: INT
     | BOOL
     | VOID
     ;

CODIGO: /* vacío */        { $$ = NULL; }
      | DECLARACION CODIGO { $$ = $2; }
      | SENTENCIA CODIGO   { $$ = $1; }
      ;

DECLARACION: TIPO VARS PYC ;

VARS: VAR
    | VAR COMA VARS
    ;

VAR: ID
   | ID OP_ASIGN E
   | ID OP_ASIGN EB
   ;

TIPO: INT
    | BOOL
    ;

SENTENCIA: ID OP_ASIGN E PYC    { $$ = nodo_assign($1, $3); }
         | ID OP_ASIGN EB PYC   { $$ = nodo_assign($1, $3); }
         | RETURN PYC           { $$ = nodo_return(NULL); }
         | RETURN E PYC         { $$ = nodo_return($2); }
         | RETURN EB PYC        { $$ = nodo_return($2); }
         ;

E: E OP_SUMA E      { $$ = nodo_opBin(TOP_SUMA, $1, $3); }
 | E OP_RESTA E     { $$ = nodo_opBin(TOP_RESTA, $1, $3); }
 | E OP_MULT E      { $$ = nodo_opBin(TOP_MULT, $1, $3); }
 | E OP_DIV E       { $$ = nodo_opBin(TOP_DIV, $1, $3); }
 | PARA E PARC      { $$ = $2; }
 | ID               { $$ = nodo_ID($1); }
 | NUMERO           { $$ = nodo_int($1); }
 ;

EB: EB OP_OR EB     { $$ = nodo_opBin(TOP_OR, $1, $3); }
  | EB OP_AND EB    { $$ = nodo_opBin(TOP_AND, $1, $3); }
  | E OP_IGUAL E    { $$ = nodo_opBin(TOP_IGUAL, $1, $3); }
  | E OP_MAYOR E    { $$ = nodo_opBin(TOP_MAYOR, $1, $3); }
  | E OP_MENOR E    { $$ = nodo_opBin(TOP_MENOR, $1, $3); }
  | PARA EB PARC    { $$ = $2; }
  | TRUE            { $$ = nodo_bool(1); }
  | FALSE           { $$ = nodo_bool(0); }
  ;

%%

int main(int argc, char *argv[]) {
    extern FILE *yyin;
    ++argv, --argc;

    if (argc > 0) yyin = fopen(argv[0], "r");
    else yyin = stdin;

    yyparse();
    return 0;
}
