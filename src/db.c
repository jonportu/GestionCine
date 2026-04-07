#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"
#include "sqlite3.h"

sqlite3 *db = NULL;

int abrir_db(const char *ruta) {
    int rc = sqlite3_open(ruta, &db);
    if (rc != SQLITE_OK) {
        printf("Error abriendo BD: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    char *sql =
        "CREATE TABLE IF NOT EXISTS usuarios ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  nombre TEXT UNIQUE NOT NULL,"
        "  password TEXT NOT NULL,"
        "  rol TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS peliculas ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  titulo TEXT NOT NULL,"
        "  genero TEXT,"
        "  duracion INTEGER,"
        "  edad INTEGER"
        ");"
        "CREATE TABLE IF NOT EXISTS salas ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  numero INTEGER UNIQUE,"
        "  capacidad INTEGER"
        ");"
        "CREATE TABLE IF NOT EXISTS sesiones ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  pelicula_id INTEGER,"
        "  sala_id INTEGER,"
        "  fecha TEXT,"
        "  hora TEXT,"
        "  precio REAL,"
        "  vendidas INTEGER DEFAULT 0"
        ");"
        "CREATE TABLE IF NOT EXISTS clientes ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  nombre TEXT NOT NULL,"
        "  email TEXT,"
        "  telefono TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS entradas ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  sesion_id INTEGER,"
        "  cliente_id INTEGER,"
        "  precio REAL,"
        "  fecha TEXT"
        ");";

    char *err = NULL;
    rc = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        printf("Error creando tablas: %s\n", err);
        sqlite3_free(err);
        return -1;
    }

    return 0;
}

void cerrar_db(void) {
    if (db != NULL) {
        sqlite3_close(db);
        db = NULL;
    }
}

int vaciar_db(void) {
    char *sql =
        "DELETE FROM entradas;"
        "DELETE FROM sesiones;"
        "DELETE FROM clientes;"
        "DELETE FROM peliculas;"
        "DELETE FROM salas;"
        "DELETE FROM usuarios;";
    return sqlite3_exec(db, sql, NULL, NULL, NULL) == SQLITE_OK ? 0 : -1;
}

int crear_admin_si_no_existe(const char *nombre, const char *hash_pass) {
    if (contar_usuarios() > 0) return 0;

    Usuario u;
    u.id = 0;
    strcpy(u.nombre, nombre);
    strcpy(u.password, hash_pass);
    strcpy(u.rol, "admin");
    return crear_usuario(&u);
}


int crear_usuario(Usuario *u) {
    char *sql = "INSERT INTO usuarios (nombre, password, rol) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, u->nombre, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, u->password, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, u->rol, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int buscar_usuario_por_nombre(const char *nombre, Usuario *destino) {
    char *sql = "SELECT id, nombre, password, rol FROM usuarios WHERE nombre = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_TRANSIENT);

    int encontrado = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        destino->id = sqlite3_column_int(stmt, 0);
        strcpy(destino->nombre, (const char *)sqlite3_column_text(stmt, 1));
        strcpy(destino->password, (const char *)sqlite3_column_text(stmt, 2));
        strcpy(destino->rol, (const char *)sqlite3_column_text(stmt, 3));
        encontrado = 0;
    }
    sqlite3_finalize(stmt);
    return encontrado;
}

int contar_usuarios(void) {
    char *sql = "SELECT COUNT(*) FROM usuarios;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;

    int n = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        n = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return n;
}


int crear_pelicula(Pelicula *p) {
    char *sql = "INSERT INTO peliculas (titulo, genero, duracion, edad) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, p->titulo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, p->genero, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, p->duracion);
    sqlite3_bind_int(stmt, 4, p->edad);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int modificar_pelicula(Pelicula *p) {
    char *sql = "UPDATE peliculas SET titulo=?, genero=?, duracion=?, edad=? WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, p->titulo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, p->genero, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, p->duracion);
    sqlite3_bind_int(stmt, 4, p->edad);
    sqlite3_bind_int(stmt, 5, p->id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int borrar_pelicula(int id) {
    char *sql = "DELETE FROM peliculas WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int buscar_pelicula(int id, Pelicula *destino) {
    char *sql = "SELECT id, titulo, genero, duracion, edad FROM peliculas WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, id);

    int encontrado = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        destino->id = sqlite3_column_int(stmt, 0);
        strcpy(destino->titulo, (const char *)sqlite3_column_text(stmt, 1));
        const unsigned char *g = sqlite3_column_text(stmt, 2);
        strcpy(destino->genero, g ? (const char *)g : "");
        destino->duracion = sqlite3_column_int(stmt, 3);
        destino->edad = sqlite3_column_int(stmt, 4);
        encontrado = 0;
    }
    sqlite3_finalize(stmt);
    return encontrado;
}

int listar_peliculas(Pelicula peliculas[], int max) {
    char *sql = "SELECT id, titulo, genero, duracion, edad FROM peliculas ORDER BY id;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;

    int n = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && n < max) {
        peliculas[n].id = sqlite3_column_int(stmt, 0);
        strcpy(peliculas[n].titulo, (const char *)sqlite3_column_text(stmt, 1));
        const unsigned char *g = sqlite3_column_text(stmt, 2);
        strcpy(peliculas[n].genero, g ? (const char *)g : "");
        peliculas[n].duracion = sqlite3_column_int(stmt, 3);
        peliculas[n].edad = sqlite3_column_int(stmt, 4);
        n++;
    }
    sqlite3_finalize(stmt);
    return n;
}

int crear_sala(Sala *s) {
    char *sql = "INSERT INTO salas (numero, capacidad) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, s->numero);
    sqlite3_bind_int(stmt, 2, s->capacidad);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int modificar_sala(Sala *s) {
    char *sql = "UPDATE salas SET numero=?, capacidad=? WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, s->numero);
    sqlite3_bind_int(stmt, 2, s->capacidad);
    sqlite3_bind_int(stmt, 3, s->id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int borrar_sala(int id) {
    char *sql = "DELETE FROM salas WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int buscar_sala(int id, Sala *destino) {
    char *sql = "SELECT id, numero, capacidad FROM salas WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, id);

    int encontrado = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        destino->id = sqlite3_column_int(stmt, 0);
        destino->numero = sqlite3_column_int(stmt, 1);
        destino->capacidad = sqlite3_column_int(stmt, 2);
        encontrado = 0;
    }
    sqlite3_finalize(stmt);
    return encontrado;
}

int listar_salas(Sala salas[], int max) {
    char *sql = "SELECT id, numero, capacidad FROM salas ORDER BY numero;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;

    int n = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && n < max) {
        salas[n].id = sqlite3_column_int(stmt, 0);
        salas[n].numero = sqlite3_column_int(stmt, 1);
        salas[n].capacidad = sqlite3_column_int(stmt, 2);
        n++;
    }
    sqlite3_finalize(stmt);
    return n;
}


int crear_sesion(Sesion *s) {
    char *sql = "INSERT INTO sesiones (pelicula_id, sala_id, fecha, hora, precio, vendidas) VALUES (?, ?, ?, ?, ?, 0);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, s->pelicula_id);
    sqlite3_bind_int(stmt, 2, s->sala_id);
    sqlite3_bind_text(stmt, 3, s->fecha, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, s->hora, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, s->precio);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int modificar_sesion(Sesion *s) {
    char *sql = "UPDATE sesiones SET pelicula_id=?, sala_id=?, fecha=?, hora=?, precio=? WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, s->pelicula_id);
    sqlite3_bind_int(stmt, 2, s->sala_id);
    sqlite3_bind_text(stmt, 3, s->fecha, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, s->hora, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, s->precio);
    sqlite3_bind_int(stmt, 6, s->id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int borrar_sesion(int id) {
    char *sql = "DELETE FROM sesiones WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int buscar_sesion(int id, Sesion *destino) {
    char *sql = "SELECT id, pelicula_id, sala_id, fecha, hora, precio, vendidas FROM sesiones WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, id);

    int encontrado = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        destino->id = sqlite3_column_int(stmt, 0);
        destino->pelicula_id = sqlite3_column_int(stmt, 1);
        destino->sala_id = sqlite3_column_int(stmt, 2);
        strcpy(destino->fecha, (const char *)sqlite3_column_text(stmt, 3));
        strcpy(destino->hora, (const char *)sqlite3_column_text(stmt, 4));
        destino->precio = sqlite3_column_double(stmt, 5);
        destino->vendidas = sqlite3_column_int(stmt, 6);
        encontrado = 0;
    }
    sqlite3_finalize(stmt);
    return encontrado;
}

int listar_sesiones(Sesion sesiones[], int max) {
    char *sql = "SELECT id, pelicula_id, sala_id, fecha, hora, precio, vendidas FROM sesiones ORDER BY fecha, hora;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;

    int n = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && n < max) {
        sesiones[n].id = sqlite3_column_int(stmt, 0);
        sesiones[n].pelicula_id = sqlite3_column_int(stmt, 1);
        sesiones[n].sala_id = sqlite3_column_int(stmt, 2);
        strcpy(sesiones[n].fecha, (const char *)sqlite3_column_text(stmt, 3));
        strcpy(sesiones[n].hora, (const char *)sqlite3_column_text(stmt, 4));
        sesiones[n].precio = sqlite3_column_double(stmt, 5);
        sesiones[n].vendidas = sqlite3_column_int(stmt, 6);
        n++;
    }
    sqlite3_finalize(stmt);
    return n;
}

int crear_cliente(Cliente *c) {
    char *sql = "INSERT INTO clientes (nombre, email, telefono) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, c->nombre, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, c->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, c->telefono, -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int modificar_cliente(Cliente *c) {
    char *sql = "UPDATE clientes SET nombre=?, email=?, telefono=? WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, c->nombre, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, c->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, c->telefono, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, c->id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int borrar_cliente(int id) {
    char *sql = "DELETE FROM clientes WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int buscar_cliente(int id, Cliente *destino) {
    char *sql = "SELECT id, nombre, email, telefono FROM clientes WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, id);

    int encontrado = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        destino->id = sqlite3_column_int(stmt, 0);
        strcpy(destino->nombre, (const char *)sqlite3_column_text(stmt, 1));
        const unsigned char *e = sqlite3_column_text(stmt, 2);
        strcpy(destino->email, e ? (const char *)e : "");
        const unsigned char *t = sqlite3_column_text(stmt, 3);
        strcpy(destino->telefono, t ? (const char *)t : "");
        encontrado = 0;
    }
    sqlite3_finalize(stmt);
    return encontrado;
}

int listar_clientes(Cliente clientes[], int max) {
    char *sql = "SELECT id, nombre, email, telefono FROM clientes ORDER BY id;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;

    int n = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && n < max) {
        clientes[n].id = sqlite3_column_int(stmt, 0);
        strcpy(clientes[n].nombre, (const char *)sqlite3_column_text(stmt, 1));
        const unsigned char *e = sqlite3_column_text(stmt, 2);
        strcpy(clientes[n].email, e ? (const char *)e : "");
        const unsigned char *t = sqlite3_column_text(stmt, 3);
        strcpy(clientes[n].telefono, t ? (const char *)t : "");
        n++;
    }
    sqlite3_finalize(stmt);
    return n;
}

int vender_entrada(Entrada *e) {

    Sesion s;
    if (buscar_sesion(e->sesion_id, &s) != 0) {
        return -1;
    }

    Sala sala;
    if (buscar_sala(s.sala_id, &sala) != 0) {
        return -1;
    }

    if (s.vendidas >= sala.capacidad) {
        return -1;  // sala llena
    }

    char *sql = "INSERT INTO entradas (sesion_id, cliente_id, precio, fecha) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt, 1, e->sesion_id);
    if (e->cliente_id > 0) {
        sqlite3_bind_int(stmt, 2, e->cliente_id);
    } else {
        sqlite3_bind_null(stmt, 2);
    }
    sqlite3_bind_double(stmt, 3, e->precio);
    sqlite3_bind_text(stmt, 4, e->fecha, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;

    char *sql2 = "UPDATE sesiones SET vendidas = vendidas + 1 WHERE id = ?;";
    sqlite3_stmt *stmt2;
    if (sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(stmt2, 1, e->sesion_id);
    sqlite3_step(stmt2);
    sqlite3_finalize(stmt2);

    return 0;
}

int listar_entradas(Entrada entradas[], int max) {
    char *sql = "SELECT id, sesion_id, cliente_id, precio, fecha FROM entradas ORDER BY id;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;

    int n = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && n < max) {
        entradas[n].id = sqlite3_column_int(stmt, 0);
        entradas[n].sesion_id = sqlite3_column_int(stmt, 1);
        if (sqlite3_column_type(stmt, 2) == SQLITE_NULL) {
            entradas[n].cliente_id = 0;
        } else {
            entradas[n].cliente_id = sqlite3_column_int(stmt, 2);
        }
        entradas[n].precio = sqlite3_column_double(stmt, 3);
        const unsigned char *f = sqlite3_column_text(stmt, 4);
        strcpy(entradas[n].fecha, f ? (const char *)f : "");
        n++;
    }
    sqlite3_finalize(stmt);
    return n;
}