#include "declaraciones.h"

int main(int argc, char* argv[]) {
    algogram_t* algogram = verificar_entrada(argc, argv);
    if (algogram == NULL) {
        return -1;
    }
    programa_algogram(algogram);
    return 0;
}