# Agentic-OS — Avance 2

## Requisitos
- Linux con sesion grafica (X11, o Wayland con XWayland — en Ubuntu de
  escritorio esto ya viene activo por defecto; en WSL se necesita WSLg
  o un servidor X externo como VcXsrv).
- gcc y make.
- Cabeceras de desarrollo de X11:

    sudo apt-get update
    sudo apt-get install build-essential libx11-dev

## Compilacion

    make

Genera tres binarios en bin/:
- bin/launcher        -> consola interactiva, crea y vigila ventanas
- bin/cliente_grafico -> ventana X11 (la invoca el launcher; no se corre a mano normalmente)
- bin/ia_learner      -> servidor que recibe las palabras tecleadas en cada ventana

## Ejecucion

Hay que levantar primero el servidor, y despues el launcher (en otra
terminal o en segundo plano), porque cada ventana se conecta a
IALearner en el momento en que se crea.

Terminal 1:

    ./bin/ia_learner

Por defecto escucha en el puerto 5050. Para usar otro puerto:

    ./bin/ia_learner 6000

Terminal 2:

    ./bin/launcher

Por defecto las ventanas se conectan a 127.0.0.1:5050. Para apuntar a
otro host/puerto (por ejemplo si IALearner corre en otra maquina o en
otro puerto):

    ./bin/launcher 127.0.0.1 6000

Desde el menu del launcher: crea ventanas, revisa su estado
(EJECUTANDO/TERMINADO), cierra todas o sal. En cada ventana, las
palabras completas (separadas por espacio o por Enter) se imprimen
tanto en la terminal del launcher como en la terminal donde corre
ia_learner. Cierra una ventana con Escape o con el boton X.

## Protocolo (avance 2)

Cada ventana abre una conexion TCP persistente con IALearner y envia
una linea de texto por mensaje:

1. Al conectar: `VENTANA <id>` (una sola vez, para identificarse).
2. Por cada palabra que el usuario termina de escribir (al presionar
   espacio o Enter): la palabra tal cual, sin espacios.
3. Al presionar Enter: el token `<ENTER>`, que marca el fin de una
   oracion.

IALearner en este avance todavia es de un solo hilo: atiende una
conexion a la vez y solo imprime lo que recibe, para validar que la
comunicacion funciona de extremo a extremo.

## Limpieza

    make clean
