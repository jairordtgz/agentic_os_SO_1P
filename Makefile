CC := gcc
CFLAGS := -Wall -Wextra -g -std=c11 -D_POSIX_C_SOURCE=200809L -Iheaders
LDLIBS_CLIENTE := -lX11

BIN_DIR := bin
OBJ_DIR := obj
SRC_DIR := src

LAUNCHER_SRCS   := $(SRC_DIR)/launcher.c $(SRC_DIR)/proceso.c
CLIENTE_SRCS    := $(SRC_DIR)/cliente_grafico.c $(SRC_DIR)/ventana.c $(SRC_DIR)/network_client.c
IALEARNER_SRCS  := $(SRC_DIR)/ialearner_main.c

LAUNCHER_OBJS   := $(LAUNCHER_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CLIENTE_OBJS    := $(CLIENTE_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
IALEARNER_OBJS  := $(IALEARNER_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(BIN_DIR)/launcher $(BIN_DIR)/cliente_grafico $(BIN_DIR)/ia_learner

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $(SRC_DIR)/$*.c

$(BIN_DIR)/launcher: $(LAUNCHER_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/cliente_grafico: $(CLIENTE_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS_CLIENTE)

$(BIN_DIR)/ia_learner: $(IALEARNER_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
