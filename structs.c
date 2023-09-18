#include "declaraciones.h"

algogram_t* algogram_crear() {
    algogram_t* algogram = malloc(sizeof(algogram_t));
    if (algogram == NULL) return NULL;
    hash_t* comandos = hash_crear(NULL);
    if (comandos == NULL) {
        free(algogram);
        return NULL;
    }
    hash_t* posts = hash_crear(eliminar_post);
    if (posts == NULL) {
        free(algogram);
        hash_destruir(comandos);
        return NULL;
    }
    algogram->users = NULL;
    algogram->comandos = comandos;
    algogram->posts = posts;
    algogram->user_actual = NULL;
    return algogram;
}

post_t* post_crear(char* texto, char* user, size_t id_post) {
    post_t* post = malloc(sizeof(post_t));
    if (post== NULL) return NULL;

    post->texto = texto;
    post->user = user;
    post->id_post = id_post;
    post->cant_likes = 0;
    post->user_likes = NULL;
    return post;
}

afinidad_post_t* afinidad_post_crear(post_t* post, int afinidad) {
    afinidad_post_t* afinidad_post = malloc(sizeof(afinidad_post_t));
    if (afinidad_post == NULL) {
        return NULL;
    }
    afinidad_post->post = post;
    afinidad_post->afinidad = afinidad;
    return afinidad_post;
}

datos_user_t* datos_user_crear(size_t id_user) {
    datos_user_t* datos_user = malloc(sizeof(datos_user_t));
    if (datos_user == NULL) return NULL;
    heap_t* feed_user = heap_crear(comparar_users_afinidad);
    if (feed_user == NULL) {
        free(datos_user);
        return NULL;
    }
    datos_user->feed_user = feed_user;
    datos_user->id_user = id_user;
    return datos_user;
}

void algogram_destruir(algogram_t* algogram) {
    hash_destruir(algogram->users);
    hash_destruir(algogram->comandos);
    hash_destruir(algogram->posts);
    free(algogram->user_actual);
    free(algogram);
}


void eliminar_post(void* dato) {
    post_t* post_user = (post_t*)dato;
    if (post_user->user_likes != NULL) {
        abb_destruir(post_user->user_likes);
    }
    free(post_user->texto);
    free(post_user->user);
    free(post_user);
}

void eliminar_afinidad_post(void* dato) {
    afinidad_post_t* afinidad_post = (afinidad_post_t*)dato;
    free(afinidad_post);
}

void eliminar_user(void* dato) {
    datos_user_t* struct_user = (datos_user_t*)dato;
    heap_destruir(struct_user->feed_user, eliminar_afinidad_post);
    free(struct_user);
}
