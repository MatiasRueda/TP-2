#include "lista.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


typedef void (*destruir_dato_t) (void*);

typedef struct nodo nodo_t;

struct nodo {
    void* dato;
    struct nodo* prox;
};

struct lista {
    struct nodo* prim;
    struct nodo* ult;
    size_t largo;
};

lista_t *lista_crear(void) {
    lista_t* nueva_lista = malloc(sizeof(lista_t));
    if (nueva_lista == NULL) {
        return NULL;
    }
    nueva_lista->prim = NULL;
    nueva_lista->ult = NULL;
    nueva_lista->largo = 0;
    return nueva_lista;
}

bool lista_esta_vacia(const lista_t *lista) {
    return lista->largo == 0;
}

size_t lista_largo(const lista_t *lista) {
    return lista->largo;
}

bool lista_insertar_primero(lista_t *lista, void *dato) {
    nodo_t* nuevo_nodo = malloc(sizeof(nodo_t));
    if (nuevo_nodo == NULL) { return false; }
    nuevo_nodo->dato = dato;
    nuevo_nodo->prox = lista->prim;
    if (lista_esta_vacia(lista)) {
        lista->ult = nuevo_nodo;
    }
    lista->prim = nuevo_nodo;
    lista->largo++;
    return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato) {
    nodo_t* nuevo_nodo = malloc(sizeof(nodo_t));
    if (nuevo_nodo == NULL) { return false; }
    nuevo_nodo->dato = dato;
    nuevo_nodo->prox = NULL;
    if (lista_esta_vacia(lista)) {
        lista->prim = nuevo_nodo;
    } else {
        lista->ult->prox = nuevo_nodo;
    }
    lista->ult = nuevo_nodo;
    lista->largo++;
    return true;
}

void *lista_borrar_primero(lista_t *lista) {
    if (lista_esta_vacia(lista)) { return NULL; }
    nodo_t* eliminado = lista->prim;
    lista->prim = lista->prim->prox;
    if (lista_largo(lista) == 1) {
        lista->ult = NULL;
    }
    void* dato = eliminado->dato;
    free(eliminado);
    lista->largo--;
    return dato;
}

void *lista_ver_primero(const lista_t *lista) {
    return (lista_esta_vacia(lista)) ? NULL : lista->prim->dato;
}

void *lista_ver_ultimo(const lista_t* lista) {
    return (lista_esta_vacia(lista)) ? NULL : lista->ult->dato;
}

void lista_destruir(lista_t *lista, destruir_dato_t destruir_dato) {
    while (!lista_esta_vacia(lista)) {
        void* dato = lista_borrar_primero(lista);
        if (destruir_dato) {
            destruir_dato(dato);
        }
    }
    free(lista);
}


/********************
 * ITERADOR EXTERNO *
*********************/

struct lista_iter {
    nodo_t* ant;
    nodo_t* act;
    lista_t* lista;
};

lista_iter_t* lista_iter_crear(lista_t *lista) {
    lista_iter_t* lista_iter = malloc(sizeof(lista_iter_t));

    if (lista_iter == NULL) {
        return NULL;
    } 

    lista_iter->lista = lista;
    lista_iter->ant = NULL;
    lista_iter->act = lista->prim;
    
    return lista_iter;
}

bool lista_iter_al_final(const lista_iter_t *iter) {
    if (lista_esta_vacia(iter->lista)) {
        return true;
    }
    return (iter->act == NULL) ? true : false;
}

bool lista_iter_avanzar(lista_iter_t *iter) {
    if (lista_iter_al_final(iter)) {
        return false;
    }
    iter->ant = iter->act;
    iter->act = iter->act->prox;
    return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter) {
    return (lista_iter_al_final(iter)) ? NULL : iter->act->dato ;
}


void lista_iter_destruir(lista_iter_t *iter) {
    free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato) {
    nodo_t* nuevo_nodo = malloc(sizeof(nodo_t));
    if (nuevo_nodo == NULL) { return false; }

    nuevo_nodo->dato = dato;
    nuevo_nodo->prox = iter->act;
    if (iter->act == NULL) {
        iter->lista->ult = nuevo_nodo;
    }
    if (iter->ant == NULL) {
        iter->lista->prim = nuevo_nodo;
    } else {
        iter->ant->prox = nuevo_nodo;
    }

    iter->act = nuevo_nodo;
    iter->lista->largo++;
    return true;
}

void *lista_iter_borrar(lista_iter_t *iter) {
    if (lista_iter_al_final(iter)) { return NULL; }
    nodo_t* eliminado = iter->act;
    void* dato = eliminado->dato;

    if (iter->ant != NULL) {
        iter->ant->prox = iter->act->prox;
    }
    iter->act = iter->act->prox;
    if (iter->act == NULL) {
        iter->lista->ult = iter->ant;
    }
    if (eliminado == iter->lista->prim) {
        iter->lista->prim = iter->act;
    }

    free(eliminado);
    iter->lista->largo--;
    return dato;
}

/********************
 * ITERADOR INTERNO *
*********************/

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra) {
    nodo_t* act = lista->prim;
    while (act != NULL) {
        void* dato_act = act->dato;
        if (!visitar(dato_act, extra)) {
            return;
        }
        act = act->prox;
    }
}
