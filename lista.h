#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stddef.h> // Para los size_t

/* *****************************************************************
 *                     FUNCIONES PRIMITIVAS                        *
 * *****************************************************************/

typedef struct lista lista_t;

/*
Crea una lista enlazada.
Post: devuelve un puntero a la lista enlazada vacía,
o NULL si no se pudo crear.
*/
lista_t *lista_crear(void);

/*
Devuelve true si la lista no tiene elementos, false en caso contrario.
Pre: la lista existe.
*/
bool lista_esta_vacia(const lista_t *lista);

/*
Agrega un nuevo elemento al principio de la lista.
Devuelve true si se agrego exitosamente, false caso contrario.
Pre: la lista existe.
Post: se agrego un nuevo elemento al principio de la lista.
*/
bool lista_insertar_primero(lista_t *lista, void *dato);

/*
Agrega un nuevo elemento al final de la lista.
Devuelve true si se agrego exitosamente, false caso contrario.
Pre: la lista existe.
Post: se agrego un nuevo elemento al final de la lista.
*/
bool lista_insertar_ultimo(lista_t *lista, void *dato);

/*
Elimina el primer elemento de la lista y devuelve su valor.
Devuelve NULL si la lista esta vacia.
Pre: la lilsta existe.
Post: se elimina el primer elemento de la lista y se devuelve su valor.
*/
void *lista_borrar_primero(lista_t *lista);

// Obtiene el dato del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el dato del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el dato del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el dato del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el largo de la lista.
// Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void (*destruir_dato)(void *));


/* *****************************************************************
 *                       ITERADOR EXTERNO                          *
 * *****************************************************************/

typedef struct lista_iter lista_iter_t;

// Crea un iterador externo de la lista.
// Post: devuelve un puntero a la lista enlazada vacía, o NULL si no se pudo crear.
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza al siguiente elemente de la lista.
// Pre: la lista fue creada.
// Post: devuelve True si se pudo avanzar un elemento mas o False en caso de que
// no se pueda avanzar mas porque ya esta en el fina la lista.
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el dato que tenga el elemento.
// Pre: la lista fue creada.
// Post: Devuelve el dato que tenga el elemento, en caso de estar vacia devuelve NULL.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Se fija que este en el final de la lista.
// Pre: la lista fue creada.
// Post: devuelve True en caso de que se encuentre en el final de la lista,
// en caso contrario devuelve False.
bool lista_iter_al_final(const lista_iter_t *iter);

/*
Destruye el iterador de la lista.
Pre: el itereador existe y actua sobre una lista.
Post: se eliminaron el iterador y todos sus elementos.
*/
void lista_iter_destruir(lista_iter_t *iter);

/*
Inserta un elemento a la lista en el lugar apuntado.
Devuelve false solo si no se pudo insertar el dato en la lista.
Caso contrario, devuelve true.
Pre: el iterador existe y actua sobre una lista.
Post: se agrega un elemento a la lista, y el iterador apunta a ese nuevo elemento.
*/
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

/*
Borra el elemento de la lista que está apuntado por el iterador.
Devuelve NULL si la lista estaba vacia, o si el iterador esta al final.
o el dato eliminado si no estaba vacia.
Pre: el iterador existe y actua sobre una lista.
Post: la lista tiene un elemento menos, y el iterador apunta al siguiente del elemento eliminado.
Si elimino el ultimo elemento, el iterador se encontrara al final.
*/
void *lista_iter_borrar(lista_iter_t *iter);


/* *****************************************************************
 *                       ITERADOR INTERNO                          *
 * *****************************************************************/

/*
Crea un iterador interno para la lista.
Itera por todos los elementos, aplicando a cada uno la funcion visitar.
Detiene la iteracion cuando la funcion visitar devuelve false,
sino itera hasta el final de la lista.
Pre: la lista existe.
Post: puede modificar los datos que tiene la lista, pero no sus elementos.
(No puede insertar o borrar nuevos elementos).
*/
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);


/* *****************************************************************
 *                      PRUEBAS UNITARIAS                          *
 * *****************************************************************/

// Realiza pruebas sobre la implementación realizada.
void pruebas_lista_estudiante(void);

#endif 