#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include "common.h"
#include "ui.h"

// Función auxiliar para registrar el cierre en bitacora.txt
void cerrar_bitacora() {
    FILE *file = fopen("logs/bitacora.txt", "a");
    if (file != NULL) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(file, "[%02d:%02d:%02d] === SIMULACIÓN FINALIZADA POR EL FINALIZADOR ===\n",
                tm.tm_hour, tm.tm_min, tm.tm_sec);
        fclose(file);
    }
}

int main() {
    print_banner();
    print_info("Iniciando secuencia de finalización y limpieza...");

    // Matar todos los procesos que estén en escena
    print_advertencia("Terminando los procesos del Productor e hilos activos...");
    // pkill envía una señal de terminación (SIGTERM) a todos los procesos con ese nombre
    int kill_status = system("pkill productor"); 
    if (kill_status == 0) {
        print_exito("Procesos del productor terminados con éxito.");
    } else {
        print_info("No se encontraron procesos del productor ejecutándose.");
    }

    // Devolver recursos: Memoria Compartida
    print_advertencia("Liberando la memoria compartida...");
    int shmid = shmget(SHM_KEY, sizeof(MemoriaCompartida), 0666);
    if (shmid >= 0) {
        // IPC_RMID marca el segmento para ser destruido por el sistema operativo
        if (shmctl(shmid, IPC_RMID, NULL) != -1) {
            print_exito("Memoria compartida devuelta al SO.");
        } else {
            print_error("Error al intentar destruir la memoria compartida.");
        }
    } else {
        print_info("La memoria compartida ya estaba liberada o no existía.");
    }

    // Devolver recursos: Semáforo POSIX
    print_advertencia("Desvinculando semáforos...");
    // sem_unlink elimina el nombre del semáforo y lo destruye cuando los procesos lo sueltan
    if (sem_unlink(SEM_MUTEX_NAME) == 0) {
        print_exito("Semáforo destruido correctamente.");
    } else {
        print_info("El semáforo ya estaba destruido o no existía.");
    }

    // Cerrar el archivo de la bitácora
    print_advertencia("Cerrando y sellando bitácora de eventos...");
    cerrar_bitacora();
    print_exito("Bitácora cerrada exitosamente.");

    printf("\n");
    print_exito("=== LIMPIEZA COMPLETADA. EL SISTEMA ESTÁ APAGADO ===");
    
    return 0;
}