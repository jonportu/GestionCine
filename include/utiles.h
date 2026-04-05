#ifndef UTILES_H
#define UTILES_H

#include <stddef.h>

typedef struct NodoLista {
    void *dato;
    struct NodoLista *siguiente;
} NodoLista;

typedef struct {
    NodoLista *cabeza;
    int tamano;
} Lista;

Lista *lista_crear(void);

void lista_destruir(Lista *lista, void (*liberar_dato)(void *));

int lista_anadir(Lista *lista, void *dato);

int lista_tamano(const Lista *lista);

int lista_vacia(const Lista *lista);

int leer_linea(char *destino, size_t max);

int leer_entero(const char *prompt, int min, int max);

double leer_double(const char *prompt, double min);

void leer_string(const char *prompt, char *destino, size_t max);

int confirmar(const char *prompt);

void pausar(void);

void limpiar_pantalla(void);

int validar_fecha(const char *fecha);

int validar_hora(const char *hora);

int validar_email(const char *email);

void fecha_actual(char *destino, size_t max);

void timestamp_actual(char *destino, size_t max);

#endif