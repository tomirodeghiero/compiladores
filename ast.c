#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Nodo *nodo_ID(char *nombre) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_PROG;
    n->nombre = strdup(nombre);

    return n;
}

Nodo *nodo_int(int val_int) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_INT;
    n->val_int = val_int;

    return n;
}

Nodo *nodo_bool(int val_bool) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_BOOL;
    n->val_int = val_bool;

    return n;
}

Nodo *nodo_opBin(TipoOP op, Nodo *izq, Nodo *der) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_OP;
    n->opBinaria.op = op;
    n->opBinaria.izq = izq;
    n->opBinaria.der = der;

    return n;
}

Nodo *nodo_assign(char *id, Nodo *expr) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_ASSIGN;
    n->assign.id = id;
    n->assign.expr = expr;

    return n;
}

Nodo *nodo_return(Nodo *expr) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_RETURN;
    n->ret_expr = expr;

    return n;
}

void nodoLibre(Nodo *n) {
    if (!n) return;

    switch (n->tipo) {
        case NODO_OP:
            nodoLibre(n->opBinaria.izq);
            nodoLibre(n->opBinaria.der);
            break;
        case NODO_ASSIGN:
            free(n->assign.id);
            nodoLibre(n->assign.expr);
            break;
        case NODO_RETURN:
            nodoLibre(n->ret_expr);
            break;
        case NODO_PROG:
        case NODO_DECL:
        case NODO_SENT:
        case NODO_INT:
        case NODO_BOOL:
            break;
    }

    if (n->tipo == NODO_PROG || n->tipo == NODO_DECL || n->tipo == NODO_SENT) {
        if (n->nombre) free(n->nombre);
    }

    free(n);
}

static void print_indent(int n) {
    for (int i = 0; i < n; i++) printf("  ");
}

void imprimirNodo(Nodo *n, int ind) {
    if (!n) return;

    print_indent(ind);

    switch (n->tipo) {
        case NODO_INT:
            printf("INT(%d)\n", n->val_int);
            break;
        case NODO_BOOL:
            printf("BOOL(%s)\n", n->val_bool ? "true" : "false");
            break;
        case NODO_ASSIGN:
            printf("ASSIGN(%s)\n", n->assign.id);
            imprimirNodo(n->assign.expr, ind + 1);
            break;
        case NODO_RETURN:
            printf("RETURN\n");
            imprimirNodo(n->ret_expr, ind + 1);
            break;
        case NODO_OP: {
            const char *ops[] = {
                "+", "-", "*", "/", "=", "==", "||", "&&", ">", "<"
            };
            printf("OP(%s)\n", ops[n->opBinaria.op]);
            imprimirNodo(n->opBinaria.izq, ind + 1);
            imprimirNodo(n->opBinaria.der, ind + 1);
            break;
        }
        case NODO_PROG:
            printf("PROG(%s)\n", n->nombre ? n->nombre : "");
            break;
        case NODO_DECL:
            printf("DECL(%s)\n", n->nombre ? n->nombre : "");
            break;
        case NODO_SENT:
            printf("SENT(%s)\n", n->nombre ? n->nombre : "");
            break;
    }
}