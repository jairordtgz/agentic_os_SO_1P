#include "proceso.h"
#include "protocolo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <sys/wait.h>

#define MAX_VENTANAS_POR_LOTE 20

static volatile sig_atomic_t flag_hijo_terminado = 0;

static void manejador_sigchld(int sig) {
    (void)sig;
    flag_hijo_terminado = 1;
}

static void revisar_procesos_terminados(TablaProcesos *tabla) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        int codigo = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        tabla_marcar_terminado(tabla, pid, codigo);
        printf("\n[launcher] La ventana con PID %ld terminó (código %d)\n", (long)pid, codigo);
    }
}

static int obtener_ruta_cliente(char *buffer, size_t tam) {
    ssize_t n = readlink("/proc/self/exe", buffer, tam - 1);
    if (n < 0) {
        perror("readlink");
        return -1;
    }
    buffer[n] = '\0';

    char *ultimo_slash = strrchr(buffer, '/');
    if (!ultimo_slash) {
        return -1;
    }

    size_t dir_len = (size_t)(ultimo_slash - buffer);
    const char *nombre_cliente = "/cliente_grafico";
    if (dir_len + strlen(nombre_cliente) + 1 > tam) {
        return -1;
    }
    snprintf(buffer + dir_len, tam - dir_len, "%s", nombre_cliente);
    return 0;
}

/* CAMBIO: ahora recibe host/puerto y los reenvia a cliente_grafico. */
static int crear_ventana(TablaProcesos *tabla, int id_ventana, const char *ruta_cliente,
                          const char *host, const char *puerto) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        char id_str[16];
        snprintf(id_str, sizeof(id_str), "%d", id_ventana);
        execl(ruta_cliente, ruta_cliente, id_str, host, puerto, (char *)NULL);
        fprintf(stderr, "[launcher] No se pudo ejecutar '%s': %s\n", ruta_cliente, strerror(errno));
        _exit(127);
    }

    if (tabla_agregar(tabla, id_ventana, pid) != 0) {
        fprintf(stderr, "[launcher] No se pudo registrar la ventana %d\n", id_ventana);
    }
    return 0;
}

static int leer_linea(char *buffer, size_t tam) {
    if (!fgets(buffer, (int)tam, stdin)) {
        return -1;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return 0;
}

static int leer_entero_positivo(const char *prompt, int minimo, int maximo) {
    char linea[64];
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        if (leer_linea(linea, sizeof(linea)) != 0) {
            return -1;
        }
        char *fin;
        long valor = strtol(linea, &fin, 10);
        if (linea[0] == '\0' || *fin != '\0') {
            printf("  Entrada invalida, escribe solo un numero.\n");
            continue;
        }
        if (valor < minimo || valor > maximo) {
            printf("  El valor debe estar entre %d y %d.\n", minimo, maximo);
            continue;
        }
        return (int)valor;
    }
}

static void mostrar_menu(void) {
    printf("\n==== Agentic-OS Launcher ====\n");
    printf("1. Crear ventanas nuevas\n");
    printf("2. Ver estado de procesos\n");
    printf("3. Terminar todas las ventanas activas\n");
    printf("4. Salir\n");
    printf("Opcion: ");
    fflush(stdout);
}

static void terminar_todas(TablaProcesos *tabla) {
    for (int i = 0; i < tabla->cantidad; i++) {
        if (tabla->procesos[i].estado == PROC_EJECUTANDO) {
            kill(tabla->procesos[i].pid, SIGTERM);
        }
    }
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, 0)) > 0) {
        int codigo = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        tabla_marcar_terminado(tabla, pid, codigo);
    }
}

/* CAMBIO: main ahora acepta host/puerto de IALearner como argumentos
   opcionales: ./bin/launcher [host] [puerto] */
int main(int argc, char *argv[]) {
    const char *host_ialearner   = (argc >= 2) ? argv[1] : IALEARNER_HOST_DEFECTO;
    const char *puerto_ialearner = (argc >= 3) ? argv[2] : IALEARNER_PUERTO_DEFECTO;

    struct sigaction sa;
    sa.sa_handler = manejador_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    char ruta_cliente[PATH_MAX];
    if (obtener_ruta_cliente(ruta_cliente, sizeof(ruta_cliente)) != 0) {
        fprintf(stderr, "No se pudo determinar la ruta de cliente_grafico\n");
        return 1;
    }

    TablaProcesos *tabla = tabla_crear(8);
    if (!tabla) {
        fprintf(stderr, "No se pudo inicializar la tabla de procesos\n");
        return 1;
    }

    printf("Las ventanas se conectaran a IALearner en %s:%s\n", host_ialearner, puerto_ialearner);

    int siguiente_id = 1;
    int salir_programa = 0;

    while (!salir_programa) {
        if (flag_hijo_terminado) {
            flag_hijo_terminado = 0;
            revisar_procesos_terminados(tabla);
        }

        mostrar_menu();
        char linea[16];
        if (leer_linea(linea, sizeof(linea)) != 0) {
            break;
        }

        char *fin;
        long opcion = strtol(linea, &fin, 10);
        if (linea[0] == '\0' || *fin != '\0') {
            printf("Opcion invalida.\n");
            continue;
        }

        switch (opcion) {
            case 1: {
                int n = leer_entero_positivo("Cuantas ventanas deseas crear? ", 1, MAX_VENTANAS_POR_LOTE);
                if (n < 0) { salir_programa = 1; break; }
                for (int i = 0; i < n; i++) {
                    crear_ventana(tabla, siguiente_id, ruta_cliente, host_ialearner, puerto_ialearner);
                    siguiente_id++;
                }
                break;
            }
            case 2:
                tabla_imprimir(tabla);
                break;
            case 3:
                terminar_todas(tabla);
                break;
            case 4:
                salir_programa = 1;
                break;
            default:
                printf("Opcion invalida.\n");
        }
    }

    terminar_todas(tabla);
    tabla_destruir(tabla);
    printf("Launcher finalizado.\n");
    return 0;
}
