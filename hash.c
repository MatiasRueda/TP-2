#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define F_REDIM 11
#define F_CARGA_MAX 2
#define F_CARGA_MIN 0.1
#define CAPACIDAD_INICIAL 11

struct hash {
    lista_t** arreglo;
    size_t cantidad;
    size_t capacidad;
    hash_destruir_dato_t destruir_dato;
};

typedef struct campo {
    char* clave;
    void* valor;
}campo_t;

struct hash_iter {
    const hash_t* hash;
    lista_iter_t* iter_actual;
    int pos_actual;
};
/*******************************************************************
 *                         FUNCION HASH                            *
 *******************************************************************/

// Esta funcion de hash fue educativamente tomada de https://github.com/mkirchner/stutter/blob/main/src/djb2.c
unsigned long funcion_hash(const char *str) {
    unsigned char *s = (unsigned char *) str;
    unsigned long hash = 5381;
    int c;

    while ((c = *s++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}


/*******************************************************************
 *                      PRIMITIVAS CAMPO                           *
 *******************************************************************/

campo_t* campo_crear(char *clave, void *dato) {
    campo_t* nuevo_campo = malloc(sizeof(campo_t));
    if (nuevo_campo == NULL) {
        return NULL;
    }
    nuevo_campo->clave = clave;
    nuevo_campo->valor = dato;
    return nuevo_campo;
}

/*******************************************************************
 *         FUNCIONES AUXILIARES UTILIZADAS PARA EL HASH            *           
 *******************************************************************/

/// Libera las listas que fueron pedidas con anterioridad 
/// Funcion utilizada en: agregar_lista_al_arreglo()
void limpieza_de_arreglo(lista_t** arreglo, size_t tamanio){
    for (int j = 0; j < tamanio; j++) {
        lista_destruir(arreglo[j],NULL);
    }
}

/// Agrega listas vacias a un arreglo de listas.
/// Devuelve true en caso de que se haya podido agregar, false en caso de que
/// no se haya podido.
///Funcion utilizada en: Redimensionar_hash() y hash_crear()
bool agregar_lista_al_arreglo(lista_t** arreglo, size_t tamanio) {
    for (int i = 0; i < tamanio; i++) {
        arreglo[i] = lista_crear();
        if (arreglo[i] == NULL) {
            limpieza_de_arreglo(arreglo, i);
            return false;
        }
    }
    return true;
}

/// Funcion utilizada para pasar todo lo de un hash a otro de tamanio new_cap
/// En realidad, pasa todos los elementos de una lista del arreglo del hash, a otro hash
/// Funcion utilizada en: Redimensionar_hash()
void pasar_elementos_al_nuevo_arreglo(int indice, hash_t* hash_viejo, lista_t** new_arr, size_t new_cap) {
    lista_t* lista_hash_viejo = hash_viejo->arreglo[indice];
    lista_iter_t* iter_lista_vieja = lista_iter_crear(lista_hash_viejo);

    while (!lista_iter_al_final(iter_lista_vieja)) {
        campo_t* campo_lista_vieja = (campo_t*)lista_iter_borrar(iter_lista_vieja);
        char* clave_vieja = campo_lista_vieja->clave;
        unsigned long int hash_pos = funcion_hash(clave_vieja) % new_cap;
        lista_t* lista_nueva = new_arr[hash_pos];
        lista_insertar_ultimo(lista_nueva, campo_lista_vieja);
    }

    lista_destruir(hash_viejo->arreglo[indice], NULL);
    lista_iter_destruir(iter_lista_vieja);
}

/// Limpia el valor de un campo.
/// Funcion utilizada en: hash_borrar() y hash_guardar().
void limpiar_campo(hash_t* hash, campo_t* campo) {
    if (hash->destruir_dato) {
        hash->destruir_dato(campo->valor);
    }
}

/// Redimensiona el hash, creando un nuevo arreglo de tamanio nueva_capacidad
/// Devuelve True en caso de que se haya podido redimensionar.
/// Devuelve False en caso de que no se haya podido redimensionar.
/// Funcion utilizada en: hash_guardar().
bool redimensionar_hash(hash_t* hash, size_t nueva_capacidad) {
    lista_t** arreglo_nuevo = malloc(sizeof(void*) * nueva_capacidad);

    if(arreglo_nuevo == NULL || !agregar_lista_al_arreglo(arreglo_nuevo, nueva_capacidad)) {
        return false;
    }

    for (int j = 0; j < hash->capacidad; j++) {
        pasar_elementos_al_nuevo_arreglo(j, hash, arreglo_nuevo, nueva_capacidad);
    }

    free(hash->arreglo);

    hash->capacidad = nueva_capacidad;
    hash->arreglo = arreglo_nuevo;
    return true;
}

/// Recorre el hash buscando un campo que contenga dicha clave
/// Devuelve NULL si la clave no fue guardada en el hash
/// Caso contrario, devuelve el campo que tiene guardada dicha clave.
/// Funcion utilizada en: hash_guardar(), hash_obtener() y hash_pertenece().
campo_t* hash_buscar_campo(const hash_t *hash, const char *clave) {
    lista_iter_t* iter = lista_iter_crear(hash->arreglo[funcion_hash(clave) % hash->capacidad]);
    campo_t* campo_buscado = NULL;
    while (!lista_iter_al_final(iter)) {
        campo_t* campo = lista_iter_ver_actual(iter);
        if (strcmp(campo->clave,clave) == 0) {
            campo_buscado = campo;
            break;
        }
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);
    return campo_buscado;
}

/*******************************************************************
 *                        PRIMITIVAS HASH                          *
 *******************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {
    hash_t* nuevo_hash = malloc(sizeof(hash_t));
    if (nuevo_hash == NULL) {
        return NULL;
    }
    lista_t** nuevo_arreglo = malloc(CAPACIDAD_INICIAL * sizeof(void*));
    if (nuevo_arreglo == NULL) {
        free(nuevo_hash);
        return NULL;
    }
    if (!agregar_lista_al_arreglo(nuevo_arreglo,CAPACIDAD_INICIAL)) {
        free(nuevo_arreglo);
        free(nuevo_hash);
        return NULL;
    }

    nuevo_hash->arreglo = nuevo_arreglo;
    nuevo_hash->cantidad = 0;
    nuevo_hash->capacidad = CAPACIDAD_INICIAL;
    nuevo_hash->destruir_dato = destruir_dato;
    return nuevo_hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
    long unsigned int F_CARGA = hash->cantidad/hash->capacidad;
    size_t nueva_capacidad = hash->capacidad * F_REDIM;
    if (F_CARGA > F_CARGA_MAX && !redimensionar_hash(hash, nueva_capacidad) ) {
        return false;
    }
    unsigned long pos_hash = funcion_hash(clave) % hash->capacidad;
    lista_t* lista_hash = hash->arreglo[pos_hash];
    char* clave_guardar = strdup(clave);
    if (clave_guardar == NULL) {
        return false;
    }
    campo_t* campo_encontrado = hash_buscar_campo(hash, clave_guardar);
    if (campo_encontrado != NULL) {
        free(clave_guardar);
        limpiar_campo(hash, campo_encontrado);
        campo_encontrado->valor = dato;
        return true;
    }
    campo_t* campo_nuevo = campo_crear(clave_guardar,dato);
    lista_insertar_ultimo(lista_hash,campo_nuevo);
    hash->cantidad++;
    return true;
}


void *hash_obtener(const hash_t *hash, const char *clave) {
    campo_t* campo_encontrado = hash_buscar_campo(hash, clave);
    if (campo_encontrado != NULL) {
        return campo_encontrado->valor;
    }
    return NULL;
}

void *hash_borrar(hash_t *hash, const char *clave) {
    lista_iter_t* iter = lista_iter_crear(hash->arreglo[funcion_hash(clave) % hash->capacidad]);
    void* valor = NULL;
    while (!lista_iter_al_final(iter)) {
        campo_t* campo = lista_iter_ver_actual(iter);
        if ((strcmp(campo->clave, clave) == 0)) {
            valor = campo->valor;
            lista_iter_borrar(iter);
            free(campo->clave);
            free(campo);
            hash->cantidad--;
            if (hash->capacidad > CAPACIDAD_INICIAL * F_REDIM && hash->cantidad/hash->capacidad < F_CARGA_MIN) {
                size_t nueva_capacidad = hash->capacidad / F_REDIM;
                redimensionar_hash(hash, nueva_capacidad);
            }
            break;
        }
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);
    return valor;
}    

bool hash_pertenece(const hash_t *hash, const char *clave) {
    return (hash_buscar_campo(hash, clave) == NULL) ? false : true;
}

size_t hash_cantidad(const hash_t *hash) {
    return hash->cantidad;
}

void hash_destruir(hash_t *hash) {
    for (int i = 0; i < hash->capacidad; i++) {
        lista_t* lista = hash->arreglo[i];
        while (!lista_esta_vacia(lista)) {
            campo_t* campo = lista_borrar_primero(lista);
            free(campo->clave);
            limpiar_campo(hash, campo);
            free(campo);
        }
        lista_destruir(lista, free);
    }
    free(hash->arreglo);
    free(hash);
}

/*******************************************************************
 *         FUNCION AUXILIAR UTILIZADA PARA EL ITERADOR HASH        *
 *******************************************************************/

/// Recorre la tabla de hash desde pos_actual hasta la capacidad
/// En cada iteracion, crea un iterador de lista, buscando que sea valida (con elementos)
/// Si ninguna posicion anterior tenia listas validas, el iterador se va a parar en la ultima lista
/// Funcion utilizada en: hash_iter_crear() y hash_iter_avanzar()
void buscar_lista_valida(hash_iter_t *iter) {
    for (; iter->pos_actual < iter->hash->capacidad; iter->pos_actual++) {
        lista_t* lista_actual = iter->hash->arreglo[iter->pos_actual];
        iter->iter_actual = lista_iter_crear(lista_actual);
        /* Devuelvo la primer lista no vacia */
        if (!lista_iter_al_final(iter->iter_actual)) {
            break;
        /* o sumo 1 a la pos si es la ultima lista y estaba vacia */
        } else if (iter->pos_actual == iter->hash->capacidad - 1) {
            lista_iter_destruir(iter->iter_actual);
            iter->pos_actual++;
            break;
        }
        lista_iter_destruir(iter->iter_actual);
    }
}

/*******************************************************************
 *                  PRIMITIVAS ITERADOR HASH                       *
 *******************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash) {
    hash_iter_t* iter = malloc(sizeof(hash_iter_t));
    if (iter == NULL) {
        return NULL;
    }
    iter->hash = hash;
    iter->pos_actual = 0;
    iter->iter_actual = NULL;

    buscar_lista_valida(iter);
    return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter) {
    if (hash_iter_al_final(iter)) {
        return false;
    }
    if (lista_iter_avanzar(iter->iter_actual) && !lista_iter_al_final(iter->iter_actual)) {
        return true;
    }
    lista_iter_destruir(iter->iter_actual);
    iter->pos_actual++;
    buscar_lista_valida(iter);
    return (hash_iter_al_final(iter)) ? false : true;
}

bool hash_iter_al_final(const hash_iter_t *iter) {
    return iter->pos_actual >= iter->hash->capacidad;
}

void hash_iter_destruir(hash_iter_t *iter) {
    if (!hash_iter_al_final(iter)) {
        lista_iter_destruir(iter->iter_actual);
    }
    free(iter);
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
    if (hash_iter_al_final(iter)) {
        return NULL;
    }
    campo_t* campo = lista_iter_ver_actual(iter->iter_actual);
    return campo->clave;
}
