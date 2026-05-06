#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <unistd.h>

// Definiciones generales para evitar números mágicos en el código
#define MAX_MEM_SPACES 1000
#define MAX_PROCESSES 1000

// Llave para la Memoria Compartida 
#define SHM_KEY 0x6600

// Nombre del semáforo POSIX para proteger la memoria (Región Crítica)
#define SEM_MUTEX_NAME "/sem_memoria_mutex"

// Modos de simulación
#define MODO_PAGINACION 1
#define MODO_SEGMENTACION 2

// Estados posibles de un proceso, tal como los pide el Programa Espía
typedef enum {
    ESTADO_BLOQUEADO,   // Esperando por la región crítica 
    ESTADO_BUSCANDO,    // Buscando espacio en la memoria
    ESTADO_DURMIENDO,   // En la memoria ocupando espacio (sleep)
    ESTADO_MUERTO,      // Murió por no haber espacio suficiente
    ESTADO_TERMINADO    // Terminó su ejecución y liberó recursos
} EstadoProceso;

// Estructura para llevar el control de cada proceso en el sistema
typedef struct {
    pid_t pid;
    EstadoProceso estado;
} InfoProceso;

// Estructura que representa una "Página" o "Espacio de Memoria"
typedef struct {
    int libre;          // 1 si está libre, 0 si está ocupado
    pid_t pid;          // PID del proceso que es dueño de este bloque
    int id_logico;      // ID de la página o segmento asignado
} BloqueMemoria;

// Lo que va a vivir en la Memoria Compartida
typedef struct {
    int total_espacios;                 // Definido por el Inicializador
    int modo_simulacion;                // Definido por el Productor (Paginación o Segmentación)
    int cant_procesos_activos;          // Contador para el arreglo de procesos
    
    InfoProceso procesos[MAX_PROCESSES]; // Registro para que el Espía sepa qué pasa
    BloqueMemoria mapa[MAX_MEM_SPACES];  // El mapa físico de la memoria
} MemoriaCompartida;

#endif