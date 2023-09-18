#define _POSIX_C_SOURCE 200809L // strdup
#include "abb.h"
#include "pila.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct abb {
    nodo_t* raiz;
    size_t cantidad;
    abb_destruir_dato_t destruir_dato;
    abb_comparar_clave_t cmp;
};

struct nodo {
    nodo_t* izq;
    nodo_t* der;
    char* clave;
    void* dato;
};

/* *****************************************************************
 *                        PRIMITIVA NODO                           *
 * *****************************************************************/

nodo_t* nodo_crear(char* clave, void* dato) {
    nodo_t* nodo = malloc(sizeof(nodo_t));
    if (nodo == NULL) {
        return NULL;
    }
    nodo->clave = clave;
    nodo->dato = dato;
    nodo->der = NULL;
    nodo->izq = NULL;
    return nodo;
}

/* *****************************************************************
 *                     FUNCIONES AUXILIARES                        *
 * *****************************************************************/

// Recorre recursivamente entre los nodos de un arbol
// Para devolver el puntero dirigido a donde debería estar el nodo buscado
nodo_t** abb_obtener_p_nodo(nodo_t** p_nodo_act, const char *clave, abb_comparar_clave_t cmp) {
    nodo_t* nodo = (*p_nodo_act);
    if (nodo == NULL) {
        return p_nodo_act;
    }
    if( cmp(clave, nodo->clave) == 0 ) {
        return p_nodo_act;
    }
    return ( cmp(clave, nodo->clave) < 0 ) ? abb_obtener_p_nodo(&(nodo->izq), clave, cmp) : abb_obtener_p_nodo(&(nodo->der), clave, cmp);
}

// Aplica la funcion para buscar punteros
// Y devuelve el nodo al que apunta -para mas placer-
nodo_t* abb_obtener_nodo(const abb_t *arbol, const char *clave) {
    return (*abb_obtener_p_nodo( &(((abb_t*)arbol)->raiz), clave, arbol->cmp ));
}

// Busca el puntero dirigido al nodo que esta mas a la izquierda de un nodo dado (nodo_act)
nodo_t** abb_obtener_ext_izq(nodo_t** p_nodo_act) {
    nodo_t* nodo = *p_nodo_act;
    if (nodo->izq ==  NULL) {
        return p_nodo_act;
    }
    return abb_obtener_ext_izq(&(nodo->izq));
}

// Destruye los nodos de forma recursiva
void abb_destruir_nodos(nodo_t* nodo, abb_destruir_dato_t destruir_dato) {
    if (nodo == NULL) {
        return;
    }
    abb_destruir_nodos(nodo->izq, destruir_dato);
    abb_destruir_nodos(nodo->der, destruir_dato);
    if (destruir_dato) {
        destruir_dato(nodo->dato);
    }
    free(nodo->clave);
    free(nodo);
}

// Destruye un nodo que tiene al menos un hijo NULL
// Haciendo que su puntero apunte a otro lado.
// Recibe el puntero al nodo que tiene que ser borrado
// Y devuelve el dato que tenia dicho nodo
void* abb_borrar_nodo(nodo_t** p_nodo_borrado) {
    nodo_t* nodo_borrado = *p_nodo_borrado;
    void* dato = nodo_borrado->dato;
    
    if ((*p_nodo_borrado)->izq == NULL) {
        (*p_nodo_borrado) = (*p_nodo_borrado)->der;    
    } else {
        (*p_nodo_borrado) = (*p_nodo_borrado)->izq;
    }
    free(nodo_borrado->clave);
    free(nodo_borrado);
    return dato;
}

// Funcion que pasa por todos los nodos en in-order , de forma recursiva y le aplica la funcion visitar
bool abb_nodo_in_order(nodo_t* nodo, bool visitar(const char *, void *, void *), void* extra) {
    if (nodo == NULL) {
        return true;
    }
    bool ok = abb_nodo_in_order(nodo->izq, visitar, extra);
    
    if (!ok || !visitar(nodo->clave, nodo->dato, extra)) {
        return false;
    }
    return abb_nodo_in_order(nodo->der, visitar, extra);
}


// Apila todos los nodos que se encuentre a la izquierda del abb
void apilar_todo_izquierda(pila_t* pila, nodo_t* nodo_raiz) {
    if (nodo_raiz == NULL) {
        return;
    }
    pila_apilar(pila,nodo_raiz);
    apilar_todo_izquierda(pila, nodo_raiz->izq);
    return;
}

/* *****************************************************************
 *                        PRIMITIVAS ABB                           *
 * *****************************************************************/


abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
    abb_t* abb = malloc(sizeof(abb_t));
    if (abb == NULL) {
        return NULL;
    }
    abb->raiz = NULL;
    abb->cantidad = 0;
    abb->cmp = cmp;
    abb->destruir_dato = destruir_dato;
    return abb;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
    nodo_t** p_nodo = abb_obtener_p_nodo( &(arbol->raiz), clave, arbol->cmp );
    if (*p_nodo == NULL) { 
        char* clave_dup = strdup(clave);
        if (clave_dup == NULL) {
            return false;
        }
        nodo_t* nuevo_nodo = nodo_crear(clave_dup, dato);
        if (nuevo_nodo ==  NULL) {
            free(clave_dup);
            return false;
        }
        *p_nodo = nuevo_nodo;
        arbol->cantidad++;
    } else { 
        if (arbol->destruir_dato) {
            arbol->destruir_dato((*p_nodo)->dato);
        }
        (*p_nodo)->dato = dato;
    }
    return true;
}

size_t abb_cantidad(const abb_t *arbol) {
    return arbol->cantidad;
}

void *abb_obtener(const abb_t *arbol, const char *clave) {
    nodo_t* nodo = abb_obtener_nodo(arbol, clave);
    return (nodo == NULL) ? NULL : nodo->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
    nodo_t* nodo = abb_obtener_nodo(arbol, clave);
    return !(nodo == NULL);
}

void abb_destruir(abb_t *arbol) {
    nodo_t* nodo_raiz = arbol->raiz;
    abb_destruir_nodos(nodo_raiz, arbol->destruir_dato);
    free(arbol);
}


void *abb_borrar(abb_t *arbol, const char *clave){
    nodo_t** p_nodo = abb_obtener_p_nodo( &(arbol->raiz), clave, arbol->cmp );
   
    if ((*p_nodo) == NULL){
        return NULL;
    }
    void* dato = (*p_nodo)->dato;
    
    if ( (*p_nodo)->izq == NULL || (*p_nodo)->der == NULL) {
        abb_borrar_nodo(p_nodo);
    } else {
        nodo_t** p_nodo_reemplazante = abb_obtener_ext_izq( &((*p_nodo)->der) );
        char* clave_reemplazante = strdup((*p_nodo_reemplazante)->clave);
        void* dato_reemplazante = abb_borrar_nodo(p_nodo_reemplazante); 
        free((*p_nodo)->clave);
        (*p_nodo)->clave = clave_reemplazante;
        (*p_nodo)->dato = dato_reemplazante;
    }
    arbol->cantidad--;
    return dato;
}


/* *****************************************************************
 *                PRIMITIVA ITERADOR INTERNO ABB                   *
 * *****************************************************************/

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra) {
    nodo_t* nodo_raiz = arbol->raiz;
    abb_nodo_in_order(nodo_raiz, visitar, extra);
}

/* *****************************************************************
 *                PRIMITIVA ITERADOR EXTERNO ABB                   *
 * *****************************************************************/

struct abb_iter {
    pila_t* pila;
};


abb_iter_t* abb_iter_in_crear(const abb_t *arbol) {
    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if (iter == NULL) {
        return NULL;
    }
    iter->pila = pila_crear();
    if (iter->pila == NULL) {
        free(iter);
        return NULL;
    }

    apilar_todo_izquierda(iter->pila, arbol->raiz);
    return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter) {
    if (abb_iter_in_al_final(iter)) {
        return false;
    }
    nodo_t* nodo_desapilado = (nodo_t*)pila_desapilar(iter->pila);
    nodo_t* nodo_desapilado_derecha = nodo_desapilado->der;
    apilar_todo_izquierda(iter->pila, nodo_desapilado_derecha);
    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
    if (abb_iter_in_al_final(iter)) {
        return NULL;
    }
    nodo_t* nodo_actual = (nodo_t*)pila_ver_tope(iter->pila); 
    return nodo_actual->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter) {
    return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter) {
    pila_destruir(iter->pila);
    free(iter);
}