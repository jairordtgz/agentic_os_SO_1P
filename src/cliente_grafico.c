#include "ventana.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <id_ventana>\n", argv[0]);
        return 1;
    }

    char *fin;
    long id = strtol(argv[1], &fin, 10);
    if (*fin != '\0' || id <= 0) {
        fprintf(stderr, "Error: <id_ventana> debe ser un entero positivo, recibido: '%s'\n", argv[1]);
        return 1;
    }

    return ejecutar_ventana((int)id);
}
