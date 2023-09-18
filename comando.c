#include "declaraciones.h"


bool validar_comando(algogram_t* algogram, char* comando) {
    return hash_pertenece(algogram->comandos, comando);
}

char* ingresar_linea() {
    ssize_t bytes_leidos;
    size_t numero_bytes = 0;
    char* linea_ingresada = NULL;
    bytes_leidos = getline(&linea_ingresada, &numero_bytes, stdin);
    if (bytes_leidos == -1) {
        return NULL;
    }
    linea_ingresada[strcspn(linea_ingresada, "\r\n")] = 0;
    return linea_ingresada;
}


void ejecutar_comando(algogram_t* algogram, char* comando) { 
    if (strcmp(comando, "login") == 0) { // EJECUTAR LOGIN
        ejecutar_login(algogram);
        return;
    }
    else if (strcmp(comando, "logout") == 0) { // EJECUTAR LOGOUT
        ejecutar_logout(algogram);
        return;
    }
    else if (strcmp(comando, "publicar") == 0) { // EJECUTAR PUBLICAR
        ejecutar_publicar(algogram);
        return;
    }
    else if (strcmp(comando, "ver_siguiente_feed") == 0) { // EJECUTAR VER_SIGUIENTE_FEED
        ejecutar_ver_sig_feed(algogram);
        return;
    }
    else if (strcmp(comando, "likear_post") == 0) { // EJECUTAR LIKEAR_POST
        ejecutar_likear_post(algogram);
        return;
    }
    
    else if (strcmp(comando, "mostrar_likes") == 0) { // EJECUTAR MOSTRAR_LIKES
        ejecutar_mostrar_likes(algogram);
        return;
    }
    
}

void programa_algogram(algogram_t* algogram){
    char* comando = NULL;
    size_t tam = 0;
    // Comportamiento
    while ( (getline(&comando, &tam, stdin)) != EOF ) {
        if (!comando) continue;
        comando[strcspn(comando, "\r\n")] = 0; 

        if (!validar_comando(algogram, comando)) {
            continue;
        }
        ejecutar_comando(algogram, comando);
    }

    // --- destruir todo ---
    free(comando);
    algogram_destruir(algogram);
    return;
}
