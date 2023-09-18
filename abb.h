#include <stdio.h>
#include <stdbool.h>

typedef struct abb abb_t;
typedef struct nodo nodo_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

/********************************************************************
 *                          PRIMITIVAS ABB                          *
 ********************************************************************/

// Crea un abb y recive como primer parametro una funcion para comparar cadenas.
// y como segundo parametros una funcion para destruir los datos.
// Devuelve un abb vacio.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Agrega una clave con su dato al abb y devuelve true, en caso que no 
// se haya podido agregar devuelve false.
// Pre: el abb fue creado.
// Post: se agrego una clave con su dato al abb.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Borra una clave del abb y devuelve su dato, en caso
// que la clave no exista en el abb devuelve NULL.
// Pre: el abb fue creado.
// Post: se elimina la clave del abb y devuelve el dato.
void *abb_borrar(abb_t *arbol, const char *clave);

// Obtiene el dato de la clave que se le pase por parametro
// en caso de que esa clave pertenezca al abb, en caso contrario
// devuelve NULL.
// Pre: el abb fue creado.
// Post: devuelve el dato de la clave y si la clave no
// pertenecia al abb devuelve NULL.
void *abb_obtener(const abb_t *arbol, const char *clave);

// Verifica que la clave pertenezca al abb y en ese caso
// devuelve true, en caso contrario devuelve false.
// Pre: el abb fue creado.
// Post: devuele true si la clave pertenece al abb o false
// en caso contrario.
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Devuelve la cantidad de claves que hay en el abb actualmente.
// Pre: el abb fue creado.
// Post: devuelve la cantidad de claves en el abb.
size_t abb_cantidad(const abb_t *arbol);

// Destruye el abb.
// Pre: el abb fue creado.
// Post: destruye las claves, los datos y el abb.
void abb_destruir(abb_t *arbol);


/* *****************************************************************
 *                PRIMITIVA ITERADOR INTERNO ABB                   *
 * *****************************************************************/


// Crea un iterador interno para el abb.
// Itera por todos los elementos, aplicando a cada uno la funcion visitar.
// Detiene la iteracion cuando la funcion visitar devuelve false,
// sino itera hasta el final de la abb.
// Pre: el abb existe.
// Post: puede modificar los datos que tiene la abb, pero no sus elementos.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

/* *****************************************************************
 *                PRIMITIVA ITERADOR EXTERNO ABB                   *
 * *****************************************************************/

typedef struct abb_iter abb_iter_t;

// Crea un iterador externo para el abb que se le pase por parametro.
// Devuelve un iterador que apunta al primer elemento del abb.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza en el iterador hacia el siguiente elemento.
// Pre: el iterador fue creado.
// Post: devuelve true en caso de que se haya podido avanzar, devuelve
// false en caso contrario.
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve la clave actual en la que se encuentra apuntando el iterador.
// Pre: el iterador fue creado.
// Post: devuelve la clave actual.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Verifica que el iterador esta al final.
// Pre: el iterador fue creado.
// Post: devuelve true en caso de que iterador este en el final,
// false en caso contrario.
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador.
// Pre: el iterador fue creado.
// Post: el iterador es destruido.
void abb_iter_in_destruir(abb_iter_t* iter);
