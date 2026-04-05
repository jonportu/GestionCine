#ifndef DB_H
#define DB_H

#include "modelos.h"
#include "utiles.h"

int db_init(const char *ruta_db);

void db_close(void);

int db_vaciar(void);

int db_usuario_crear(const Usuario *u);
int db_usuario_modificar(const Usuario *u);
int db_usuario_eliminar(int id);
Usuario *db_usuario_buscar_por_id(int id);
Usuario *db_usuario_buscar_por_nombre(const char *nombre_usuario);
Lista *db_usuario_listar_todos(void);
int db_usuario_contar(void);

int db_pelicula_crear(const Pelicula *p);
int db_pelicula_modificar(const Pelicula *p);
int db_pelicula_eliminar(int id);
Pelicula *db_pelicula_buscar_por_id(int id);
Lista *db_pelicula_listar_todas(void);

int db_sala_crear(const Sala *s);
int db_sala_modificar(const Sala *s);
int db_sala_eliminar(int id);
Sala *db_sala_buscar_por_id(int id);
Lista *db_sala_listar_todas(void);

int db_sesion_crear(const Sesion *s);
int db_sesion_modificar(const Sesion *s);
int db_sesion_eliminar(int id);
Sesion *db_sesion_buscar_por_id(int id);
Lista *db_sesion_listar_todas(void);

int db_cliente_crear(const Cliente *c);
int db_cliente_modificar(const Cliente *c);
int db_cliente_eliminar(int id);
Cliente *db_cliente_buscar_por_id(int id);
Lista *db_cliente_listar_todos(void);

int db_entrada_crear(const Entrada *e);
int db_entrada_eliminar(int id);
Entrada *db_entrada_buscar_por_id(int id);
Lista *db_entrada_listar_todas(void);

int db_importar_peliculas_csv(const char *ruta_csv);

#endif