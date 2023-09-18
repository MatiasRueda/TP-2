#include "declaraciones.h"

hash_t* procesar_usuarios(FILE* mi_archivo) {
    char* usuario_actual = NULL;
    size_t tam = 0;
    size_t id_user = 0;
    hash_t* usuarios = hash_crear(eliminar_user);

    while ( (getline(&usuario_actual, &tam, mi_archivo)) != EOF ) {
        if (!usuario_actual) continue;
        usuario_actual[strcspn(usuario_actual, "\r\n")] = 0;
        datos_user_t* datos_user = datos_user_crear(id_user);
        if (datos_user == NULL) {
            fprintf(stdout, "Error: no se pudo guardar los datos del usuario_actual\n");
            free(usuario_actual);
            hash_destruir(usuarios);
            return NULL;
        }
        hash_guardar(usuarios, usuario_actual, datos_user);
        id_user++;
    }
    free(usuario_actual);
    return usuarios;
}

algogram_t* verificar_entrada(int argc, char* argv[]){
    if (argc != 2) {
        fprintf(stdout, "Error: numero de parametros incorrectos\n");
        return NULL;
    }

    char* nombre_archivo = argv[1];
    int cant_comandos = 6;
    char* comandos[] = {"login", "logout", "publicar", "ver_siguiente_feed", "likear_post", "mostrar_likes"};
    FILE* mi_archivo = fopen(nombre_archivo, "r"); 
    if (mi_archivo == NULL) {
        fprintf(stdout, "Ocurrio un error en la apertura del archivo\n");
        return NULL;
    }
    algogram_t* algogram = algogram_crear();
    if (algogram == NULL) {
        fclose(mi_archivo);
        fprintf(stdout, "Ocurrio un error\n");
        return NULL;
    };
    for (int i = 0; i < cant_comandos; i++) { 
        hash_guardar(algogram->comandos, comandos[i], NULL);
    }
    algogram->users = procesar_usuarios(mi_archivo);
    fclose(mi_archivo);
    return algogram;
}