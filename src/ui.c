#include "ui.h"

void print_banner(void) {
    printf("%s", COLOR_MAGENTA);
    printf("  ____                   __  __     _   _    _         _      ____      U  ___ u   ____            U  ___ u  ____     \n");
    printf(" / __\"| u      ___     U|' \\/ '|uU |\"|u| |  |\"|     U  /\"\\  u |  _\"\\      \\/\"_ \\/U |  _\"\\ u         \\/\"_ \\/ / __\"| u  \n");
    printf("<\\___ \\/      |_\"_|    \\| |\\/| |/ \\| |\\| |U | | u   \\/ _ \\/ /| | | |     | | | | \\| |_) |/         | | | |<\\___ \\/   \n");
    printf(" u___) |       | |      | |  | |   | |_| | \\| |/__  / ___ \\ U| |_| |\\.-,_| |_| |  |  _ <        .-,_| |_| | u___) |   \n");
    printf(" |____/>>    U/| |\\u    |_|  |_|  <<\\___/   |_____|/_/   \\_\\ |____/ u \\_)-\\___/   |_| \\_\\       \\_)-\\___/  |____/>>  \n");
    printf("  )(  (__).-,_|___|_,-.<<,-,,-.  (__) )(    //  \\\\  \\\\    >>  |||_         \\\\     //   \\\\_           \\\\     )(  (__) \n");
    printf(" (__)      \\_)-' '-(_/  (./  \\.)     (__)  (_\")(\"_)(__)  (__)(__)_)        (__)   (__)  (__)         (__)   (__)      \n");
    printf("%s\n", COLOR_RESET);
}

void print_paginacion(void) {
    printf("%s", COLOR_CYAN);
    printf("   .--------------.               \n");
    printf("  (  Paginación!   )              \n");
    printf("   `-------.------'               \n");
    printf("            \\      .-.           \n");
    printf("             \\    / /            \n");
    printf("                 / |              \n");
    printf("   |\\     ._ ,-\"\"  `.          \n");
    printf("   | |,,_/  7        ;            \n");
    printf(" `;=     ,=(     ,  /             \n");
    printf("  |`q  q  ` |    \\_,|            \n");
    printf(" .=; <> _ ; /  ,/'/ |             \n");
    printf("';|\\,j_ \\;=\\ ,/   `-'          \n");
    printf("    `--'_|\\  )                   \n");
    printf("   ,' | /  ;'                     \n");
    printf("  (,,/ (,,/                       \n");
    printf("%s\n", COLOR_RESET);
}

void print_segmentacion(void) {
    printf("%s", COLOR_CYAN);
    printf("   .--------------.               \n");
    printf("  ( Segmentación!  )              \n");
    printf("   `-------.------'               \n");
    printf("            \\  _=,_              \n");
    printf("             o_/6 /#\\            \n");
    printf("             \\__ |##/            \n");
    printf("              ='|--\\             \n");
    printf("                /   #'-.          \n");
    printf("                \\#|_   _'-. /    \n");
    printf("                 |/ \\_( # |\"   \n");
    printf("               C/ ,--___/         \n");
    printf("%s\n", COLOR_RESET);
}

void print_info(const char *mensaje) {
    printf("%s[INFO] %s%s\n", COLOR_CYAN, mensaje, COLOR_RESET);
}

void print_exito(const char *mensaje) {
    printf("%s[OK] %s%s\n", COLOR_GREEN, mensaje, COLOR_RESET);
}

void print_error(const char *mensaje) {
    printf("%s[DEAD] %s%s\n", COLOR_RED, mensaje, COLOR_RESET);
}

void print_advertencia(const char *mensaje) {
    printf("%s[WAIT] %s%s\n", COLOR_YELLOW, mensaje, COLOR_RESET);
}