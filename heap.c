#include "heap.h"
#include <stdio.h>
#include <stdlib.h> // malloc
#define TAM_INI 5
#define TAM_REDIMENSION 2

struct heap {
    void** datos;
    size_t cantidad;
    size_t tamanio;
    cmp_func_t cmp;
};

void downheap(void* arr[], size_t tam, size_t padre, cmp_func_t cmp);
void upheap(void* arr[], size_t indice, cmp_func_t cmp);

/***************************
    FUNCIONES AUXILIARES
***************************/

// Hace que los elementos de un arreglo sean los mismos que los elementos de otro arreglo
// Pre: el arreglo de destino tiene tantos lugares disponibles como el largo del arreglo origen
void copiar_arr(void* arr_dest[], void* arr_ori[], size_t n) {
    for (int i = 0; i < n; i++)  {
        arr_dest[i] = arr_ori[i];
    }
}

void heapify(void* arr[], size_t n, cmp_func_t cmp) {
    for (int i = 0; i < n; i++) {
        downheap(arr, n, n-i-1, cmp);
    }
}

void swap(void* arr[], size_t i, size_t j) {
    void* aux = arr[i];
    arr[i] = arr[j];
    arr[j] = aux;
}

size_t buscar_padre(size_t i) {
    return (i - 1) / 2;
}

size_t buscar_h_izq(size_t i, size_t tam) {
    size_t h_izq =  2 * i + 1;
    return h_izq < tam ? h_izq : i;
}

size_t buscar_h_der(size_t i, size_t tam) {
    size_t h_der =  2 * i + 2;
    return h_der < tam ? h_der : i;
}

size_t maximo(void* arr[], cmp_func_t cmp, size_t padre, size_t h_izq, size_t h_der) {
    size_t h_mayor = (cmp(arr[h_izq], arr[h_der]) < 0) ? h_der : h_izq;
    return (cmp(arr[h_mayor], arr[padre]) < 0) ? padre : h_mayor;
}

void downheap(void* arr[], size_t tam, size_t padre, cmp_func_t cmp) {
    size_t h_izq = buscar_h_izq(padre, tam);
    size_t h_der = buscar_h_der(padre, tam);
    if (padre == 0 && h_izq == 0 && h_der == 0) return;
    size_t max = maximo(arr, cmp, padre, h_izq, h_der);
    if (max != padre) {
        swap(arr, padre, max);
        downheap(arr, tam, max, cmp);
    }
}


bool redimensionar(heap_t* heap, size_t cap_nueva) {
    void* datos_nuevos = realloc(heap->datos , cap_nueva * sizeof(void*));
    if (cap_nueva > 0 && datos_nuevos == NULL) {
        return false;
    }
    heap->datos = datos_nuevos;
    heap->tamanio = cap_nueva;
    return true;
}

bool redimensionar_arriba(heap_t *heap) {
    return !(heap->cantidad >= heap->tamanio && !redimensionar(heap, heap->tamanio * TAM_REDIMENSION));
}

void redimensionar_abajo(heap_t* heap) {
    if(heap->cantidad*4 <= heap->tamanio && heap->tamanio > TAM_INI) {
        redimensionar(heap, heap->tamanio/TAM_REDIMENSION);
    }
}

void upheap(void* arr[], size_t indice, cmp_func_t cmp) {
    if (indice == 0) return;
    size_t padre = buscar_padre(indice);

    while (!(cmp(arr[indice], arr[padre]) <= 0)) {
        swap(arr, indice, padre);
        size_t nuevo_indice = padre;
        indice = nuevo_indice;
        if (indice == 0) return;
        padre = buscar_padre(nuevo_indice);
    }
}
/**********************
    PRIMITIVAS HEAP
**********************/

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp) {
    heap_t* heap = malloc(sizeof(heap_t));
    if (heap == NULL) return NULL;
    size_t nuevo_tam = (n * 2 > TAM_INI) ? n * 2 : TAM_INI;
    void** arr_heap = malloc(sizeof(void*) * nuevo_tam);
    if (arr_heap == NULL) {
        free(heap);
        return NULL;
    }
    copiar_arr(arr_heap, arreglo, n);
    heap->datos = arr_heap;
    heap->cantidad = n;
    heap-> tamanio = nuevo_tam;
    heap->cmp = cmp;
    // Heapify
    heapify(arr_heap, n, cmp);
    return heap;
}


bool heap_esta_vacio(const heap_t *heap) {
    return heap->cantidad == 0;
}

void *heap_ver_max(const heap_t *heap) {
    return heap_esta_vacio(heap) ? NULL : heap->datos[0];
}

void *heap_desencolar(heap_t *heap) {
    if (heap_esta_vacio(heap)) {
        return NULL;
    }
    size_t padre = 0;
    size_t ultimo = heap->cantidad - 1;
    swap(heap->datos, padre, ultimo);
    void* dato = heap->datos[ultimo];
    heap->cantidad--;
    downheap(heap->datos, heap->cantidad, padre, heap->cmp);
    redimensionar_abajo(heap);
    return dato;
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp) {
    heapify(elementos, cant, cmp);
    size_t len = cant;
    // swapeo primero con ultimo
    while (len > 0) {
        swap(elementos, 0, len-1);
        len--;
        downheap(elementos, len, 0, cmp);
    }
}

heap_t *heap_crear(cmp_func_t cmp) {
    heap_t* heap = malloc(sizeof(heap_t));
    if (heap == NULL) {
        return NULL;
    }

    heap->datos = malloc(sizeof(void*) * TAM_INI);
    
    if (heap->datos == NULL) {
        free(heap);
        return NULL;
    }

    heap->tamanio = TAM_INI;
    heap->cantidad = 0;
    heap->cmp = cmp;
    return heap;

}

size_t heap_cantidad(const heap_t *heap) {
    return heap->cantidad;
}

bool heap_encolar(heap_t *heap, void *elem) {
    if (!redimensionar_arriba(heap)) {
        return false;
    }
    heap->datos[heap->cantidad] = elem;
    upheap(heap->datos, heap->cantidad, heap->cmp);
    heap->cantidad++;
    return true;   
}

void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e)) {
    if (destruir_elemento) {
        while (!heap_esta_vacio(heap)) {
            void* dato = heap_desencolar(heap);
            destruir_elemento(dato);
        }
    }
    free(heap->datos);
    free(heap);
}