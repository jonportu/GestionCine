#ifndef DB_H
#define DB_H

#include "modelos.h"

#define MAX_REGISTROS 100

int abrir_db(const char *ruta);
void cerrar_db(void);
int crear_admin_si_no_existe(const char *nombre, const char *hash_pass);

int crear_usuario(Usuario *u);
int buscar_usuario_por_nombre(const char *nombre, Usuario *destino);
int contar_usuarios(void);

int crear_pelicula(Pelicula *p);
int modificar_pelicula(Pelicula *p);
int borrar_pelicula(int id);
int buscar_pelicula(int id, Pelicula *destino);
int listar_peliculas(Pelicula peliculas[], int max);

int crear_sala(Sala *s);
int modificar_sala(Sala *s);
int borrar_sala(int id);
int buscar_sala(int id, Sala *destino);
int listar_salas(Sala salas[], int max);

int crear_sesion(Sesion *s);
int modificar_sesion(Sesion *s);
int borrar_sesion(int id);
int buscar_sesion(int id, Sesion *destino);
int listar_sesiones(Sesion sesiones[], int max);

int crear_cliente(Cliente *c);
int modificar_cliente(Cliente *c);
int borrar_cliente(int id);
int buscar_cliente(int id, Cliente *destino);
int listar_clientes(Cliente clientes[], int max);

int vender_entrada(Entrada *e);  // comprueba sesion, inserta, incrementa contador
int listar_entradas(Entrada entradas[], int max);

int vaciar_db(void);

#endif