#ifndef PROCESO_H
#define PROCESO_H

#include <sys/types.h>

typedef enum {
    PROC_EJECUTANDO,
    PROC_TERMINADO
} EstadoProceso;

typedef struct {
    int id_ventana;
    pid_t pid;
    EstadoProceso estado;
    int codigo_salida;
} Proceso;

typedef struct {
    Proceso *procesos;
    int cantidad;
    int capacidad;
} TablaProcesos;

TablaProcesos *tabla_crear(int capacidad_inicial);
int  tabla_agregar(TablaProcesos *tabla, int id_ventana, pid_t pid);
int  tabla_marcar_terminado(TablaProcesos *tabla, pid_t pid, int codigo_salida);
int  tabla_contar_activos(const TablaProcesos *tabla);
void tabla_imprimir(const TablaProcesos *tabla);
void tabla_destruir(TablaProcesos *tabla);

#endif
