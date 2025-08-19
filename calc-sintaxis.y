%{
#include <stdlib.h>
#include <stdio.h>

extern int yylex(void);
extern void yyerror(const char *s);
%}

%token INT BOOL VOID RETURN ID MENOS MAS PARA PARC LLAA LLAC CORA CORC IGUAL MULT PYC NUMERO MAIN COMA

%left MAS MENOS
%left MULT
%right IGUAL

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

E: E MAS E
 | E MENOS E
 | E MULT E
 | PARA E PARC
 | ID
 | NUMERO
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