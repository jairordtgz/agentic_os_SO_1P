#include "network_client.h"
#include "protocolo.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

int conectar_a_ialearner(const char *host, const char *puerto) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *resultados = NULL;
    int rc = getaddrinfo(host, puerto, &hints, &resultados);
    if (rc != 0) {
        fprintf(stderr, "No se pudo resolver %s:%s (%s)\n", host, puerto, gai_strerror(rc));
        return -1;
    }

    int sockfd = -1;
    for (struct addrinfo *ai = resultados; ai != NULL; ai = ai->ai_next) {
        sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sockfd < 0) {
            continue;
        }
        if (connect(sockfd, ai->ai_addr, ai->ai_addrlen) == 0) {
            break;
        }
        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(resultados);

    if (sockfd < 0) {
        fprintf(stderr, "No se pudo conectar a IALearner en %s:%s\n", host, puerto);
        return -1;
    }
    return sockfd;
}

static int enviar_linea(int sockfd, const char *texto) {
    char mensaje[PROTOCOLO_LONGITUD_MAX_MENSAJE];
    int n = snprintf(mensaje, sizeof(mensaje), "%s\n", texto);
    if (n < 0 || (size_t)n >= sizeof(mensaje)) {
        return -1;
    }

    size_t enviados = 0;
    size_t total = (size_t)n;
    while (enviados < total) {
        ssize_t r = send(sockfd, mensaje + enviados, total - enviados, 0);
        if (r < 0) {
            perror("send");
            return -1;
        }
        enviados += (size_t)r;
    }
    return 0;
}

int enviar_identificacion(int sockfd, int id_ventana) {
    char linea[64];
    snprintf(linea, sizeof(linea), "%s%d", PROTOCOLO_PREFIJO_VENTANA, id_ventana);
    return enviar_linea(sockfd, linea);
}

int enviar_palabra(int sockfd, const char *palabra) {
    if (palabra[0] == '\0') {
        return 0;
    }
    return enviar_linea(sockfd, palabra);
}

int enviar_fin_linea(int sockfd) {
    return enviar_linea(sockfd, PROTOCOLO_FIN_LINEA);
}

void cerrar_conexion(int sockfd) {
    if (sockfd >= 0) {
        close(sockfd);
    }
}
