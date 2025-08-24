%{
#include <stdlib.h>
#include <stdio.h>

extern int yylex(void);
extern void yyerror(const char *s);
%}

%token INT BOOL VOID RETURN ID OP_RESTA OP_SUMA PARA PARC LLAA LLAC CORA CORC OP_IGUAL OP_MULT PYC NUMERO MAIN COMA
%token OP_MAYOR OP_MENOR OP_AND OP_OR OP_DIV TRUE FALSE OP_ASIGN

%left OP_OR
%left OP_AND
%nonassoc OP_IGUAL OP_MAYOR OP_MENOR
%left OP_SUMA OP_RESTA
%left OP_MULT OP_DIV

%%

prog: TIPOM MAIN PARA PARC LLAA CODIGO LLAC { printf("No hay errores\n"); }
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
    | ID OP_ASIGN E
    | ID OP_ASIGN EB
    ;

TIPO: INT
    | BOOL
    ;

SENTENCIA: ID OP_ASIGN E PYC
    | ID OP_ASIGN EB PYC
    | RETURN PYC
    | RETURN E PYC
    | RETURN EB PYC
    ;

E: E OP_SUMA E
    | E OP_RESTA E
    | E OP_MULT E
    | E OP_DIV E
    | PARA E PARC
    | ID
    | NUMERO
    ;

EB: EB OP_OR EB
    | EB OP_AND EB
    | E OP_IGUAL E
    | E OP_MAYOR E
    | E OP_MENOR E
    | PARA EB PARC
    | TRUE
    | FALSE
    ;

%%

int main(int argc, char *argv[]) {
    extern FILE *yyin;
    ++argv, --argc;
    if (argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;

    yyparse();
    return 0;
}
