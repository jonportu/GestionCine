
#include "utiles.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

Lista *lista_crear(void) {
    Lista *l = malloc(sizeof(Lista));
    if (l == NULL) {
        return NULL;
    }
    l->cabeza = NULL;
    l->tamano = 0;
    return l;
}

void lista_destruir(Lista *lista, void (*liberar_dato)(void *)) {
    if (lista == NULL) {
        return;
    }

    NodoLista *actual = lista->cabeza;
    while (actual != NULL) {
        NodoLista *siguiente = actual->siguiente;

        if (liberar_dato != NULL && actual->dato != NULL) {
            liberar_dato(actual->dato);
        }

        free(actual);
        actual = siguiente;
    }

    free(lista);
}

int lista_anadir(Lista *lista, void *dato) {
    if (lista == NULL) {
        return -1;
    }

    NodoLista *nuevo = malloc(sizeof(NodoLista));
    if (nuevo == NULL) {
        return -1;
    }
    nuevo->dato = dato;
    nuevo->siguiente = NULL;

    if (lista->cabeza == NULL) {
        lista->cabeza = nuevo;
    } else {
        NodoLista *actual = lista->cabeza;
        while (actual->siguiente != NULL) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }

    lista->tamano++;
    return 0;
}

int lista_tamano(const Lista *lista) {
    if (lista == NULL) {
        return 0;
    }
    return lista->tamano;
}

int lista_vacia(const Lista *lista) {
    if (lista == NULL) {
        return 1;
    }
    return lista->tamano == 0 ? 1 : 0;
}

int leer_linea(char *destino, size_t max) {
    if (destino == NULL || max == 0) {
        return -1;
    }

    if (fgets(destino, (int)max, stdin) == NULL) {
        return -1;
    }

    size_t len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
    } else if (len == max - 1) {

        int c;
        while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }
    }

    return 0;
}

int leer_entero(const char *prompt, int min, int max) {
    char buffer[64];
    int valor;

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (leer_linea(buffer, sizeof(buffer)) != 0) {
            printf("Error de lectura. Intentalo de nuevo.\n");
            continue;
        }

        char sobrante;
        int leidos = sscanf(buffer, "%d %c", &valor, &sobrante);
        if (leidos != 1) {
            printf("Entrada invalida. Introduce un numero entero.\n");
            continue;
        }

        if (valor < min || valor > max) {
            printf("El valor debe estar entre %d y %d.\n", min, max);
            continue;
        }

        return valor;
    }
}

double leer_double(const char *prompt, double min) {
    char buffer[64];
    double valor;

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (leer_linea(buffer, sizeof(buffer)) != 0) {
            printf("Error de lectura. Intentalo de nuevo.\n");
            continue;
        }

        char sobrante;
        int leidos = sscanf(buffer, "%lf %c", &valor, &sobrante);
        if (leidos != 1) {
            printf("Entrada invalida. Introduce un numero.\n");
            continue;
        }

        if (valor < min) {
            printf("El valor debe ser mayor o igual a %.2f.\n", min);
            continue;
        }

        return valor;
    }
}

void leer_string(const char *prompt, char *destino, size_t max) {
    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (leer_linea(destino, max) != 0) {
            printf("Error de lectura. Intentalo de nuevo.\n");
            continue;
        }

        if (destino[0] == '\0') {
            printf("No puede estar vacio. Intentalo de nuevo.\n");
            continue;
        }

        return;
    }
}

int confirmar(const char *prompt) {
    char buffer[16];

    while (1) {
        printf("%s [s/n]: ", prompt);
        fflush(stdout);

        if (leer_linea(buffer, sizeof(buffer)) != 0) {
            continue;
        }

        if (buffer[0] == 's' || buffer[0] == 'S') return 1;
        if (buffer[0] == 'n' || buffer[0] == 'N') return 0;

        printf("Responde 's' o 'n'.\n");
    }
}

void pausar(void) {
    printf("\nPulsa ENTER para continuar...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }
}

void limpiar_pantalla(void) {

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int validar_fecha(const char *fecha) {
    if (fecha == NULL) return 0;

    if (strlen(fecha) != 10) return 0;
    if (fecha[4] != '-' || fecha[7] != '-') return 0;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)fecha[i])) return 0;
    }

    int anio  = (fecha[0]-'0')*1000 + (fecha[1]-'0')*100 + (fecha[2]-'0')*10 + (fecha[3]-'0');
    int mes   = (fecha[5]-'0')*10 + (fecha[6]-'0');
    int dia   = (fecha[8]-'0')*10 + (fecha[9]-'0');

    if (anio < 1900 || anio > 2100) return 0;
    if (mes  < 1    || mes  > 12)   return 0;
    if (dia  < 1    || dia  > 31)   return 0;

    int dias_por_mes[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (dia > dias_por_mes[mes - 1]) return 0;

    return 1;
}

int validar_hora(const char *hora) {
    if (hora == NULL) return 0;

    if (strlen(hora) != 5) return 0;
    if (hora[2] != ':') return 0;

    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!isdigit((unsigned char)hora[i])) return 0;
    }

    int h = (hora[0]-'0')*10 + (hora[1]-'0');
    int m = (hora[3]-'0')*10 + (hora[4]-'0');

    if (h < 0 || h > 23) return 0;
    if (m < 0 || m > 59) return 0;

    return 1;
}

int validar_email(const char *email) {
    if (email == NULL) return 0;

    int tiene_arroba = 0;
    int tiene_punto_tras_arroba = 0;
    int pos_arroba = -1;

    for (int i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') {
            if (tiene_arroba) return 0;  /* más de una @ */
            tiene_arroba = 1;
            pos_arroba = i;
        } else if (email[i] == '.' && tiene_arroba && i > pos_arroba + 1) {
            tiene_punto_tras_arroba = 1;
        }
    }

    return tiene_arroba && tiene_punto_tras_arroba;
}

void fecha_actual(char *destino, size_t max) {
    if (destino == NULL || max < 11) return;

    time_t ahora = time(NULL);
    struct tm *t = localtime(&ahora);
    strftime(destino, max, "%Y-%m-%d", t);
}

void timestamp_actual(char *destino, size_t max) {
    if (destino == NULL || max < 20) return;

    time_t ahora = time(NULL);
    struct tm *t = localtime(&ahora);
    strftime(destino, max, "%Y-%m-%d %H:%M:%S", t);
}