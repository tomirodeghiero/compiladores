/* ast.c - Implementación completa del AST, tabla de símbolos y evaluador */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "ast.h"

/* ------------------ Constructores y destructor de nodos ------------------ */

static int contador_nodos = 0;

Nodo *nodo_ID(char *nombre) {
    if (!nombre) {
        fprintf(stderr, "Error: nodo_ID recibió nombre NULL\n");
        exit(EXIT_FAILURE);
    }
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    n->tipo = NODO_ID;
    n->nombre = strdup(nombre);
    return n;
}

Nodo *nodo_int(int val_int) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    n->tipo = NODO_INT;
    n->val_int = val_int;
    return n;
}

Nodo *nodo_bool(int val_bool) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    n->tipo = NODO_BOOL;
    n->val_bool = val_bool;
    return n;
}

Nodo *nodo_opBin(TipoOP op, Nodo *izq, Nodo *der) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    n->tipo = NODO_OP;
    n->opBinaria.op = op;
    n->opBinaria.izq = izq;
    n->opBinaria.der = der;
    return n;
}

Nodo *nodo_assign(char *id, Nodo *expr) {
    if (!id) {
        fprintf(stderr, "Error: nodo_assign recibió id NULL\n");
        exit(EXIT_FAILURE);
    }
    if (!expr) {
        fprintf(stderr, "Error: nodo_assign recibió expr NULL\n");
        exit(EXIT_FAILURE);
    }
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    n->tipo = NODO_ASSIGN;
    n->assign.id = strdup(id);
    n->assign.expr = expr;
    return n;
}

Nodo *nodo_return(Nodo *expr) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    n->tipo = NODO_RETURN;
    n->ret_expr = expr;
    return n;
}

Nodo *nodo_seq(Nodo *primero, Nodo *resto) {
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    n->tipo = NODO_SEQ;
    n->opBinaria.izq = primero;
    n->opBinaria.der = resto;
    return n;
}

Nodo *nodo_decl(char *id, Nodo *expr) {
    if (!id) {
        fprintf(stderr, "Error: nodo_decl recibió id NULL\n");
        exit(EXIT_FAILURE);
    }
    Nodo *n = malloc(sizeof(Nodo));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
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

/* ------------------ Exportar DOT para Graphviz ------------------ */

static void escribir_dot_rec(FILE *f, Nodo *n) {
    if (!n) return;

    int mi_id = contador_nodos++;

    switch (n->tipo) {
        case NODO_SEQ:
            fprintf(f, "    nodo%d [label=\"SEQ\", style=filled, fillcolor=lightgray];\n", mi_id);
            if (n->opBinaria.izq) {
                fprintf(f, "    nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->opBinaria.izq);
            }
            if (n->opBinaria.der) {
                fprintf(f, "    nodo%d -> nodo%d [style=dashed];\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->opBinaria.der);
            }
            break;
        case NODO_DECL:
            fprintf(f, "    nodo%d [label=\"DECL %s\"];\n", mi_id, n->assign.id);
            if (n->assign.expr) {
                fprintf(f, "    nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->assign.expr);
            }
            break;
        case NODO_OP:
            fprintf(f, "    nodo%d [label=\"%s\"];\n", mi_id,
                    n->opBinaria.op == TOP_SUMA ? "+" :
                    n->opBinaria.op == TOP_RESTA ? "-" :
                    n->opBinaria.op == TOP_MULT ? "*" :
                    n->opBinaria.op == TOP_DIV ? "/" :
                    n->opBinaria.op == TOP_IGUAL ? "==" :
                    n->opBinaria.op == TOP_OR ? "or" :
                    n->opBinaria.op == TOP_AND ? "and" :
                    n->opBinaria.op == TOP_MAYOR ? ">" :
                    n->opBinaria.op == TOP_MENOR ? "<" : "?");
            if (n->opBinaria.izq) {
                fprintf(f, "    nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->opBinaria.izq);
            }
            if (n->opBinaria.der) {
                fprintf(f, "    nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->opBinaria.der);
            }
            break;
        case NODO_ID:
            fprintf(f, "    nodo%d [label=\"%s\"];\n", mi_id, n->nombre);
            break;
        case NODO_INT:
            fprintf(f, "    nodo%d [label=\"%d\"];\n", mi_id, n->val_int);
            break;
        case NODO_RETURN:
            fprintf(f, "    nodo%d [label=\"RETURN\", style=filled, fillcolor=lightpink];\n", mi_id);
            if (n->ret_expr) {
                fprintf(f, "    nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->ret_expr);
            }
            break;
        case NODO_ASSIGN:
            fprintf(f, "    nodo%d [label=\"ASSIGN %s\"];\n", mi_id, n->assign.id);
            if (n->assign.expr) {
                fprintf(f, "    nodo%d -> nodo%d;\n", mi_id, contador_nodos);
                escribir_dot_rec(f, n->assign.expr);
            }
            break;
        default:
            fprintf(stderr, "Warning: tipo de nodo desconocido %d en escribir_dot_rec\n", n->tipo);
            break;
    }
}

void exportar_dot(Nodo *nodo, const char *filename) {
    char dot_file[PATH_MAX];
    char png_file[PATH_MAX];
    char command[PATH_MAX * 2 + 80];

    snprintf(dot_file, sizeof(dot_file), "%s.dot", filename);
    snprintf(png_file, sizeof(png_file), "%s.png", filename);

    FILE *f = fopen(dot_file, "w");
    if (!f) {
        perror("Error al crear archivo DOT");
        return;
    }

    fprintf(f, "digraph AST {\n");
    fprintf(f, "    node [shape=box, fontname=\"Arial\"];\n");

    contador_nodos = 0;
    escribir_dot_rec(f, nodo);

    fprintf(f, "}\n");
    fclose(f);

    snprintf(command, sizeof(command), "dot -Tpng %s -o %s 2>/dev/null", dot_file, png_file);
    system(command);
}

/* ------------------ Tabla de símbolos simple ------------------ */

#define MAX_SIMBOLOS 1024

typedef struct {
    char *id;           /* nombre */
    int valor;          /* valor (int o bool 0/1) */
    int es_bool;        /* 0=int, 1=bool (para futura extensión) */
    int inicializado;   /* 0=no inicializado, 1=inicializado */
} Simbolo;

static Simbolo tabla[MAX_SIMBOLOS];
static int tabla_size = 0;

static Simbolo* buscar_simbolo(const char *id) {
    for (int i = 0; i < tabla_size; ++i) {
        if (strcmp(tabla[i].id, id) == 0) return &tabla[i];
    }
    return NULL;
}

static void declarar_simbolo(const char *id, int valor, int es_bool, int inicializado) {
    if (buscar_simbolo(id)) {
        fprintf(stderr, "Error: variable '%s' ya declarada\n", id);
        exit(EXIT_FAILURE);
    }
    if (tabla_size >= MAX_SIMBOLOS) {
        fprintf(stderr, "Error: tabla de símbolos llena\n");
        exit(EXIT_FAILURE);
    }
    tabla[tabla_size].id = strdup(id);
    tabla[tabla_size].valor = valor;
    tabla[tabla_size].es_bool = es_bool;
    tabla[tabla_size].inicializado = inicializado;
    tabla_size++;
}

static void asignar_simbolo(const char *id, int valor) {
    Simbolo *s = buscar_simbolo(id);
    if (!s) {
        fprintf(stderr, "Error: asignación a variable no declarada '%s'\n", id);
        exit(EXIT_FAILURE);
    }
    s->valor = valor;
    s->inicializado = 1;
}

static void liberar_tabla_simbolos(void) {
    for (int i = 0; i < tabla_size; ++i) {
        free(tabla[i].id);
    }
    tabla_size = 0;
}

/* ------------------ Evaluador / intérprete ------------------ */

int eval_nodo(Nodo *n) {
    if (!n) return 0;

    switch (n->tipo) {
        case NODO_INT:
            return n->val_int;
        case NODO_BOOL:
            return n->val_bool ? 1 : 0;
        case NODO_ID: {
            Simbolo *s = buscar_simbolo(n->nombre);
            if (!s) {
                fprintf(stderr, "Error: variable '%s' no declarada\n", n->nombre);
                exit(EXIT_FAILURE);
            }
            if (!s->inicializado) {
                fprintf(stderr, "Error: variable '%s' usada sin inicializar\n", n->nombre);
                exit(EXIT_FAILURE);
            }
            return s->valor;
        }
        case NODO_DECL: {
            int inicializado = 0;
            int valor = 0;
            if (n->assign.expr) {
                valor = eval_nodo(n->assign.expr);
                inicializado = 1;
            }
            declarar_simbolo(n->assign.id, valor, 0, inicializado);
            return 0;
        }
        case NODO_ASSIGN: {
            Simbolo *s = buscar_simbolo(n->assign.id);
            if (!s) {
                fprintf(stderr, "Error: variable '%s' no declarada (assign)\n", n->assign.id);
                exit(EXIT_FAILURE);
            }
            int valor = eval_nodo(n->assign.expr);
            asignar_simbolo(n->assign.id, valor);
            return valor;
        }
        case NODO_OP: {
            int izq = eval_nodo(n->opBinaria.izq);
            int der = eval_nodo(n->opBinaria.der);
            switch (n->opBinaria.op) {
                case TOP_SUMA:  return izq + der;
                case TOP_RESTA: return izq - der;
                case TOP_MULT:  return izq * der;
                case TOP_DIV:
                    if (der == 0) {
                        fprintf(stderr, "Error: división por cero\n");
                        exit(EXIT_FAILURE);
                    }
                    return izq / der;
                case TOP_IGUAL: return izq == der;
                case TOP_OR:    return (izq != 0) || (der != 0);
                case TOP_AND:   return (izq != 0) && (der != 0);
                case TOP_MAYOR: return izq > der;
                case TOP_MENOR: return izq < der;
                default:
                    fprintf(stderr, "Error: operación binaria desconocida %d\n", n->opBinaria.op);
                    exit(EXIT_FAILURE);
            }
        }
        case NODO_SEQ: {
            if (n->opBinaria.izq) eval_nodo(n->opBinaria.izq);
            if (n->opBinaria.der) return eval_nodo(n->opBinaria.der);
            return 0;
        }
        case NODO_RETURN: {
            if (!n->ret_expr) return 0;
            return eval_nodo(n->ret_expr);
        }
        case NODO_PROG:
        case NODO_SENT:
            if (n->opBinaria.izq) return eval_nodo(n->opBinaria.izq);
            return 0;
        default:
            fprintf(stderr, "Error: eval_nodo no soporta tipo de nodo %d\n", n->tipo);
            exit(EXIT_FAILURE);
    }
}

/* ------------------ Salida bonita en consola ------------------ */

static void print_divider(const char *title) {
    printf("┌──────────────────────────────────────────────────┐\n");
    printf("│ %-48s │\n", title);
    printf("└──────────────────────────────────────────────────┘\n");
}

void print_ast(Nodo *n, int ind) {
    if (!n) return;

    for (int i = 0; i < ind; i++) printf("    ");

    switch (n->tipo) {
        case NODO_SEQ:
            printf("%-10s\n", "SEQ");
            print_ast(n->opBinaria.izq, ind + 1);
            print_ast(n->opBinaria.der, ind + 1);
            break;
        case NODO_DECL:
            printf("%-10s%s", "DECL", n->assign.id);
            if (n->assign.expr) {
                printf(" = ");
                print_ast(n->assign.expr, 0);
            }
            printf("\n");
            break;
        case NODO_OP:
            switch (n->opBinaria.op) {
                case TOP_SUMA:  printf("%-10s", "SUMA"); break;
                case TOP_RESTA: printf("%-10s", "RESTA"); break;
                case TOP_MULT:  printf("%-10s", "MULT"); break;
                case TOP_DIV:   printf("%-10s", "DIV"); break;
                case TOP_IGUAL: printf("%-10s", "IGUAL"); break;
                case TOP_OR:    printf("%-10s", "OR"); break;
                case TOP_AND:   printf("%-10s", "AND"); break;
                case TOP_MAYOR: printf("%-10s", "MAYOR"); break;
                case TOP_MENOR: printf("%-10s", "MENOR"); break;
                default:        printf("%-10s", "OP_UNKNOWN"); break;
            }
            printf("(");
            print_ast(n->opBinaria.izq, 0);
            printf(", ");
            print_ast(n->opBinaria.der, 0);
            printf(")\n");
            break;
        case NODO_ID:
            printf("%-10s%s\n", "ID", n->nombre);
            break;
        case NODO_INT:
            printf("%-10s%d\n", "INT", n->val_int);
            break;
        case NODO_RETURN:
            printf("%-10s\n", "RETURN");
            if (n->ret_expr)
                print_ast(n->ret_expr, ind + 1);
            break;
        case NODO_BOOL:
            printf("%-10s%s\n", "BOOL", n->val_bool ? "true" : "false");
            break;
        case NODO_ASSIGN:
            printf("%-10s%s = ", "ASSIGN", n->assign.id);
            print_ast(n->assign.expr, 0);
            printf("\n");
            break;
        case NODO_PROG:
            printf("%-10s\n", "PROG");
            print_ast(n->opBinaria.izq, ind + 1);
            break;
        case NODO_SENT:
            printf("%-10s\n", "SENT");
            print_ast(n->opBinaria.izq, ind + 1);
            break;
        default:
            printf("%-10s%d\n", "UNKNOWN", n->tipo);
            break;
    }
}

void imprimir_nodo(Nodo *nodo, int indent) {
    if (!nodo) return;

    for (int i = 0; i < indent; i++) printf("    ");

    switch (nodo->tipo) {
        case NODO_SEQ:
            printf("%-10s\n", "SEQ");
            imprimir_nodo(nodo->opBinaria.izq, indent + 1);
            imprimir_nodo(nodo->opBinaria.der, indent + 1);
            break;
        case NODO_DECL:
            printf("%-10s%s\n", "DECL", nodo->assign.id);
            if (nodo->assign.expr)
                imprimir_nodo(nodo->assign.expr, indent + 1);
            break;
        case NODO_OP:
            printf("%-10s\n",
                   nodo->opBinaria.op == TOP_SUMA ? "SUMA" :
                   nodo->opBinaria.op == TOP_RESTA ? "RESTA" :
                   nodo->opBinaria.op == TOP_MULT ? "MULT" :
                   nodo->opBinaria.op == TOP_DIV ? "DIV" :
                   nodo->opBinaria.op == TOP_IGUAL ? "IGUAL" :
                   nodo->opBinaria.op == TOP_OR ? "OR" :
                   nodo->opBinaria.op == TOP_AND ? "AND" :
                   nodo->opBinaria.op == TOP_MAYOR ? "MAYOR" :
                   nodo->opBinaria.op == TOP_MENOR ? "MENOR" : "OP_UNKNOWN");
            imprimir_nodo(nodo->opBinaria.izq, indent + 1);
            imprimir_nodo(nodo->opBinaria.der, indent + 1);
            break;
        case NODO_ID:
            printf("%-10s%s\n", "ID", nodo->nombre);
            break;
        case NODO_INT:
            printf("%-10s%d\n", "INT", nodo->val_int);
            break;
        case NODO_BOOL:
            printf("%-10s%s\n", "BOOL", nodo->val_bool ? "true" : "false");
            break;
        case NODO_ASSIGN:
            printf("%-10s%s\n", "ASSIGN", nodo->assign.id);
            imprimir_nodo(nodo->assign.expr, indent + 1);
            break;
        case NODO_RETURN:
            printf("%-10s\n", "RETURN");
            if (nodo->ret_expr)
                imprimir_nodo(nodo->ret_expr, indent + 1);
            break;
        case NODO_PROG:
            printf("%-10s\n", "PROG");
            imprimir_nodo(nodo->opBinaria.izq, indent + 1);
            break;
        case NODO_SENT:
            printf("%-10s\n", "SENT");
            imprimir_nodo(nodo->opBinaria.izq, indent + 1);
            break;
        default:
            printf("%-10s%d\n", "UNKNOWN", nodo->tipo);
            break;
    }
}

/* ------------------ Intérprete / wrapper de ejecución ------------------ */

int interpretar_programa(Nodo *programa) {
    liberar_tabla_simbolos();

    printf("\n");
    print_divider("EJECUCION DEL PROGRAMA");

    printf("Árbol Sintáctico Abstracto (AST):\n");
    printf("----------------------------------------\n");
    if (programa)
        imprimir_nodo(programa, 0);   // Recorrido dinámico con sangrías
    else
        printf("(AST vacío)\n");
    printf("\n");

    int resultado = 0;
    if (programa)
        resultado = eval_nodo(programa);

    printf("Tabla de Símbolos Final:\n");
    printf("----------------------------------------\n");
    if (tabla_size > 0) {
        // Calculamos ancho dinámico según los nombres de variables
        int max_len = 8; // mínimo ancho para "Variable"
        for (int i = 0; i < tabla_size; ++i) {
            int len = (int)strlen(tabla[i].id);
            if (len > max_len) max_len = len;
        }
        printf("%-*s %-10s %-15s\n", max_len, "Variable", "Valor", "Estado");
        printf("----------------------------------------\n");
        for (int i = 0; i < tabla_size; ++i) {
            printf("%-*s %-10d %-15s\n",
                   max_len, tabla[i].id, tabla[i].valor,
                   tabla[i].inicializado ? "Inicializado" : "No inicializado");
        }
    } else {
        printf("(Tabla vacía)\n");
    }
    printf("\n");

    print_divider("FIN DE LA EJECUCION");

    return resultado;
}

void ast_liberar_recursos(void) {
    liberar_tabla_simbolos();
}