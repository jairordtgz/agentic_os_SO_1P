#include "proceso.h"
#include <stdio.h>
#include <stdlib.h>

TablaProcesos *tabla_crear(int capacidad_inicial) {
    if (capacidad_inicial <= 0) {
        capacidad_inicial = 4;
    }
    TablaProcesos *tabla = malloc(sizeof(TablaProcesos));
    if (!tabla) {
        return NULL;
    }
    tabla->procesos = malloc(sizeof(Proceso) * (size_t)capacidad_inicial);
    if (!tabla->procesos) {
        free(tabla);
        return NULL;
    }
    tabla->cantidad = 0;
    tabla->capacidad = capacidad_inicial;
    return tabla;
}

static int tabla_expandir(TablaProcesos *tabla) {
    int nueva_capacidad = tabla->capacidad * 2;
    Proceso *nuevo = realloc(tabla->procesos, sizeof(Proceso) * (size_t)nueva_capacidad);
    if (!nuevo) {
        return -1;
    }
    tabla->procesos = nuevo;
    tabla->capacidad = nueva_capacidad;
    return 0;
}

int tabla_agregar(TablaProcesos *tabla, int id_ventana, pid_t pid) {
    if (!tabla) return -1;
    if (tabla->cantidad == tabla->capacidad) {
        if (tabla_expandir(tabla) != 0) {
            return -1;
        }
    }
    Proceso *p = &tabla->procesos[tabla->cantidad];
    p->id_ventana = id_ventana;
    p->pid = pid;
    p->estado = PROC_EJECUTANDO;
    p->codigo_salida = 0;
    tabla->cantidad++;
    return 0;
}

int tabla_marcar_terminado(TablaProcesos *tabla, pid_t pid, int codigo_salida) {
    if (!tabla) return -1;
    for (int i = 0; i < tabla->cantidad; i++) {
        if (tabla->procesos[i].pid == pid) {
            tabla->procesos[i].estado = PROC_TERMINADO;
            tabla->procesos[i].codigo_salida = codigo_salida;
            return 0;
        }
    }
    return -1;
}

int tabla_contar_activos(const TablaProcesos *tabla) {
    if (!tabla) return 0;
    int activos = 0;
    for (int i = 0; i < tabla->cantidad; i++) {
        if (tabla->procesos[i].estado == PROC_EJECUTANDO) {
            activos++;
        }
    }
    return activos;
}

void tabla_imprimir(const TablaProcesos *tabla) {
    if (!tabla || tabla->cantidad == 0) {
        printf("  (no hay ventanas registradas)\n");
        return;
    }
    printf("  %-10s %-10s %-12s %s\n", "Ventana", "PID", "Estado", "Cod.Salida");
    for (int i = 0; i < tabla->cantidad; i++) {
        const Proceso *p = &tabla->procesos[i];
        printf("  %-10d %-10ld %-12s %d\n",
               p->id_ventana,
               (long)p->pid,
               p->estado == PROC_EJECUTANDO ? "EJECUTANDO" : "TERMINADO",
               p->codigo_salida);
    }
}

void tabla_destruir(TablaProcesos *tabla) {
    if (!tabla) return;
    free(tabla->procesos);
    free(tabla);
}
