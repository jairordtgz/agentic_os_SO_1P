# Agentic-OS — Avance 1

## Requisitos
- Linux con sesión gráfica (X11 o Wayland con XWayland, que viene activado
  por defecto en Ubuntu de escritorio).
- gcc y make.
- Cabeceras de desarrollo de X11:

    sudo apt-get update
    sudo apt-get install build-essential libx11-dev

## Compilación

    make

Genera dos binarios en bin/:
- bin/launcher        -> consola interactiva, crea y vigila ventanas
- bin/cliente_grafico -> ventana X11 (la invoca el launcher; no se corre a mano)

## Ejecución

    ./bin/launcher

Desde el menú: crea ventanas, revisa su estado (EJECUTANDO/TERMINADO),
cierra todas o sal. En cada ventana, las teclas presionadas se imprimen
en la terminal (en este avance todavía no se envían por red). Cierra
una ventana con Escape o con el botón X.

## Limpieza

    make clean
