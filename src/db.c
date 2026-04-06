#include "db.h"
#include "auth.h"
#include "utiles.h"
#include "sqlite3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static sqlite3 *g_db = NULL;


static const char *ESQUEMA_SQL =
"CREATE TABLE IF NOT EXISTS usuarios ("
"    id              INTEGER PRIMARY KEY AUTOINCREMENT,"
"    nombre_usuario  TEXT UNIQUE NOT NULL,"
"    password_hash   TEXT NOT NULL,"
"    rol             TEXT NOT NULL CHECK(rol IN ('admin','empleado')),"
"    fecha_alta      TEXT NOT NULL"
");"

"CREATE TABLE IF NOT EXISTS peliculas ("
"    id                  INTEGER PRIMARY KEY AUTOINCREMENT,"
"    titulo              TEXT NOT NULL,"
"    genero              TEXT,"
"    duracion_min        INTEGER NOT NULL CHECK(duracion_min > 0),"
"    clasificacion_edad  INTEGER NOT NULL"
");"

"CREATE TABLE IF NOT EXISTS salas ("
"    id          INTEGER PRIMARY KEY AUTOINCREMENT,"
"    numero_sala INTEGER UNIQUE NOT NULL,"
"    capacidad   INTEGER NOT NULL CHECK(capacidad > 0)"
");"

"CREATE TABLE IF NOT EXISTS sesiones ("
"    id                 INTEGER PRIMARY KEY AUTOINCREMENT,"
"    pelicula_id        INTEGER NOT NULL REFERENCES peliculas(id),"
"    sala_id            INTEGER NOT NULL REFERENCES salas(id),"
"    fecha              TEXT NOT NULL,"
"    hora               TEXT NOT NULL,"
"    precio_base        REAL NOT NULL CHECK(precio_base >= 0),"
"    asientos_vendidos  INTEGER NOT NULL DEFAULT 0 CHECK(asientos_vendidos >= 0)"
");"

"CREATE TABLE IF NOT EXISTS clientes ("
"    id              INTEGER PRIMARY KEY AUTOINCREMENT,"
"    nombre          TEXT NOT NULL,"
"    email           TEXT UNIQUE,"
"    telefono        TEXT,"
"    fecha_registro  TEXT NOT NULL"
");"

"CREATE TABLE IF NOT EXISTS entradas ("
"    id            INTEGER PRIMARY KEY AUTOINCREMENT,"
"    sesion_id     INTEGER NOT NULL REFERENCES sesiones(id),"
"    cliente_id    INTEGER REFERENCES clientes(id),"
"    precio        REAL NOT NULL CHECK(precio >= 0),"
"    fecha_compra  TEXT NOT NULL"
");";


int db_init(const char *ruta_db) {
    if (ruta_db == NULL) return -1;

    int rc = sqlite3_open(ruta_db, &g_db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error abriendo BD: %s\n", sqlite3_errmsg(g_db));
        log_escribir(LOG_ERROR, NULL, "Error abriendo BD: %s", sqlite3_errmsg(g_db));
        sqlite3_close(g_db);
        g_db = NULL;
        return -1;
    }

    sqlite3_exec(g_db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);

    char *err_msg = NULL;
    rc = sqlite3_exec(g_db, ESQUEMA_SQL, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creando esquema: %s\n", err_msg);
        log_escribir(LOG_ERROR, NULL, "Error creando esquema: %s", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(g_db);
        g_db = NULL;
        return -1;
    }

    log_escribir(LOG_INFO, NULL, "Base de datos inicializada: %s", ruta_db);

    if (db_usuario_contar() == 0) {
        const Config *cfg = config_obtener();
        if (cfg == NULL) {
            log_escribir(LOG_ERROR, NULL, "Config no disponible al crear admin por defecto");
            return -1;
        }

        Usuario admin;
        memset(&admin, 0, sizeof(Usuario));
        strncpy(admin.nombre_usuario, cfg->admin_default_user, MAX_USUARIO - 1);
        strncpy(admin.password_hash, cfg->admin_default_pass_hash, MAX_PASSWORD - 1);
        strncpy(admin.rol, "admin", MAX_ROL - 1);

        fecha_actual(admin.fecha_alta, MAX_FECHA);

        if (db_usuario_crear(&admin) == 0) {
            log_escribir(LOG_INFO, NULL, "Admin por defecto creado: %s", admin.nombre_usuario);
        } else {
            log_escribir(LOG_ERROR, NULL, "No se pudo crear el admin por defecto");
        }
    }

    return 0;
}


void db_close(void) {
    if (g_db != NULL) {
        sqlite3_close(g_db);
        g_db = NULL;
        log_escribir(LOG_INFO, NULL, "Base de datos cerrada");
    }
}


int db_vaciar(void) {
    if (g_db == NULL) return -1;

    const char *sql =
        "DELETE FROM entradas;"
        "DELETE FROM sesiones;"
        "DELETE FROM clientes;"
        "DELETE FROM peliculas;"
        "DELETE FROM salas;"
        "DELETE FROM usuarios;";

    char *err_msg = NULL;
    int rc = sqlite3_exec(g_db, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        log_escribir(LOG_ERROR, NULL, "Error vaciando BD: %s", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    log_escribir(LOG_WARN, NULL, "Base de datos vaciada");
    return 0;
}


int db_usuario_crear(const Usuario *u) {
    if (g_db == NULL || u == NULL) return -1;

    const char *sql =
        "INSERT INTO usuarios (nombre_usuario, password_hash, rol, fecha_alta) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_escribir(LOG_ERROR, NULL, "prepare usuario_crear: %s", sqlite3_errmsg(g_db));
        return -1;
    }


    sqlite3_bind_text(stmt, 1, u->nombre_usuario, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, u->password_hash,  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, u->rol,            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, u->fecha_alta,     -1, SQLITE_TRANSIENT);


    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        log_escribir(LOG_ERROR, NULL, "step usuario_crear: %s", sqlite3_errmsg(g_db));
        return -1;
    }

    return 0;
}


int db_usuario_modificar(const Usuario *u) {
    if (g_db == NULL || u == NULL) return -1;

    const char *sql =
        "UPDATE usuarios SET nombre_usuario = ?, password_hash = ?, rol = ? "
        "WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_escribir(LOG_ERROR, NULL, "prepare usuario_modificar: %s", sqlite3_errmsg(g_db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, u->nombre_usuario, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, u->password_hash,  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, u->rol,            -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 4, u->id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


int db_usuario_eliminar(int id) {
    if (g_db == NULL) return -1;

    const char *sql = "DELETE FROM usuarios WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

static void usuario_from_row(sqlite3_stmt *stmt, Usuario *u) {
    u->id = sqlite3_column_int(stmt, 0);

    const unsigned char *txt;

    txt = sqlite3_column_text(stmt, 1);
    strncpy(u->nombre_usuario, txt ? (const char *)txt : "", MAX_USUARIO - 1);
    u->nombre_usuario[MAX_USUARIO - 1] = '\0';

    txt = sqlite3_column_text(stmt, 2);
    strncpy(u->password_hash, txt ? (const char *)txt : "", MAX_PASSWORD - 1);
    u->password_hash[MAX_PASSWORD - 1] = '\0';

    txt = sqlite3_column_text(stmt, 3);
    strncpy(u->rol, txt ? (const char *)txt : "", MAX_ROL - 1);
    u->rol[MAX_ROL - 1] = '\0';

    txt = sqlite3_column_text(stmt, 4);
    strncpy(u->fecha_alta, txt ? (const char *)txt : "", MAX_FECHA - 1);
    u->fecha_alta[MAX_FECHA - 1] = '\0';
}


Usuario *db_usuario_buscar_por_id(int id) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, nombre_usuario, password_hash, rol, fecha_alta "
        "FROM usuarios WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    sqlite3_bind_int(stmt, 1, id);

    Usuario *u = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u = malloc(sizeof(Usuario));
        if (u != NULL) {
            usuario_from_row(stmt, u);
        }
    }

    sqlite3_finalize(stmt);
    return u;
}


Usuario *db_usuario_buscar_por_nombre(const char *nombre_usuario) {
    if (g_db == NULL || nombre_usuario == NULL) return NULL;

    const char *sql =
        "SELECT id, nombre_usuario, password_hash, rol, fecha_alta "
        "FROM usuarios WHERE nombre_usuario = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    sqlite3_bind_text(stmt, 1, nombre_usuario, -1, SQLITE_TRANSIENT);

    Usuario *u = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u = malloc(sizeof(Usuario));
        if (u != NULL) {
            usuario_from_row(stmt, u);
        }
    }

    sqlite3_finalize(stmt);
    return u;
}


Lista *db_usuario_listar_todos(void) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, nombre_usuario, password_hash, rol, fecha_alta "
        "FROM usuarios ORDER BY id;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    Lista *lista = lista_crear();
    if (lista == NULL) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Usuario *u = malloc(sizeof(Usuario));
        if (u == NULL) continue;
        usuario_from_row(stmt, u);
        lista_anadir(lista, u);
    }

    sqlite3_finalize(stmt);
    return lista;
}


int db_usuario_contar(void) {
    if (g_db == NULL) return 0;

    const char *sql = "SELECT COUNT(*) FROM usuarios;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}


static void pelicula_from_row(sqlite3_stmt *stmt, Pelicula *p) {
    p->id = sqlite3_column_int(stmt, 0);

    const unsigned char *txt;

    txt = sqlite3_column_text(stmt, 1);
    strncpy(p->titulo, txt ? (const char *)txt : "", MAX_NOMBRE - 1);
    p->titulo[MAX_NOMBRE - 1] = '\0';

    txt = sqlite3_column_text(stmt, 2);
    strncpy(p->genero, txt ? (const char *)txt : "", MAX_GENERO - 1);
    p->genero[MAX_GENERO - 1] = '\0';

    p->duracion_min       = sqlite3_column_int(stmt, 3);
    p->clasificacion_edad = sqlite3_column_int(stmt, 4);
}


int db_pelicula_crear(const Pelicula *p) {
    if (g_db == NULL || p == NULL) return -1;

    const char *sql =
        "INSERT INTO peliculas (titulo, genero, duracion_min, clasificacion_edad) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_escribir(LOG_ERROR, NULL, "prepare pelicula_crear: %s", sqlite3_errmsg(g_db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, p->titulo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, p->genero, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 3, p->duracion_min);
    sqlite3_bind_int (stmt, 4, p->clasificacion_edad);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


int db_pelicula_modificar(const Pelicula *p) {
    if (g_db == NULL || p == NULL) return -1;

    const char *sql =
        "UPDATE peliculas SET titulo = ?, genero = ?, duracion_min = ?, "
        "clasificacion_edad = ? WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, p->titulo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, p->genero, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 3, p->duracion_min);
    sqlite3_bind_int (stmt, 4, p->clasificacion_edad);
    sqlite3_bind_int (stmt, 5, p->id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


int db_pelicula_eliminar(int id) {
    if (g_db == NULL) return -1;

    const char *sql = "DELETE FROM peliculas WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


Pelicula *db_pelicula_buscar_por_id(int id) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, titulo, genero, duracion_min, clasificacion_edad "
        "FROM peliculas WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    sqlite3_bind_int(stmt, 1, id);

    Pelicula *p = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        p = malloc(sizeof(Pelicula));
        if (p != NULL) pelicula_from_row(stmt, p);
    }

    sqlite3_finalize(stmt);
    return p;
}


Lista *db_pelicula_listar_todas(void) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, titulo, genero, duracion_min, clasificacion_edad "
        "FROM peliculas ORDER BY id;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    Lista *lista = lista_crear();
    if (lista == NULL) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Pelicula *p = malloc(sizeof(Pelicula));
        if (p == NULL) continue;
        pelicula_from_row(stmt, p);
        lista_anadir(lista, p);
    }

    sqlite3_finalize(stmt);
    return lista;
}


static void sala_from_row(sqlite3_stmt *stmt, Sala *s) {
    s->id          = sqlite3_column_int(stmt, 0);
    s->numero_sala = sqlite3_column_int(stmt, 1);
    s->capacidad   = sqlite3_column_int(stmt, 2);
}


int db_sala_crear(const Sala *s) {
    if (g_db == NULL || s == NULL) return -1;

    const char *sql =
        "INSERT INTO salas (numero_sala, capacidad) VALUES (?, ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_escribir(LOG_ERROR, NULL, "prepare sala_crear: %s", sqlite3_errmsg(g_db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, s->numero_sala);
    sqlite3_bind_int(stmt, 2, s->capacidad);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


int db_sala_modificar(const Sala *s) {
    if (g_db == NULL || s == NULL) return -1;

    const char *sql =
        "UPDATE salas SET numero_sala = ?, capacidad = ? WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, s->numero_sala);
    sqlite3_bind_int(stmt, 2, s->capacidad);
    sqlite3_bind_int(stmt, 3, s->id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


int db_sala_eliminar(int id) {
    if (g_db == NULL) return -1;

    const char *sql = "DELETE FROM salas WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


Sala *db_sala_buscar_por_id(int id) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, numero_sala, capacidad FROM salas WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    sqlite3_bind_int(stmt, 1, id);

    Sala *s = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        s = malloc(sizeof(Sala));
        if (s != NULL) sala_from_row(stmt, s);
    }

    sqlite3_finalize(stmt);
    return s;
}


Lista *db_sala_listar_todas(void) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, numero_sala, capacidad FROM salas ORDER BY numero_sala;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    Lista *lista = lista_crear();
    if (lista == NULL) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Sala *s = malloc(sizeof(Sala));
        if (s == NULL) continue;
        sala_from_row(stmt, s);
        lista_anadir(lista, s);
    }

    sqlite3_finalize(stmt);
    return lista;
}


static void sesion_from_row(sqlite3_stmt *stmt, Sesion *s) {
    s->id          = sqlite3_column_int(stmt, 0);
    s->pelicula_id = sqlite3_column_int(stmt, 1);
    s->sala_id     = sqlite3_column_int(stmt, 2);

    const unsigned char *txt;

    txt = sqlite3_column_text(stmt, 3);
    strncpy(s->fecha, txt ? (const char *)txt : "", MAX_FECHA - 1);
    s->fecha[MAX_FECHA - 1] = '\0';

    txt = sqlite3_column_text(stmt, 4);
    strncpy(s->hora, txt ? (const char *)txt : "", MAX_HORA - 1);
    s->hora[MAX_HORA - 1] = '\0';

    s->precio_base       = sqlite3_column_double(stmt, 5);
    s->asientos_vendidos = sqlite3_column_int   (stmt, 6);
}


int db_sesion_crear(const Sesion *s) {
    if (g_db == NULL || s == NULL) return -1;

    const char *sql =
        "INSERT INTO sesiones "
        "(pelicula_id, sala_id, fecha, hora, precio_base, asientos_vendidos) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_escribir(LOG_ERROR, NULL, "prepare sesion_crear: %s", sqlite3_errmsg(g_db));
        return -1;
    }

    sqlite3_bind_int   (stmt, 1, s->pelicula_id);
    sqlite3_bind_int   (stmt, 2, s->sala_id);
    sqlite3_bind_text  (stmt, 3, s->fecha, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 4, s->hora,  -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, s->precio_base);
    sqlite3_bind_int   (stmt, 6, s->asientos_vendidos);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


int db_sesion_modificar(const Sesion *s) {
    if (g_db == NULL || s == NULL) return -1;

    const char *sql =
        "UPDATE sesiones SET pelicula_id = ?, sala_id = ?, fecha = ?, "
        "hora = ?, precio_base = ?, asientos_vendidos = ? WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_int   (stmt, 1, s->pelicula_id);
    sqlite3_bind_int   (stmt, 2, s->sala_id);
    sqlite3_bind_text  (stmt, 3, s->fecha, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 4, s->hora,  -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, s->precio_base);
    sqlite3_bind_int   (stmt, 6, s->asientos_vendidos);
    sqlite3_bind_int   (stmt, 7, s->id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


int db_sesion_eliminar(int id) {
    if (g_db == NULL) return -1;

    const char *sql = "DELETE FROM sesiones WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


Sesion *db_sesion_buscar_por_id(int id) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, pelicula_id, sala_id, fecha, hora, precio_base, asientos_vendidos "
        "FROM sesiones WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    sqlite3_bind_int(stmt, 1, id);

    Sesion *s = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        s = malloc(sizeof(Sesion));
        if (s != NULL) sesion_from_row(stmt, s);
    }

    sqlite3_finalize(stmt);
    return s;
}


Lista *db_sesion_listar_todas(void) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, pelicula_id, sala_id, fecha, hora, precio_base, asientos_vendidos "
        "FROM sesiones ORDER BY fecha, hora;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    Lista *lista = lista_crear();
    if (lista == NULL) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Sesion *s = malloc(sizeof(Sesion));
        if (s == NULL) continue;
        sesion_from_row(stmt, s);
        lista_anadir(lista, s);
    }

    sqlite3_finalize(stmt);
    return lista;
}


static void cliente_from_row(sqlite3_stmt *stmt, Cliente *c) {
    c->id = sqlite3_column_int(stmt, 0);

    const unsigned char *txt;

    txt = sqlite3_column_text(stmt, 1);
    strncpy(c->nombre, txt ? (const char *)txt : "", MAX_NOMBRE - 1);
    c->nombre[MAX_NOMBRE - 1] = '\0';

    txt = sqlite3_column_text(stmt, 2);
    strncpy(c->email, txt ? (const char *)txt : "", MAX_EMAIL - 1);
    c->email[MAX_EMAIL - 1] = '\0';

    txt = sqlite3_column_text(stmt, 3);
    strncpy(c->telefono, txt ? (const char *)txt : "", MAX_TELEFONO - 1);
    c->telefono[MAX_TELEFONO - 1] = '\0';

    txt = sqlite3_column_text(stmt, 4);
    strncpy(c->fecha_registro, txt ? (const char *)txt : "", MAX_FECHA - 1);
    c->fecha_registro[MAX_FECHA - 1] = '\0';
}


int db_cliente_crear(const Cliente *c) {
    if (g_db == NULL || c == NULL) return -1;

    const char *sql =
        "INSERT INTO clientes (nombre, email, telefono, fecha_registro) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_escribir(LOG_ERROR, NULL, "prepare cliente_crear: %s", sqlite3_errmsg(g_db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, c->nombre,         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, c->email,          -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, c->telefono,       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, c->fecha_registro, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


int db_cliente_modificar(const Cliente *c) {
    if (g_db == NULL || c == NULL) return -1;

    const char *sql =
        "UPDATE clientes SET nombre = ?, email = ?, telefono = ? WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, c->nombre,   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, c->email,    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, c->telefono, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 4, c->id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


int db_cliente_eliminar(int id) {
    if (g_db == NULL) return -1;

    const char *sql = "DELETE FROM clientes WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


Cliente *db_cliente_buscar_por_id(int id) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, nombre, email, telefono, fecha_registro "
        "FROM clientes WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    sqlite3_bind_int(stmt, 1, id);

    Cliente *c = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        c = malloc(sizeof(Cliente));
        if (c != NULL) cliente_from_row(stmt, c);
    }

    sqlite3_finalize(stmt);
    return c;
}


Lista *db_cliente_listar_todos(void) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, nombre, email, telefono, fecha_registro "
        "FROM clientes ORDER BY id;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    Lista *lista = lista_crear();
    if (lista == NULL) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Cliente *c = malloc(sizeof(Cliente));
        if (c == NULL) continue;
        cliente_from_row(stmt, c);
        lista_anadir(lista, c);
    }

    sqlite3_finalize(stmt);
    return lista;
}


int db_entrada_crear(const Entrada *e) {
    if (g_db == NULL || e == NULL) return -1;

    if (sqlite3_exec(g_db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK) {
        return -1;
    }

    /* Paso 1: comprobar si la sesion tiene sitio (asientos_vendidos < capacidad) */
    const char *sql_check =
        "SELECT s.asientos_vendidos, sa.capacidad "
        "FROM sesiones s JOIN salas sa ON s.sala_id = sa.id "
        "WHERE s.id = ?;";

    sqlite3_stmt *stmt_check;
    if (sqlite3_prepare_v2(g_db, sql_check, -1, &stmt_check, NULL) != SQLITE_OK) {
        sqlite3_exec(g_db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }

    sqlite3_bind_int(stmt_check, 1, e->sesion_id);

    int vendidos = 0, capacidad = 0, encontrada = 0;
    if (sqlite3_step(stmt_check) == SQLITE_ROW) {
        vendidos   = sqlite3_column_int(stmt_check, 0);
        capacidad  = sqlite3_column_int(stmt_check, 1);
        encontrada = 1;
    }
    sqlite3_finalize(stmt_check);

    if (!encontrada) {
        sqlite3_exec(g_db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;  /* sesion no existe */
    }

    if (vendidos >= capacidad) {
        sqlite3_exec(g_db, "ROLLBACK;", NULL, NULL, NULL);
        log_escribir(LOG_WARN, NULL, "Sesion %d llena, no se puede vender entrada", e->sesion_id);
        return -1;  /* sesion llena */
    }

    const char *sql_insert =
        "INSERT INTO entradas (sesion_id, cliente_id, precio, fecha_compra) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *stmt_ins;
    if (sqlite3_prepare_v2(g_db, sql_insert, -1, &stmt_ins, NULL) != SQLITE_OK) {
        sqlite3_exec(g_db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }

    sqlite3_bind_int   (stmt_ins, 1, e->sesion_id);
    if (e->cliente_id > 0) {
        sqlite3_bind_int(stmt_ins, 2, e->cliente_id);
    } else {
        sqlite3_bind_null(stmt_ins, 2);
    }
    sqlite3_bind_double(stmt_ins, 3, e->precio);
    sqlite3_bind_text  (stmt_ins, 4, e->fecha_compra, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt_ins);
    sqlite3_finalize(stmt_ins);

    if (rc != SQLITE_DONE) {
        sqlite3_exec(g_db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }

    const char *sql_upd =
        "UPDATE sesiones SET asientos_vendidos = asientos_vendidos + 1 "
        "WHERE id = ?;";

    sqlite3_stmt *stmt_upd;
    if (sqlite3_prepare_v2(g_db, sql_upd, -1, &stmt_upd, NULL) != SQLITE_OK) {
        sqlite3_exec(g_db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }

    sqlite3_bind_int(stmt_upd, 1, e->sesion_id);
    rc = sqlite3_step(stmt_upd);
    sqlite3_finalize(stmt_upd);

    if (rc != SQLITE_DONE) {
        sqlite3_exec(g_db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }

    if (sqlite3_exec(g_db, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK) {
        sqlite3_exec(g_db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }

    return 0;
}


int db_entrada_eliminar(int id) {
    if (g_db == NULL) return -1;

    const char *sql = "DELETE FROM entradas WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}


static void entrada_from_row(sqlite3_stmt *stmt, Entrada *e) {
    e->id         = sqlite3_column_int   (stmt, 0);
    e->sesion_id  = sqlite3_column_int   (stmt, 1);
    if (sqlite3_column_type(stmt, 2) == SQLITE_NULL) {
        e->cliente_id = 0;
    } else {
        e->cliente_id = sqlite3_column_int(stmt, 2);
    }
    e->precio     = sqlite3_column_double(stmt, 3);

    const unsigned char *txt = sqlite3_column_text(stmt, 4);
    strncpy(e->fecha_compra, txt ? (const char *)txt : "", MAX_TIMESTAMP - 1);
    e->fecha_compra[MAX_TIMESTAMP - 1] = '\0';
}


Entrada *db_entrada_buscar_por_id(int id) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, sesion_id, cliente_id, precio, fecha_compra "
        "FROM entradas WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    sqlite3_bind_int(stmt, 1, id);

    Entrada *e = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        e = malloc(sizeof(Entrada));
        if (e != NULL) entrada_from_row(stmt, e);
    }

    sqlite3_finalize(stmt);
    return e;
}


Lista *db_entrada_listar_todas(void) {
    if (g_db == NULL) return NULL;

    const char *sql =
        "SELECT id, sesion_id, cliente_id, precio, fecha_compra "
        "FROM entradas ORDER BY fecha_compra DESC;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;

    Lista *lista = lista_crear();
    if (lista == NULL) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Entrada *e = malloc(sizeof(Entrada));
        if (e == NULL) continue;
        entrada_from_row(stmt, e);
        lista_anadir(lista, e);
    }

    sqlite3_finalize(stmt);
    return lista;
}


int db_importar_peliculas_csv(const char *ruta_csv) {
    (void)ruta_csv;
    return -1;
}