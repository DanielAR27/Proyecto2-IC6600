#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include "common.h"
#include "ui.h"

int main()
{
    print_banner();
    print_info("Iniciando configuración del ambiente...");


    // Resetear/Limpiar la bitácora vieja al iniciar un nuevo ambiente
    FILE *file = fopen("logs/bitacora.txt", "w");
    if (file != NULL) fclose(file);
    print_info("Bitácora reseteada y lista para la nueva simulación.");

    int total_espacios;
    printf("Ingrese la cantidad de páginas o espacios de memoria para la simulación: ");
    if (scanf("%d", &total_espacios) != 1 || total_espacios <= 0 || total_espacios > MAX_MEM_SPACES)
    {
        print_error("Cantidad inválida. Debe ser un número mayor a 0 y menor al límite máximo.");
        return 1;
    }

    // Se crea  la memoria COMPARTIDA
    print_info("Solicitando memoria compartida al SO...");
    // IPC_CREAT crea el segmento si no existe, 0666 da permisos de lectura/escritura
    int shmid = shmget(SHM_KEY, sizeof(MemoriaCompartida), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        print_error("Fallo al crear la memoria compartida (shmget).");
        return 1;
    }

    // Adjuntar la memoria al espacio de direcciones del proceso
    MemoriaCompartida *memoria = (MemoriaCompartida *)shmat(shmid, NULL, 0);
    if (memoria == (MemoriaCompartida *)-1)
    {
        print_error("Fallo al adjuntar la memoria compartida (shmat).");
        return 1;
    }

    // Formatear/Inicializar la estructura
    memoria->total_espacios = total_espacios;
    memoria->modo_simulacion = 0; // Aún no definido, el Productor decide esto
    memoria->cant_procesos_activos = 0;

    // Se limpia el mapa de memoria
    for (int i = 0; i < total_espacios; i++)
    {
        memoria->mapa[i].libre = 1;
        memoria->mapa[i].pid = 0;
        memoria->mapa[i].id_logico = -1;
    }

    // Se limpia el registro del Espía
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        memoria->procesos[i].pid = 0;
    }

    print_exito("Memoria compartida solicitada e inicializada.");

    // Se crea e inicializar Semáforo (POSIX)
    print_info("Creando semáforo de exclusión mutua...");

    // Como práctica, se intenta desvincularlo primero por si quedó colgado de una ejecución anterior fallida
    sem_unlink(SEM_MUTEX_NAME);

    // O_CREAT para crearlo, permisos 0666, y valor inicial 1 (Mutex = 1 porque un proceso pasa a la vez)
    sem_t *sem_mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);
    if (sem_mutex == SEM_FAILED)
    {
        print_error("Fallo al inicializar el semáforo (sem_open).");
        return 1;
    }

    print_exito("Semáforo configurado correctamente.");

    // Como parte de la limpieza, se desvincula la memoria y cierra el semáforo en este proceso
    shmdt(memoria);
    sem_close(sem_mutex);

    print_exito("El ambiente está listo. El Programa Inicializador muere en paz.");
    return 0;
}