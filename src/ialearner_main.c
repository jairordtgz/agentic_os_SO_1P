#include "protocolo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TAMANO_BUFFER_LINEA 256

static int crear_socket_escucha(const char *puerto) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    /* SO_REUSEADDR evita el error "Address already in use" si reinicias
       el servidor justo despues de cerrarlo (el puerto queda "ocupado"
       un rato por el sistema, aun sin conexiones activas). */
    int reutilizar = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reutilizar, sizeof(reutilizar)) < 0) {
        perror("setsockopt");
        close(sockfd);
        return -1;
    }

    struct sockaddr_in direccion;
    memset(&direccion, 0, sizeof(direccion));
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY; /* escucha en todas las interfaces */
    direccion.sin_port = htons((uint16_t)atoi(puerto));

    if (bind(sockfd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) {
        perror("bind");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 16) < 0) {
        perror("listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

/* TCP entrega un flujo de bytes, no mensajes: aqui reensamblamos ese
   flujo en lineas completas separadas por '\n' antes de imprimirlas. */
static void atender_cliente(int sockfd_cliente, struct sockaddr_in *direccion_cliente) {
    char ip_texto[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &direccion_cliente->sin_addr, ip_texto, sizeof(ip_texto));
    printf("[ialearner] Nueva conexion desde %s:%d\n", ip_texto, ntohs(direccion_cliente->sin_port));
    fflush(stdout);

    char buffer_linea[TAMANO_BUFFER_LINEA];
    int longitud_linea = 0;
    char byte;
    ssize_t leidos;

    while ((leidos = recv(sockfd_cliente, &byte, 1, 0)) > 0) {
        if (byte == '\n') {
            buffer_linea[longitud_linea] = '\0';
            printf("[ialearner] recibido: %s\n", buffer_linea);
            fflush(stdout);
            longitud_linea = 0;
        } else if (longitud_linea < (int)sizeof(buffer_linea) - 1) {
            buffer_linea[longitud_linea++] = byte;
        }
        /* Si una linea excede el buffer sin traer '\n', el exceso se
           descarta en vez de desbordar memoria (programacion defensiva). */
    }

    if (leidos < 0) {
        perror("recv");
    }
    printf("[ialearner] Conexion cerrada\n");
    fflush(stdout);
    close(sockfd_cliente);
}

int main(int argc, char *argv[]) {
    const char *puerto = (argc >= 2) ? argv[1] : IALEARNER_PUERTO_DEFECTO;

    /* Si el cliente cierra la conexion justo cuando le escribimos algo,
       el sistema manda SIGPIPE y por defecto mata el proceso. En este
       avance no le escribimos nada al cliente, pero lo dejamos
       ignorado desde ya para no arrastrar un bug silencioso despues. */
    signal(SIGPIPE, SIG_IGN);

    int sockfd_escucha = crear_socket_escucha(puerto);
    if (sockfd_escucha < 0) {
        return 1;
    }

    printf("[ialearner] Escuchando en el puerto %s...\n", puerto);
    fflush(stdout);

    while (1) {
        struct sockaddr_in direccion_cliente;
        socklen_t longitud_direccion = sizeof(direccion_cliente);

        int sockfd_cliente = accept(sockfd_escucha,
                                     (struct sockaddr *)&direccion_cliente,
                                     &longitud_direccion);
        if (sockfd_cliente < 0) {
            perror("accept");
            continue;
        }

        /* Avance 2: un solo hilo, una conexion a la vez. El avance 3
           reemplaza esto por un hilo nuevo por cada conexion. */
        atender_cliente(sockfd_cliente, &direccion_cliente);
    }

    close(sockfd_escucha);
    return 0;
}
