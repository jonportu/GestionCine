#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utiles.h"

void leer_string(const char *prompt, char *destino, int max) {
    printf("%s", prompt);
    fgets(destino, max, stdin);
    int len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
    }
}

int leer_entero(const char *prompt) {
    char buffer[50];
    char *endptr;
    long val;
    while (1) {
        leer_string(prompt, buffer, sizeof(buffer));
        if (strlen(buffer) == 0) continue;
        val = strtol(buffer, &endptr, 10);
        if (*endptr == '\0') {
            return (int)val;
        }
        printf("Entrada invalida. Introduce un numero entero.\n");
    }
}

float leer_float(const char *prompt) {
    char buffer[50];
    char *endptr;
    float val;
    while (1) {
        leer_string(prompt, buffer, sizeof(buffer));
        if (strlen(buffer) == 0) continue;
        val = strtof(buffer, &endptr);
        if (*endptr == '\0') {
            return val;
        }
        printf("Entrada invalida. Introduce un numero decimal.\n");
    }
}

int confirmar(const char *prompt) {
    char buffer[10];
    printf("%s (s/n): ", prompt);
    fgets(buffer, sizeof(buffer), stdin);
    return (buffer[0] == 's' || buffer[0] == 'S');
}

void pausar(void) {
    printf("\nPulsa ENTER para continuar...");
    getchar();
}

void limpiar(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void escribir_log(const char *mensaje) {
    FILE *f = fopen("data/servidor.log", "a");
    if (f == NULL) return;

    time_t ahora = time(NULL);
    char fecha[20];
    strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", localtime(&ahora));

    fprintf(f, "[%s] %s\n", fecha, mensaje);
    fclose(f);
}