#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h> // abs

#include "hash.h"
#include "lista.h"
#include "heap.h"
#include "abb.h"
#include "pila.h"

#define TAM_BUFFER 50

#define ERROR_LOGIN "Error: Ya habia un usuario loggeado\n"
#define ERROR_LINEA_INGRESADA "Error: no se pudo leer la linea ingresada\n"

#define ERROR_LOGOUT "Error: no habia usuario loggeado\n"
#define DESPEDIDA "Adios\n"

#define ERROR_USER "Error: usuario no existente\n"

#define POST_PUBLICADO "Post publicado\n"

#define ERROR_VER_SIG_FEED "Usuario no loggeado o no hay mas posts para ver\n"

#define ERROR_POST_LIKE "Error: Usuario no loggeado o Post inexistente\n"
#define POST_LIKEADO "Post likeado\n"

#define ERROR_MOSTRAR_LIKE "Error: Post inexistente o sin likes\n"

#define ERROR_MEMORIA "Error: No se pudo pedir memoria\n"

/*************************
 * ESTRUCTURAS AUXILIARES
**************************/

typedef struct datos_user {
    heap_t* feed_user;
    size_t id_user;
}datos_user_t;

typedef struct post {
    char* texto;
    char* user;
    size_t id_post;
    size_t cant_likes;
    abb_t* user_likes;
}post_t;

typedef struct algogram {
    hash_t* users;
    hash_t* comandos;
    hash_t* posts;
    char* user_actual;
}algogram_t;

typedef struct afinidad_post {
    int afinidad;
    post_t* post;
}afinidad_post_t;


// DECLARACIONES

// CODIGO EN ENTRADA.C

// Verifica que todo se halla creado correctamente antes de iniciar el programa
// Devuelve un puntero hacia algogram en caso de que este todo bien, en caso 
// contrario devuelve NULL.
algogram_t* verificar_entrada(int argc, char* argv[]);

// Lee el archivo de usuarios que se le pase en el primer parametro y luego guarda
// la informacion de cada usuario en un hash que tiene como clave el nombre del usuario y como
// valor su informacion.
// Funcion utilizada en: main().
hash_t* procesar_usuarios(FILE* archivo);


// CODIGO EN COMANDO.C

// Lee la linea ingresada por el usuario y devuelve un puntero de
// la linea leida, en caso de error devuelve NULL.
// Funcion utilizada en: ejecutar_login(), ejecutar_publicar(), 
// ejecutar_likear_post() y ejecutar_mostrar_likes().
char* ingresar_linea();

// Verifica que el comando ingresado sea valido.
// Devuelve True en caso de que sea valido, False en caso contrario.
// Funcion utilizada en: main().
bool validar_comando(algogram_t* algogram, char* comando);

// Ejecuta el comando que se haya ingresado.
// Funcion utilizada en: programa_algogram().
void ejecutar_comando(algogram_t* algogram, char* comando);

// Funcion encargada de ejecutar el ciclo que reciba una y otra vez 
// comandos que son ingresados por el usuario.
// Funcion utilizada en: main().
void programa_algogram(algogram_t* algogram);


// CODIGO EN STRUCT.C

// Crea la estructura algogram
// Devuelve un puntero a algogram en caso de que se haya podido crear,
// en caso contrario devuelve NULL.
// Funcion utilizada en: main().
algogram_t* algogram_crear();

// Crea la estructura post.
// Devuelve un puntero a post en caso de que se haya podido crear,
// en caso contrario devuelve NULL.
// Funcion utilizada en: ejecutar_publicar().
post_t* post_crear(char* texto, char* user, size_t id_post);

// Crea la estructura datos_user.
// Devuelve el puntero a datos_user en caso de que se haya
// podido crear, en caso contrario devuelve NULL.
// Funcion utilizada en: procesar_usuarios().
datos_user_t* datos_user_crear(size_t id_user);

// Crea la estructura afinidad_post 
// Devuelve un puntero al afinidad_post en caso de que se haya podido crear,
// en caso contrario devuelve NULL.
// Funcion utilizada en: actualizar_feed().
afinidad_post_t* afinidad_post_crear(post_t* post, int afinidad);

// Destruye algogram.
// Pre: algogram fue creado.
void algogram_destruir(algogram_t* algogram);

// Destruye la estructura post.
// Utilizada para destruir los datos del hash_post.
// Funcion utilizada en: hash_crear() del hash_post.
void eliminar_post(void* dato);

// Destruye la estructura datos_user.
// Utilizada para destruir los datos de los usuarios.
// Funcion utlizada en: hash_crear() de los usuarios.
void eliminar_user(void* dato);

// Destruye la estructura afinidad_post.
// Utilizada para destruir los datos del feed_user.
// Funcion utilizada en: heap_destruir() del feed_user.
void eliminar_afinidad_post(void* dato);


// CODIGO EN OPERACIONES.C

// Compara afinidad_post_t segun afinidad al usuario actual.
// debe devolver:
//  menor a 0  si  a < b
//      0      si  a == b
//  mayor a 0  si  a > b
// Funcion utilizada en: datos_user_crear().
int comparar_users_afinidad(const void *a, const void *b);

// Actualiza el feed de los usuarios, 
// añadiendo el post a los heaps de los usuarios que no son el actual
void actualizar_feed(algogram_t* algogram, post_t* post);

// Imprime por pantalla cant_likes, luego recorre todo el
// arbol user_likes en inorder y lo va imprimiendo por pantalla.
// Funcion utilizada en: ejecutar_mostrar_likes().
void imprimir_likes(abb_t* user_likes, size_t cant_likes);

// Verifica que no haya un usuario logeado
// En caso de que haya un usuario logeado devuelve error,
// en caso contrario lo agrega al algogram.
void ejecutar_login(algogram_t* algogram);

// Verifica que haya un usuario logeado
// En caso de que no haya un usuario logeado devuelve error,
// en caso contrario lo saca del algogram.
void ejecutar_logout(algogram_t* algogram);

// Crea un post del usuario logeado actualmente
// En caso de que no haya un usuario logeado devuelve error,
// en caso contrario crea y guarda el post.
void ejecutar_publicar(algogram_t* algogram);

// Ve el siguiente post del feed
// En caso de que no haya un usuario logeado devuelve error,
// en caso contrario imprime por pantalla el siguiente post.
void ejecutar_ver_sig_feed(algogram_t* algogram);

// Likea un post existente
// En caso de que no haya nadie logueado o el post sea inexistente
// devuelve error, en caso contrario se likea el post y se guarda
// el usuario que likeo.
void ejecutar_likear_post(algogram_t* algogram);

// Muetra los usuarios que likearon el post.
// En caso de que no tenga likes o que el post sea inexistente
// devuelve error, en caso contrario muestra los usuarios y 
// la cantidad de likes que tiene el post. 
void ejecutar_mostrar_likes(algogram_t* algogram);

