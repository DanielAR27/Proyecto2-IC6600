# Compilador y banderas
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
# -lrt es necesario en algunos sistemas Linux para memoria compartida POSIX
LDFLAGS = -lrt 

# Directorios
SRC_DIR = src
BIN_DIR = bin

# Ejecutables a generar
TARGETS = $(BIN_DIR)/inicializador $(BIN_DIR)/productor $(BIN_DIR)/espia $(BIN_DIR)/finalizador

all: $(TARGETS)

$(BIN_DIR)/inicializador: $(SRC_DIR)/inicializador.c $(SRC_DIR)/ui.c $(SRC_DIR)/common.h $(SRC_DIR)/ui.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/inicializador.c $(SRC_DIR)/ui.c $(LDFLAGS)

$(BIN_DIR)/productor: $(SRC_DIR)/productor.c $(SRC_DIR)/ui.c $(SRC_DIR)/common.h $(SRC_DIR)/ui.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/productor.c $(SRC_DIR)/ui.c $(LDFLAGS)

$(BIN_DIR)/espia: $(SRC_DIR)/espia.c $(SRC_DIR)/ui.c $(SRC_DIR)/common.h $(SRC_DIR)/ui.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/espia.c $(SRC_DIR)/ui.c $(LDFLAGS)

$(BIN_DIR)/finalizador: $(SRC_DIR)/finalizador.c $(SRC_DIR)/ui.c $(SRC_DIR)/common.h $(SRC_DIR)/ui.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/finalizador.c $(SRC_DIR)/ui.c $(LDFLAGS)

# Regla para "matar" los binarios, borrar la carpeta bin entera y limpiar logs
clean:
	rm -rf $(BIN_DIR)
	rm -f logs/bitacora.txt

.PHONY: all clean