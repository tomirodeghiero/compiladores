#ifndef AST_H
#define AST_H

typedef struct Nodo Nodo;

typedef enum {
    NODO_PROG,
    NODO_DECL,
    NODO_SENT,
    NODO_ASSIGN,
    NODO_RETURN,
    NODO_OP,
    NODO_INT,
    NODO_BOOL,
    NODO_ID
} TipoNodo;

typedef enum {
    TOP_SUMA,
    TOP_RESTA,
    TOP_MULT,
    TOP_DIV,
    TOP_ASSIGN,
    TOP_IGUAL,
    TOP_OR,
    TOP_AND,
    TOP_MAYOR,
    TOP_MENOR
} TipoOP;

struct Nodo {
    TipoNodo tipo;
    union {
        int val_int;
        int val_bool;
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
};

Nodo *nodo_ID(char *nombre);
Nodo *nodo_int(int val_int);
Nodo *nodo_bool(int val_bool);
Nodo *nodo_opBin(TipoOP op, Nodo *izq, Nodo *der);
Nodo *nodo_assign(char *id, Nodo *expr);
Nodo *nodo_return(Nodo *expr);

void imprimir_nodo(Nodo *nodo, int n);
void nodo_libre(Nodo *nodo);

#endif
