#include "ventana.h"
#include "network_client.h"
#include "protocolo.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>

int ejecutar_ventana(int id_ventana, const char *host, const char *puerto) {
    int sockfd = conectar_a_ialearner(host, puerto);
    if (sockfd < 0) {
        fprintf(stderr, "[ventana %d] No se pudo conectar a IALearner en %s:%s\n",
                id_ventana, host, puerto);
        return 1;
    }
    if (enviar_identificacion(sockfd, id_ventana) != 0) {
        fprintf(stderr, "[ventana %d] No se pudo enviar identificacion\n", id_ventana);
        cerrar_conexion(sockfd);
        return 1;
    }

    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "[ventana %d] No se pudo conectar al servidor X11 (revisa $DISPLAY)\n", id_ventana);
        cerrar_conexion(sockfd);
        return 1;
    }

    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(
        display, RootWindow(display, screen),
        10, 10, 400, 200, 1,
        BlackPixel(display, screen), WhitePixel(display, screen)
    );

    char titulo[64];
    snprintf(titulo, sizeof(titulo), "Ventana %d", id_ventana);
    XStoreName(display, window, titulo);

    XSelectInput(display, window, ExposureMask | KeyPressMask);

    Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete, 1);

    XMapWindow(display, window);

    char palabra_actual[PROTOCOLO_LONGITUD_MAX_PALABRA + 1];
    int longitud_palabra = 0;

    int salir = 0;
    int codigo_salida = 0;
    XEvent event;

    while (!salir) {
        XNextEvent(display, &event);

        if (event.type == KeyPress) {
            char buf[8];
            KeySym keysym;
            int n = XLookupString(&event.xkey, buf, (int)sizeof(buf) - 1, &keysym, NULL);

            if (keysym == XK_Escape) {
                salir = 1;
            } else if (keysym == XK_Return || keysym == XK_KP_Enter) {
                if (longitud_palabra > 0) {
                    palabra_actual[longitud_palabra] = '\0';
                    enviar_palabra(sockfd, palabra_actual);
                    longitud_palabra = 0;
                }
                enviar_fin_linea(sockfd);
                printf("[ventana %d] <enter>\n", id_ventana);
                fflush(stdout);
            } else if (keysym == XK_space) {
                if (longitud_palabra > 0) {
                    palabra_actual[longitud_palabra] = '\0';
                    enviar_palabra(sockfd, palabra_actual);
                    printf("[ventana %d] palabra: %s\n", id_ventana, palabra_actual);
                    fflush(stdout);
                    longitud_palabra = 0;
                }
            } else if (keysym == XK_BackSpace) {
                if (longitud_palabra > 0) {
                    longitud_palabra--;
                }
            } else if (n == 1 && (unsigned char)buf[0] >= 32 && (unsigned char)buf[0] < 127) {
                if (longitud_palabra < PROTOCOLO_LONGITUD_MAX_PALABRA) {
                    palabra_actual[longitud_palabra++] = buf[0];
                }
            }
        } else if (event.type == ClientMessage) {
            if ((Atom)event.xclient.data.l[0] == wm_delete) {
                salir = 1;
            }
        }
    }

    if (longitud_palabra > 0) {
        palabra_actual[longitud_palabra] = '\0';
        enviar_palabra(sockfd, palabra_actual);
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
    cerrar_conexion(sockfd);
    return codigo_salida;
}
