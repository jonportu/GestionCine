#ifndef MODELOS_H
#define MODELOS_H

#define MAX_TEXTO 100

typedef struct {
    int id;
    char nombre[50];
    char password[50];
    char rol[20];
} Usuario;

typedef struct {
    int id;
    char titulo[MAX_TEXTO];
    char genero[50];
    int duracion;
    int edad;
} Pelicula;

typedef struct {
    int id;
    int numero;
    int capacidad;
} Sala;

typedef struct {
    int id;
    int pelicula_id;
    int sala_id;
    char fecha[15];
    char hora[10];
    float precio;
    int vendidas;
} Sesion;

typedef struct {
    int id;
    char nombre[MAX_TEXTO];
    char email[MAX_TEXTO];
    char telefono[20];
} Cliente;

typedef struct {
    int id;
    int sesion_id;
    int cliente_id;
    float precio;
    char fecha[20];
} Entrada;

#endif