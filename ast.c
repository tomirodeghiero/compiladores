#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ast.h"

static int contador_nodos = 0;

Nodo *nodo_ID(char *nombre) {
    if (!nombre) {
        fprintf(stderr, "Error: nodo_ID recibió nombre NULL\n");
        exit(1);
    }
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_ID;
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
    n->val_bool = val_bool;

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
    if (!id) {
        fprintf(stderr, "Error: nodo_assign recibió id NULL\n");
        exit(1);
    }
    if (!expr) {
        fprintf(stderr, "Error: nodo_assign recibió expr NULL\n");
        exit(1);
    }
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_ASSIGN;
    n->assign.id = strdup(id);
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

Nodo *nodo_seq(Nodo *primero, Nodo *resto) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_SEQ;
    n->opBinaria.izq = primero;
    n->opBinaria.der = resto;

    return n;
}

Nodo *nodo_decl(char *id, Nodo *expr) {
    if (!id) {
        fprintf(stderr, "Error: nodo_decl recibió id NULL\n");
        exit(1);
    }
    // if (!expr) {
    //     fprintf(stderr, "Error: nodo_decl recibió expr NULL\n");
    //     exit(1);
    // }
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) { perror("malloc"); exit(1); }

    n->tipo = NODO_DECL;
    n->assign.id = strdup(id);
    n->assign.expr = expr;

    return n;
}

void nodo_libre(Nodo *n) {
    if (!n) return;

    switch (n->tipo) {
        case NODO_SEQ:
        case NODO_OP:
            nodo_libre(n->opBinaria.izq);
            nodo_libre(n->opBinaria.der);
            break;
        case NODO_ASSIGN:
        case NODO_DECL:
            free(n->assign.id);
            nodo_libre(n->assign.expr);
            break;
        case NODO_RETURN:
            nodo_libre(n->ret_expr);
            break;
        case NODO_ID:
            free(n->nombre);
            break;
        default:
            break;
    }
    free(n);
}

/* IMPRIMIR TREE AST EN CONSOLA */
static void indentaciones(int n) { while (n--) putchar(' '); }

void imprimir_nodo(Nodo *n, int ind) {
    if (!n) return;

    indentaciones(ind);
    
    switch (n->tipo) {
        case NODO_SEQ:
            printf("SEQ\n");
            imprimir_nodo(n->opBinaria.izq, ind + 1);
            imprimir_nodo(n->opBinaria.der, ind + 1);
            break;
        case NODO_DECL:
            printf("DECL(%s", n->assign.id);
            if (n->assign.expr) {
                printf(" = ");
                imprimir_nodo(n->assign.expr, 0);
            }
            printf(")\n");
            break;
        case NODO_OP:
            switch (n->opBinaria.op) {
                case TOP_SUMA: printf("SUMA("); break;
                case TOP_RESTA: printf("RESTA("); break;
                case TOP_MULT: printf("MULT("); break;
                case TOP_DIV: printf("DIV("); break;
                case TOP_IGUAL: printf("IGUAL("); break;
                case TOP_OR: printf("OR("); break;
                case TOP_AND: printf("AND("); break;
                case TOP_MAYOR: printf("MAYOR("); break;
                case TOP_MENOR: printf("MENOR("); break;
                default: printf("OP_DESCONOCIDO("); break;
            }
            imprimir_nodo(n->opBinaria.izq, 0);
            printf(", ");
            imprimir_nodo(n->opBinaria.der, 0);
            printf(")");
            break;
        case NODO_ID:
            printf("ID(%s)", n->nombre);
            break;
        case NODO_INT:
            printf("%d", n->val_int);
            break;
        case NODO_RETURN:
            printf("RETURN\n");
            if (n->ret_expr)
                imprimir_nodo(n->ret_expr, ind + 1);
            break;
            case NODO_BOOL:
            printf("%s", n->val_bool ? "true" : "false");
            break;
        case NODO_ASSIGN:
            printf("ASSIGN(%s = ", n->assign.id);
            imprimir_nodo(n->assign.expr, 0);
            printf(")\n");
            break;
        case NODO_PROG:
            printf("PROG\n");
            imprimir_nodo(n->opBinaria.izq, ind + 1);
            break;
        case NODO_SENT:
            printf("SENT\n");
            imprimir_nodo(n->opBinaria.izq, ind + 1);
            break;        
    }
}

/* CREAR UNA IMAGEN PNG CON EL ARBOL AST */
static void escribir_dot_rec(FILE *f, Nodo *n) {
    if (!n) return;
    
    int mi_id = contador_nodos++;
    
    switch (n->tipo) {
        case NODO_SEQ:
            fprintf(f, "  nodo%d [label=\"SEQ\", style=filled, fillcolor=lightgray];\n", mi_id);
            if (n->opBinaria.izq) {
                fprintf(f, "  nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->opBinaria.izq);
            }
            if (n->opBinaria.der) {
                fprintf(f, "  nodo%d -> nodo%d [style=dashed];\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->opBinaria.der);
            }
            break;
            
        case NODO_DECL:
            fprintf(f, "  nodo%d [label=\"DECL %s\"];\n", mi_id, n->assign.id);
            if (n->assign.expr) {
                fprintf(f, "  nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->assign.expr);
            }
            break;
            
        case NODO_OP:
            fprintf(f, "  nodo%d [label=\"%s\"];\n", mi_id, 
                n->opBinaria.op == TOP_SUMA ? "+" :
                n->opBinaria.op == TOP_RESTA ? "-" :
                n->opBinaria.op == TOP_MULT ? "*" :
                n->opBinaria.op == TOP_DIV ? "/" : "?");
            if (n->opBinaria.izq) {
                fprintf(f, "  nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->opBinaria.izq);
            }
            if (n->opBinaria.der) {
                fprintf(f, "  nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->opBinaria.der);
            }
            break;
            
        case NODO_ID:
            fprintf(f, "  nodo%d [label=\"%s\"];\n", mi_id, n->nombre);
            break;
            
        case NODO_INT:
            fprintf(f, "  nodo%d [label=\"%d\"];\n", mi_id, n->val_int);
            break;
            
        case NODO_RETURN:
            fprintf(f, "  nodo%d [label=\"RETURN\", style=filled, fillcolor=lightpink];\n", mi_id);
            if (n->ret_expr) {
                fprintf(f, "  nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->ret_expr);
            }
            break;
        case NODO_ASSIGN:
            fprintf(f, "  nodo%d [label=\"ASSIGN %s\"];\n", mi_id, n->assign.id);
            if (n->assign.expr) {
                fprintf(f, "  nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->assign.expr);
            }
            break;
        default: 
            fprintf(stderr, "Error: tipo de nodo desconocido %d\n", n->tipo);
            break;
    }
}

void exportar_dot(Nodo *nodo, const char *filename) {
    char dot_file[PATH_MAX];
    char png_file[PATH_MAX];
    char command[PATH_MAX * 2 + 50];
    
    snprintf(dot_file, sizeof(dot_file), "%s.dot", filename);
    snprintf(png_file, sizeof(png_file), "%s.png", filename);
    
    FILE *f = fopen(dot_file, "w");
    if (!f) {
        perror("Error al crear archivo DOT");
        return;
    }
    
    fprintf(f, "digraph AST {\n");
    fprintf(f, "  node [shape=box, fontname=\"Arial\"];\n");
    
    contador_nodos = 0;
    escribir_dot_rec(f, nodo);
    
    fprintf(f, "}\n");
    fclose(f);
    
    snprintf(command, sizeof(command), "dot -Tpng %s -o %s", dot_file, png_file);
    system(command);
}