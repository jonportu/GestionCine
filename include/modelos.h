#ifndef MODELOS_H
#define MODELOS_H

#define MAX_NOMBRE      100
#define MAX_EMAIL       100
#define MAX_TELEFONO     20
#define MAX_GENERO       50
#define MAX_USUARIO      50
#define MAX_PASSWORD    128   
#define MAX_ROL          20
#define MAX_FECHA        11   
#define MAX_HORA          6   
#define MAX_TIMESTAMP    20   


typedef struct {
    int  id;
    char nombre_usuario[MAX_USUARIO];
    char password_hash[MAX_PASSWORD];
    char rol[MAX_ROL];              
    char fecha_alta[MAX_FECHA];
} Usuario;


typedef struct {
    int  id;
    char titulo[MAX_NOMBRE];
    char genero[MAX_GENERO];
    int  duracion_min;
    int  clasificacion_edad;        
} Pelicula;


typedef struct {
    int id;
    int numero_sala;
    int capacidad;
} Sala;

typedef struct {
    int    id;
    int    pelicula_id;
    int    sala_id;
    char   fecha[MAX_FECHA];
    char   hora[MAX_HORA];
    double precio_base;
    int    asientos_vendidos;
} Sesion;


typedef struct {
    int  id;
    char nombre[MAX_NOMBRE];
    char email[MAX_EMAIL];
    char telefono[MAX_TELEFONO];
    char fecha_registro[MAX_FECHA];
} Cliente;


typedef struct {
    int    id;
    int    sesion_id;
    int    cliente_id;               
    double precio;
    char   fecha_compra[MAX_TIMESTAMP];
} Entrada;

#endif