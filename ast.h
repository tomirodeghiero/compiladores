#ifndef AST_H
#define AST_H

typedef enum {
    NODO_PROG,
    NODO_DECL,
    NODO_SENT,
    NODO_ASSIGN,
    NODO_RETURN,
    NODO_OP,
    NODO_INT,
    NODO_BOOL
} TipoNodo;

typedef enum {
    OP_SUMA,
    OP_RESTA,
    OP_MULT,
    OP_DIV,
    OP_ASSIGN,
    OP_IGUAL,
    OP_OR,
    OP_AND,
    OP_MAYOR,
    OP_MENOR
} TipoOP;

typedef struct Nodo {
    TipoNodo tipo;

    union {
        int *id;
        int val_int;
        int val_bool; // 0-1
        char *nombre;

        struct {
            struct Nodo *izq;
            struct Nodo *der;
            TipoOP op;
        } opBinaria;

        struct {
            char *id;
            struct Nodo *expr;
        } assign;

        struct Nodo *ret_expr;
    };
} Nodo;

Nodo *nodo_ID(char *name);
Nodo *nodo_int(int val_int);
Nodo *nodo_bool(int val_bool);
Nodo *nodo_opBin(TipoOP op, Nodo *izq, Nodo *der);
Nodo *nodo_assign(char *id, Nodo *expr);
Nodo *nodo_return(Nodo *expr);

void imprimir_nodo(Nodo *nodo, int n);
void nodo_libre(Nodo *nodo);

#endif