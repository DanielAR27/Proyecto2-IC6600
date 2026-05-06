#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "common.h"
#include "ui.h"

int main() {
    print_banner();

    // Conectar a la memoria compartida (Solo lectura visual, no se necesita el semáforo aquí
    // porque el espía solo toma un "screenshot" del estado actual).
    int shmid = shmget(SHM_KEY, sizeof(MemoriaCompartida), 0666);
    if (shmid < 0) {
        print_error("No se encontró la memoria compartida. ¿Está corriendo la simulación?");
        return 1;
    }
    
    MemoriaCompartida *memoria = (MemoriaCompartida *)shmat(shmid, NULL, 0);

    printf("%s==================================================%s\n", COLOR_MAGENTA, COLOR_RESET);
    printf("%s           REPORTE DEL PROGRAMA ESPÍA             %s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s==================================================%s\n\n", COLOR_MAGENTA, COLOR_RESET);

    // ESTADO DE LOS PROCESOS
    printf("%s>>> ESTADO DE LOS PROCESOS <<<%s\n", COLOR_YELLOW, COLOR_RESET);
    
    printf("\n%s[Buscando Espacio]%s (Solo puede haber 1):\n", COLOR_CYAN, COLOR_RESET);
    int buscando = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (memoria->procesos[i].pid != 0 && memoria->procesos[i].estado == ESTADO_BUSCANDO) {
            printf(" - PID: %d\n", memoria->procesos[i].pid);
            buscando++;
        }
    }
    if (buscando == 0) printf(" - Ninguno en este momento.\n");

    printf("\n%s[En Memoria (Durmiendo)]%s:\n", COLOR_GREEN, COLOR_RESET);
    int durmiendo = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (memoria->procesos[i].pid != 0 && memoria->procesos[i].estado == ESTADO_DURMIENDO) {
            printf(" - PID: %d\n", memoria->procesos[i].pid);
            durmiendo++;
        }
    }
    if (durmiendo == 0) printf(" - Ninguno en este momento.\n");

    printf("\n%s[Bloqueados (Esperando)]%s:\n", COLOR_YELLOW, COLOR_RESET);
    int bloqueados = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (memoria->procesos[i].pid != 0 && memoria->procesos[i].estado == ESTADO_BLOQUEADO) {
            printf(" - PID: %d\n", memoria->procesos[i].pid);
            bloqueados++;
        }
    }
    if (bloqueados == 0) printf(" - Ninguno en este momento.\n");

    printf("\n%s[Muertos (Sin Espacio)]%s:\n", COLOR_RED, COLOR_RESET);
    int muertos = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (memoria->procesos[i].pid != 0 && memoria->procesos[i].estado == ESTADO_MUERTO) {
            printf(" - PID: %d\n", memoria->procesos[i].pid);
            muertos++;
        }
    }
    if (muertos == 0) printf(" - Ninguno en este momento.\n");

    printf("\n%s[Terminados (Ejecución Exitosa)]%s:\n", COLOR_MAGENTA, COLOR_RESET);
    int terminados = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (memoria->procesos[i].pid != 0 && memoria->procesos[i].estado == ESTADO_TERMINADO) {
            printf(" - PID: %d\n", memoria->procesos[i].pid);
            terminados++;
        }
    }
    if (terminados == 0) printf(" - Ninguno en este momento.\n");

    // ESTADO DE LA MEMORIA (MAPA)
    printf("\n%s==================================================%s\n", COLOR_MAGENTA, COLOR_RESET);
    printf("%s>>> MAPA DE LA MEMORIA <<<%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("Modo: %s\n\n", memoria->modo_simulacion == MODO_PAGINACION ? "Paginación" : "Segmentación");

    int libres = 0;
    int ocupados = 0;

    // Imprimir cuadrícula
    for (int i = 0; i < memoria->total_espacios; i++) {
        if (memoria->mapa[i].libre) {
            printf("%s[----]%s ", COLOR_GREEN, COLOR_RESET);
            libres++;
        } else {
            printf("%s[%04d]%s ", COLOR_RED, memoria->mapa[i].pid, COLOR_RESET);
            ocupados++;
        }

        // Salto de línea cada 10 espacios para que no se rompa la consola
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }
    printf("\n\n");
    
    char resumen[100];
    sprintf(resumen, "Espacios Libres: %d | Espacios Ocupados: %d", libres, ocupados);
    print_info(resumen);

    // Desconectar de la memoria
    shmdt(memoria);

    return 0;
}