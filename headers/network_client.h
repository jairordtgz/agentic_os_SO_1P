#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

int conectar_a_ialearner(const char *host, const char *puerto);
int enviar_identificacion(int sockfd, int id_ventana);
int enviar_palabra(int sockfd, const char *palabra);
int enviar_fin_linea(int sockfd);
void cerrar_conexion(int sockfd);

#endif
