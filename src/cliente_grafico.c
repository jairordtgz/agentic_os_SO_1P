#include "ventana.h"
#include "protocolo.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Uso: %s <id_ventana> [host] [puerto]\n", argv[0]);
        return 1;
    }

    char *fin;
    long id = strtol(argv[1], &fin, 10);
    if (*fin != '\0' || id <= 0) {
        fprintf(stderr, "Error: <id_ventana> debe ser un entero positivo, recibido: '%s'\n", argv[1]);
        return 1;
    }

    const char *host   = (argc >= 3) ? argv[2] : IALEARNER_HOST_DEFECTO;
    const char *puerto = (argc >= 4) ? argv[3] : IALEARNER_PUERTO_DEFECTO;

    return ejecutar_ventana((int)id, host, puerto);
}
