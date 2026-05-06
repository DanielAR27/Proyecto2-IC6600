#ifndef UI_H
#define UI_H

#include <stdio.h>

// Códigos de escape ANSI 
#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"   // Para creaciones y asignaciones exitosas
#define COLOR_RED     "\x1b[31m"   // Para procesos muertos por falta de espacio
#define COLOR_YELLOW  "\x1b[33m"   // Para procesos bloqueados/esperando
#define COLOR_CYAN    "\x1b[36m"   // Para información general del sistema
#define COLOR_MAGENTA "\x1b[35m"   // Para el ASCII Art y títulos

// Funciones de impresión
void print_info(const char *mensaje);
void print_exito(const char *mensaje);
void print_error(const char *mensaje);
void print_advertencia(const char *mensaje);

void print_banner(void);

// ASCII para los modos de simulación
void print_paginacion(void);
void print_segmentacion(void);

#endif