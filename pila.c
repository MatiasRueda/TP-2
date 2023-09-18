#include "pila.h"
#include <stdlib.h>

#define CAP_REDIMENSION 2
#define CAP_INI 5


/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void **datos;
    size_t cantidad;   // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};


bool pila_redimensionar_arriba(pila_t *pila);
void pila_redimensionar_abajo(pila_t *pila);
bool pila_redimensionar(pila_t* pila, size_t cap_nueva);

/******************************************************************
 *                    PRIMITIVAS DE LA PILA                       *
 ******************************************************************/

// ...

pila_t *pila_crear(void) {
    pila_t *pila = malloc(sizeof(pila_t));

    if(pila == NULL) {
        return NULL;
    }
    pila->datos = malloc(CAP_INI*sizeof(void*));

    if(pila->datos == NULL){
        free(pila);
        return NULL;
    }
    pila->cantidad = 0;
    pila->capacidad = CAP_INI;
    return pila;
}

void pila_destruir(void* direc) {
    pila_t* pila = direc;
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila) {
    return pila->cantidad == 0;
}


bool pila_apilar(pila_t *pila, void *valor) { 
    if (!pila_redimensionar_arriba(pila)) {
        return false;
    }
    pila->datos[pila->cantidad] = valor;
    pila->cantidad++ ;
    return true;
}

void *pila_ver_tope(const pila_t *pila) {
    return pila_esta_vacia(pila) ? NULL : pila->datos[pila->cantidad-1];
}


void *pila_desapilar(pila_t *pila) {
    if (pila_esta_vacia(pila)) {
        return NULL;
    }
    pila->cantidad--;
    pila_redimensionar_abajo(pila);
    return pila->datos[pila->cantidad];

}

/******************************************************************
 *             FUNCIONES PARA REDIMENSIONAR LA PILA               *
 ******************************************************************/


bool pila_redimensionar_arriba(pila_t *pila) {
    return !(pila->cantidad >= pila->capacidad && !pila_redimensionar(pila, pila->capacidad * CAP_REDIMENSION));
}

void pila_redimensionar_abajo(pila_t *pila) {
    if (pila->cantidad * 4 <= pila->capacidad && pila->capacidad > CAP_INI) {
        pila->capacidad /= CAP_REDIMENSION;
        pila_redimensionar(pila,pila->capacidad);
    }
}

bool pila_redimensionar(pila_t* pila, size_t cap_nueva) {
    void* datos_nuevos = realloc(pila->datos , cap_nueva * sizeof(void*));
    if (cap_nueva > 0 && datos_nuevos == NULL) {
        return false;
    }
    pila->datos = datos_nuevos;
    pila->capacidad = cap_nueva;
    return true;
}
