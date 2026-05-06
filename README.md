# Simulacion de Paginacion y Segmentacion con Sincronizacion de Procesos

**Curso:** Sistemas Operativos (Escuela de Computacion)  
**Institucion:** Instituto Tecnologico de Costa Rica (ITCR)  
**Profesora:** Ing. Erika Marin Schumann  
**Semestre:** I Semestre 2026  

## Integrantes

| Nombre                      | Carnet     |
|-----------------------------|------------|
| Daniel Aleman Ruiz          | 2023051957 |
| Joseph Arrieta Mora         | 2023020875 |
| Sebastian Rodriguez Sanchez | 2023020875 |

---

## Descripcion General

Este proyecto implementa una simulacion de asignacion de memoria a procesos mediante los esquemas de **Paginacion** y **Segmentacion**, con sincronizacion de procesos a traves de semaforos POSIX con nombre. El sistema esta compuesto por cuatro programas independientes que se comunican entre si mediante memoria compartida System V (`shmget`) y un semaforo nombrado (`sem_open`), lo que permite la coordinacion entre procesos separados.

El **Productor** genera procesos (hilos) de forma continua con parametros aleatorios. Cada proceso compite por espacio en la memoria compartida, donde solo uno puede ejecutar el algoritmo de busqueda a la vez. El **Espia** permite consultar el estado del sistema en cualquier momento, y el **Finalizador** se encarga de liberar todos los recursos y cerrar la bitacora.

---

## Estructura del Proyecto

```
/Proyecto2-IC6600
├── Makefile          # Compila los cuatro programas del sistema
├── README.md         # Instrucciones de compilacion y ejecucion
├── /src              # Todo el codigo fuente
│   ├── common.h      # Estructuras y constantes compartidas entre programas
│   ├── ui.h          # Cabeceras para la interfaz de usuario (colores, banners)
│   ├── ui.c          # Implementacion de funciones de impresion en consola
│   ├── inicializador.c  # Crea y configura la memoria compartida y el semaforo
│   ├── productor.c      # Genera procesos (hilos) y gestiona la asignacion de memoria
│   ├── espia.c          # Consulta y muestra el estado del sistema en tiempo real
│   └── finalizador.c    # Libera recursos y cierra la bitacora
├── /bin              # Ejecutables generados por el Makefile
└── /logs             # Bitacora de eventos (generada automaticamente)
    └── bitacora.txt
```

---

## Requisitos del Sistema

- Sistema operativo: Linux
- Compilador: GCC con soporte para C99 o superior
- Librerias: PThreads y librt (incluidas en la mayoria de distribuciones Linux)

---

## Compilacion

Desde la raiz del proyecto, ejecutar:

```bash
make
```

Esto compilara los cuatro programas y depositara los ejecutables en el directorio `/bin`.

Para limpiar los ejecutables y la bitacora:

```bash
make clean
```

---

## Ejecucion

El sistema debe iniciarse y detenerse en el orden correcto. Se recomienda usar terminales separadas para el Productor y el Espia.

### 1. Inicializador

Debe ejecutarse primero. Crea la memoria compartida, inicializa el semaforo y limpia cualquier registro previo de la bitácora para iniciar un nuevo ambiente. El programa solicita la cantidad de espacios de memoria y muere al terminar.

```bash
./bin/inicializador
```

Ejemplo de interaccion:

```
Ingrese la cantidad de paginas o espacios de memoria para la simulacion: 20
```

### 2. Productor

Se ejecuta despues del inicializador. Solicita el esquema de simulacion y comienza a generar procesos de forma continua con intervalos aleatorios de 30 a 60 segundos. Mantener en ejecucion hasta que se desee finalizar la simulacion.

```bash
./bin/productor
```

Ejemplo de interaccion:

```
¿Con que algoritmo desea correr la simulacion?
1. Paginacion
2. Segmentacion
Elija (1 o 2): 1
```

Para detener el productor, presionar `Ctrl+C` o ejecutar el Finalizador.

### 3. Espia

Puede ejecutarse en cualquier momento mientras el sistema este corriendo. Toma un snapshot del estado actual de la memoria y los procesos, lo imprime y termina.

```bash
./bin/espia
```

La salida muestra:

- Procesos buscando espacio en memoria (maximo 1 a la vez)
- Procesos en memoria (durmiendo)
- Procesos bloqueados esperando la region critica
- Procesos muertos por falta de espacio
- Procesos que terminaron exitosamente
- Mapa visual de la memoria con los PIDs asignados a cada bloque

### 4. Finalizador

Debe ejecutarse al terminar la simulacion. Mata el proceso del productor, libera la memoria compartida, destruye el semaforo y sella la bitacora.

```bash
./bin/finalizador
```

---

## Ejemplo de Uso Completo

**Terminal 1 — Inicializar el ambiente:**

```bash
./bin/inicializador
# Ingresar: 20
```

**Terminal 2 — Iniciar el productor:**

```bash
./bin/productor
# Seleccionar: 1 (Paginacion) o 2 (Segmentacion)
```

**Terminal 3 — Consultar el estado en cualquier momento:**

```bash
./bin/espia
```

**Terminal 3 — Al finalizar, limpiar el sistema:**

```bash
./bin/finalizador
```

---

## Bitacora de Eventos

La bitacora se genera automaticamente en `logs/bitacora.txt` y se resetea cada vez que se ejecuta el inicializador. Registra cada evento con el siguiente formato:

```
[HH:MM:SS] PID: <pid> | Accion: <accion> | Tipo: <tipo> | Espacios: <espacios>
```

Ejemplos de entradas:

```
[14:23:01] PID: 1001 | Accion: Entra a Memoria | Tipo: Paginacion | Espacios: 0 3 7 
[14:23:45] PID: 1001 | Accion: Sale de Memoria  | Tipo: Desasignacion | Espacios: 0 3 7 
[14:24:10] PID: 1002 | Accion: Muere - Sin Espacio | Tipo: Paginacion | Espacios: N/A
[14:55:00] === SIMULACION FINALIZADA POR EL FINALIZADOR ===
```
