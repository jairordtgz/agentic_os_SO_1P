#include "ventana.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>

int ejecutar_ventana(int id_ventana) {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "[ventana %d] No se pudo conectar al servidor X11 (revisa $DISPLAY)\n", id_ventana);
        return 1;
    }

    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        10, 10, 400, 200,
        1,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );

    char titulo[64];
    snprintf(titulo, sizeof(titulo), "Ventana %d", id_ventana);
    XStoreName(display, window, titulo);

    XSelectInput(display, window, ExposureMask | KeyPressMask);

    /* Le pedimos al window manager que nos avise con un evento en vez
       de matar el proceso cuando el usuario presiona la X. */
    Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete, 1);

    XMapWindow(display, window);

    int salir = 0;
    int codigo_salida = 0;
    XEvent event;

    while (!salir) {
        XNextEvent(display, &event);

        if (event.type == KeyPress) {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            char *nombre = XKeysymToString(keysym);

            if (nombre) {
                printf("[ventana %d] tecla: %s\n", id_ventana, nombre);
                fflush(stdout);
            }
            if (keysym == XK_Escape) {
                salir = 1;
            }
        } else if (event.type == ClientMessage) {
            if ((Atom)event.xclient.data.l[0] == wm_delete) {
                salir = 1;
            }
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return codigo_salida;
}
