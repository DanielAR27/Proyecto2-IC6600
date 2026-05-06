#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "common.h"
#include "ui.h"

// Variables globales para los hilos
MemoriaCompartida *memoria;
sem_t *sem_mutex;
int modo_actual;

// Estructura para pasarle datos a cada hilo creado
typedef struct {
    int pid; 
    int cant_unidades; 
    int tam_segmentos[5]; 
    int tiempo_sleep;
    int indice_registro; // Índice asignado por el proceso principal
} DatosProceso;

// Función auxiliar para registrar en bitacora.txt
void escribir_bitacora(int pid, const char *accion, const char *tipo, const char *espacios) {
    FILE *file = fopen("logs/bitacora.txt", "a");
    if (file != NULL) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(file, "[%02d:%02d:%02d] PID: %d | Acción: %s | Tipo: %s | Espacios: %s\n",
                tm.tm_hour, tm.tm_min, tm.tm_sec, pid, accion, tipo, espacios);
        fclose(file);
    }
}

// ---------------------------------------------------------
// LÓGICA DEL HILO (Proceso)
// ---------------------------------------------------------
void *hilo_proceso(void *arg) {
    DatosProceso *datos = (DatosProceso *)arg;
    int pid = datos->pid;
    int indice = datos->indice_registro; 
    int espacios_asignados[MAX_MEM_SPACES];
    int asignados_count = 0;
    char buffer_espacios[256] = "";

    // Pedir semáforo de memoria (El estado ya es ESTADO_BLOQUEADO)
    sem_wait(sem_mutex); 
    
    // printf("PID %d agarró el semáforo y se quedó dormido con la llave!\n", pid); // TESTING
    // sleep(10); // TESTING

    // Buscar su ubicación
    if(indice != -1) memoria->procesos[indice].estado = ESTADO_BUSCANDO;
    int espacio_encontrado = 1;

    if (modo_actual == MODO_PAGINACION) {
        for (int i = 0; i < memoria->total_espacios && asignados_count < datos->cant_unidades; i++) {
            if (memoria->mapa[i].libre) {
                espacios_asignados[asignados_count++] = i;
            }
        }
        if (asignados_count < datos->cant_unidades) espacio_encontrado = 0;

    } else if (modo_actual == MODO_SEGMENTACION) {
        for (int s = 0; s < datos->cant_unidades; s++) {
            int tam_necesario = datos->tam_segmentos[s];
            int contiguos = 0;
            int inicio_bloque = -1;

            for (int i = 0; i < memoria->total_espacios; i++) {
                if (memoria->mapa[i].libre) {
                    if (contiguos == 0) inicio_bloque = i;
                    contiguos++;
                    if (contiguos == tam_necesario) {
                        for (int j = 0; j < tam_necesario; j++) {
                            espacios_asignados[asignados_count++] = inicio_bloque + j;
                            memoria->mapa[inicio_bloque + j].libre = 0; 
                        }
                        break;
                    }
                } else {
                    contiguos = 0; 
                }
            }
            if (contiguos < tam_necesario) {
                espacio_encontrado = 0;
                break;
            }
        }
    }

    if (espacio_encontrado) {
        // Consolidar la asignación
        for (int i = 0; i < asignados_count; i++) {
            memoria->mapa[espacios_asignados[i]].libre = 0;
            memoria->mapa[espacios_asignados[i]].pid = pid;
            char num[10];
            sprintf(num, "%d ", espacios_asignados[i]);
            strcat(buffer_espacios, num);
        }
        
        // Escribir en Bitácora
        escribir_bitacora(pid, "Entra a Memoria", modo_actual == MODO_PAGINACION ? "Paginación" : "Segmentación", buffer_espacios);
        
        // Devolver semáforo de memoria
        sem_post(sem_mutex);

        // Sleep
        if(indice != -1) memoria->procesos[indice].estado = ESTADO_DURMIENDO;
        sleep(datos->tiempo_sleep);

        // Pedir Semáforo de memoria (Se marca como bloqueado antes de pedirlo)
        if(indice != -1) memoria->procesos[indice].estado = ESTADO_BLOQUEADO; 
        sem_wait(sem_mutex);
        
        // Liberar memoria
        for (int i = 0; i < asignados_count; i++) {
            memoria->mapa[espacios_asignados[i]].libre = 1;
            memoria->mapa[espacios_asignados[i]].pid = 0;
        }
        if(indice != -1) memoria->procesos[indice].estado = ESTADO_TERMINADO;

        // Escribir en Bitácora
        escribir_bitacora(pid, "Sale de Memoria", "Desasignación", buffer_espacios);
        
        // Devolver semáforo de memoria
        sem_post(sem_mutex);

    } else {
        // Revertir ocupaciones temporales si falló la segmentación
        if (modo_actual == MODO_SEGMENTACION) {
            for (int i = 0; i < asignados_count; i++) {
                memoria->mapa[espacios_asignados[i]].libre = 1;
            }
        }
        
        if(indice != -1) memoria->procesos[indice].estado = ESTADO_MUERTO;
        escribir_bitacora(pid, "Muere - Sin Espacio", modo_actual == MODO_PAGINACION ? "Paginación" : "Segmentación", "N/A");
        sem_post(sem_mutex);
    }

    free(datos);
    return NULL;
}

// ---------------------------------------------------------
// HILO PRINCIPAL (Generador)
// ---------------------------------------------------------
int main() {
    srand(time(NULL));
    print_banner();

    // Solicitar algoritmo al usuario
    printf("¿Con qué algoritmo desea correr la simulación?\n");
    printf("1. Paginación\n");
    printf("2. Segmentación\n");
    printf("Elija (1 o 2): ");
    if (scanf("%d", &modo_actual) != 1 || (modo_actual != 1 && modo_actual != 2)) {
        print_error("Opción inválida.");
        return 1;
    }

    if (modo_actual == MODO_PAGINACION) {
        print_paginacion();
    } else {
        print_segmentacion();
    }

    // Conectar a la memoria compartida y semáforo
    int shmid = shmget(SHM_KEY, sizeof(MemoriaCompartida), 0666);
    if (shmid < 0) {
        print_error("No se encontró la memoria compartida. Ejecute el inicializador.");
        return 1;
    }
    memoria = (MemoriaCompartida *)shmat(shmid, NULL, 0);
    memoria->modo_simulacion = modo_actual;

    sem_mutex = sem_open(SEM_MUTEX_NAME, 0);
    if (sem_mutex == SEM_FAILED) {
        print_error("No se pudo abrir el semáforo.");
        return 1;
    }

    system("mkdir -p logs");

    int contador_pid = 1000; 
    print_info("Iniciando generación de procesos... (Presione Ctrl+C para detener)");

    // Ciclo infinito de creación de hilos
    while (1) {
        int tiempo_creacion = (rand() % 31) + 30; // 30s - 60s

        // int tiempo_creacion = (rand() % 3) + 1; // MODO TESTING
        
        printf("\nEsperando %d segundos para crear el siguiente proceso...\n", tiempo_creacion);
        sleep(tiempo_creacion);

        DatosProceso *datos = malloc(sizeof(DatosProceso));
        datos->pid = contador_pid++;
        datos->tiempo_sleep = (rand() % 41) + 20; // 20s - 60s

        // datos->tiempo_sleep = (rand() % 11) + 5; // MODO TESTING

        if (modo_actual == MODO_PAGINACION) {
            datos->cant_unidades = (rand() % 10) + 1; // 1 a 10 páginas
            printf("Generando Proceso PID %d | Paginación: %d páginas | Tiempo: %ds\n", 
                   datos->pid, datos->cant_unidades, datos->tiempo_sleep);
        } else {
            datos->cant_unidades = (rand() % 5) + 1; // 1 a 5 segmentos
            printf("Generando Proceso PID %d | Segmentación: %d segmentos | Tiempo: %ds\n", 
                   datos->pid, datos->cant_unidades, datos->tiempo_sleep);
            for (int i = 0; i < datos->cant_unidades; i++) {
                datos->tam_segmentos[i] = (rand() % 3) + 1;
            }
        }

        // Buscar un campo libre en el registro y marcar como bloqueado antes de crear el hilo
        int indice_libre = -1;
        for(int i = 0; i < MAX_PROCESSES; i++) {
            if(memoria->procesos[i].pid == 0) {
                indice_libre = i;
                memoria->procesos[i].pid = datos->pid;
                memoria->procesos[i].estado = ESTADO_BLOQUEADO; 
                break;
            }
        }
        datos->indice_registro = indice_libre;

        pthread_t hilo;
        pthread_create(&hilo, NULL, hilo_proceso, datos);
        pthread_detach(hilo);
    }

    return 0;
}