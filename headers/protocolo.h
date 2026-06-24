#ifndef PROTOCOLO_H
#define PROTOCOLO_H

/* Valores por defecto, usados solo si nadie pasa host/puerto como
   parametro. Nunca se usan directamente "quemados" en otro archivo. */
#define IALEARNER_HOST_DEFECTO   "127.0.0.1"
#define IALEARNER_PUERTO_DEFECTO "5050"

/* Formato de los mensajes que viajan por el socket. */
#define PROTOCOLO_PREFIJO_VENTANA "VENTANA "
#define PROTOCOLO_FIN_LINEA       "<ENTER>"

#define PROTOCOLO_LONGITUD_MAX_PALABRA  63
#define PROTOCOLO_LONGITUD_MAX_MENSAJE 128

#endif
