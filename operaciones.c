#include "declaraciones.h"

void actualizar_feed(algogram_t* algogram, post_t* post) {
    hash_t* users = algogram->users;
    char* user_actual = algogram->user_actual;
    datos_user_t* datos_user_post = hash_obtener(users, post->user);
    size_t id_user_post = datos_user_post->id_user; 

    hash_iter_t* iter_usuarios = hash_iter_crear(users); 
    while (!hash_iter_al_final(iter_usuarios)) {
        const char* usuario = hash_iter_ver_actual(iter_usuarios); 
        if (strcmp(usuario, user_actual) == 0) { 
            hash_iter_avanzar(iter_usuarios);
            continue;
        }
        datos_user_t* datos_user = hash_obtener(users, usuario); 
        
        int afinidad = abs((int)id_user_post - (int)datos_user->id_user);
        afinidad_post_t* afinidad_post = afinidad_post_crear(post, afinidad);
        if (afinidad_post == NULL) {
            fprintf(stdout,  ERROR_MEMORIA);
            hash_iter_destruir(iter_usuarios);
            return;
        }
        heap_encolar(datos_user->feed_user, afinidad_post);
        hash_iter_avanzar(iter_usuarios);
    }
    hash_iter_destruir(iter_usuarios);
}


int comparar_users_afinidad(const void *a, const void *b) {
    afinidad_post_t* bloque_a = (afinidad_post_t*)a;
    afinidad_post_t* bloque_b = (afinidad_post_t*)b;

    int diff_a = bloque_a->afinidad;
    int diff_b = bloque_b->afinidad;

    if (diff_a > diff_b) return -1;
    if (diff_b > diff_a) return 1;
    return (bloque_a->post->id_post < bloque_b->post->id_post) ? 1 : -1;
}

void imprimir_likes(abb_t* users_likes, size_t cant_likes) {
    fprintf(stdout, "El post tiene %zu likes:\n", cant_likes);
    abb_iter_t* iter_users_likes = abb_iter_in_crear(users_likes);
    while (!abb_iter_in_al_final(iter_users_likes)) {
        const char* clave_actual = abb_iter_in_ver_actual(iter_users_likes);
        fprintf(stdout, "	%s\n", clave_actual);
        abb_iter_in_avanzar(iter_users_likes);
    }
    abb_iter_in_destruir(iter_users_likes);
}


void ejecutar_login(algogram_t* algogram) {
    if (algogram->user_actual != NULL) {
        fprintf(stdout, ERROR_LOGIN);
        return;
    }

    char* user_ingresado = ingresar_linea();
    if (user_ingresado == NULL) {
        fprintf(stdout, ERROR_LINEA_INGRESADA);
        return;
    }

    if (!hash_pertenece(algogram->users, user_ingresado)) {
        free(user_ingresado);
        fprintf(stdout, ERROR_USER);
        return;
    }

    fprintf(stdout, "Hola %s\n", user_ingresado);
    algogram->user_actual = user_ingresado;
}

void ejecutar_logout(algogram_t* algogram) {
    if (algogram->user_actual == NULL) {
        fprintf(stdout, ERROR_LOGOUT);
        return;
    }
    fprintf(stdout, DESPEDIDA);
    free(algogram->user_actual);
    algogram->user_actual = NULL;
}

void ejecutar_publicar(algogram_t* algogram) {
    if (algogram->user_actual == NULL) {
        fprintf(stdout, ERROR_LOGOUT);
        return;
    }

    char* texto_post = ingresar_linea();
    if (texto_post == NULL) {
        fprintf(stdout, ERROR_LINEA_INGRESADA);
        return;
    }

    char* user_dup = strdup(algogram->user_actual);
    if (user_dup == NULL) {
        fprintf(stdout,  ERROR_MEMORIA);
        free(texto_post);
        return;
    }
    size_t cant_posts = hash_cantidad(algogram->posts);

    post_t* nuevo_post = post_crear(texto_post, user_dup, cant_posts);
    if (nuevo_post == NULL) {
        free(texto_post);
        free(user_dup);
        fprintf(stdout,  ERROR_MEMORIA);
        return;
    }

    char nuevo_id[TAM_BUFFER];
    sprintf(nuevo_id, "%zu", cant_posts);

    hash_guardar(algogram->posts, nuevo_id, nuevo_post);
    actualizar_feed(algogram, nuevo_post);
    fprintf(stdout, POST_PUBLICADO);
}

void ejecutar_ver_sig_feed(algogram_t* algogram) {
    if (algogram->user_actual == NULL) {
        fprintf(stdout, ERROR_VER_SIG_FEED);
        return;
    }
    datos_user_t* datos_user_actual = hash_obtener(algogram->users, algogram->user_actual);
    heap_t* feed_user_actual = datos_user_actual->feed_user;
    afinidad_post_t* afinidad_post_user_actual = heap_desencolar(feed_user_actual);
    if (afinidad_post_user_actual == NULL) {
        fprintf(stdout, ERROR_VER_SIG_FEED);
        return;
    }
    fprintf(stdout, "Post ID %zu\n", afinidad_post_user_actual->post->id_post);
    fprintf(stdout, "%s dijo: %s\n", afinidad_post_user_actual->post->user, afinidad_post_user_actual->post->texto);
    fprintf(stdout, "Likes: %zu\n", afinidad_post_user_actual->post->cant_likes);
    free(afinidad_post_user_actual);
}

void ejecutar_likear_post(algogram_t* algogram) {
    if (algogram->user_actual == NULL) {
        fprintf(stdout, ERROR_POST_LIKE);
        return;
    }

    char* id_post = ingresar_linea();
    if (id_post == NULL) {
        fprintf(stdout, ERROR_LINEA_INGRESADA);
        return;
    }

    if (!hash_pertenece(algogram->posts, id_post)) {
        free(id_post);
        fprintf(stdout, ERROR_POST_LIKE);
        return;
    }
    
    post_t* post_a_likear = hash_obtener(algogram->posts, id_post);
    if (post_a_likear->user_likes == NULL) {
        post_a_likear->user_likes = abb_crear(strcmp, free);
    }

    if (abb_pertenece(post_a_likear->user_likes, algogram->user_actual)) {
        free(id_post);
        fprintf(stdout, POST_LIKEADO);
        return;
    }

    abb_guardar(post_a_likear->user_likes, algogram->user_actual, NULL);
    post_a_likear->cant_likes++;
    fprintf(stdout, POST_LIKEADO);
    free(id_post);
    return;
}

void ejecutar_mostrar_likes(algogram_t* algogram) {
    char* id_post_like = ingresar_linea();
    if (id_post_like == NULL) {
        fprintf(stdout, ERROR_LINEA_INGRESADA);
        return;
    }

    if (!hash_pertenece(algogram->posts, id_post_like)) {
        free(id_post_like);
        fprintf(stdout, ERROR_MOSTRAR_LIKE);
        return;
    }

    post_t* post_a_likear = hash_obtener(algogram->posts, id_post_like);
    size_t cant_likes_post = post_a_likear->cant_likes;

    if (cant_likes_post == 0) {
        free(id_post_like);
        fprintf(stdout, ERROR_MOSTRAR_LIKE);
        return;
    }
    imprimir_likes(post_a_likear->user_likes, cant_likes_post);
    free(id_post_like);
}

