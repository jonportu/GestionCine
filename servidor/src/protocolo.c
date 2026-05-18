#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "protocolo.h"
#include "red.h"
#include "db.h"
#include "auth.h"
#include "modelos.h"
#include "utiles.h"

#define MAX_REGISTROS 100

/* Funcion auxiliar: separa un texto por '|' y devuelve cuantas partes hay.
 * Ejemplo: "CREAR_PELICULA|Dune|SciFi|155|13" se separa en 5 partes.
 * Las partes se guardan en el array 'partes'. */
static int separar(char *texto, char *partes[], int max_partes) {
    int n = 0;
    char *token = strtok(texto, "|");
    while (token != NULL && n < max_partes) {
        partes[n] = token;
        n++;
        token = strtok(NULL, "|");
    }
    return n;
}

/* ---- LOGIN ---- */
static void cmd_login(char *partes[], int n) {
    if (n < 3) {
        red_enviar("ERROR|Faltan parametros: LOGIN|usuario|password");
        return;
    }

    if (login(partes[1], partes[2]) == 0) {
        char resp[512];
        snprintf(resp, sizeof(resp), "OK|%d|%s|%s",
                 usuario_actual.id, usuario_actual.nombre, usuario_actual.rol);
        red_enviar(resp);
        escribir_log("Login correcto");
    } else {
        red_enviar("ERROR|Usuario o password incorrectos");
    }
}

/* ---- PELICULAS ---- */
static void cmd_listar_peliculas(void) {
    Pelicula peliculas[MAX_REGISTROS];
    int n = listar_peliculas(peliculas, MAX_REGISTROS);

    char resp[256];
    snprintf(resp, sizeof(resp), "OK|%d", n);
    red_enviar(resp);

    for (int i = 0; i < n; i++) {
        char linea[512];
        snprintf(linea, sizeof(linea), "%d|%s|%s|%d|%d",
                 peliculas[i].id, peliculas[i].titulo, peliculas[i].genero,
                 peliculas[i].duracion, peliculas[i].edad);
        red_enviar(linea);
    }
}

static void cmd_buscar_pelicula(char *partes[], int n) {
    if (n < 2) {
        red_enviar("ERROR|Falta ID");
        return;
    }

    int id = atoi(partes[1]);
    Pelicula p;
    if (buscar_pelicula(id, &p) == 0) {
        char resp[512];
        snprintf(resp, sizeof(resp), "OK|%d|%s|%s|%d|%d",
                 p.id, p.titulo, p.genero, p.duracion, p.edad);
        red_enviar(resp);
    } else {
        red_enviar("ERROR|Pelicula no encontrada");
    }
}

static void cmd_crear_pelicula(char *partes[], int n) {
    if (n < 5) {
        red_enviar("ERROR|Faltan parametros: titulo|genero|duracion|edad");
        return;
    }

    Pelicula p;
    p.id = 0;
    strncpy(p.titulo, partes[1], sizeof(p.titulo) - 1);
    strncpy(p.genero, partes[2], sizeof(p.genero) - 1);
    p.duracion = atoi(partes[3]);
    p.edad = atoi(partes[4]);

    if (crear_pelicula(&p) == 0) {
        red_enviar("OK|Pelicula creada");
        escribir_log("Pelicula creada");
    } else {
        red_enviar("ERROR|No se pudo crear");
    }
}

static void cmd_modificar_pelicula(char *partes[], int n) {
    if (n < 6) {
        red_enviar("ERROR|Faltan parametros: id|titulo|genero|duracion|edad");
        return;
    }

    Pelicula p;
    p.id = atoi(partes[1]);
    strncpy(p.titulo, partes[2], sizeof(p.titulo) - 1);
    strncpy(p.genero, partes[3], sizeof(p.genero) - 1);
    p.duracion = atoi(partes[4]);
    p.edad = atoi(partes[5]);

    if (modificar_pelicula(&p) == 0) {
        red_enviar("OK|Pelicula modificada");
        escribir_log("Pelicula modificada");
    } else {
        red_enviar("ERROR|No se pudo modificar");
    }
}

static void cmd_borrar_pelicula(char *partes[], int n) {
    if (n < 2) {
        red_enviar("ERROR|Falta ID");
        return;
    }

    int id = atoi(partes[1]);
    if (borrar_pelicula(id) == 0) {
        red_enviar("OK|Pelicula borrada");
        escribir_log("Pelicula borrada");
    } else {
        red_enviar("ERROR|No se pudo borrar");
    }
}

/* ---- SALAS ---- */
static void cmd_listar_salas(void) {
    Sala salas[MAX_REGISTROS];
    int n = listar_salas(salas, MAX_REGISTROS);

    char resp[256];
    snprintf(resp, sizeof(resp), "OK|%d", n);
    red_enviar(resp);

    for (int i = 0; i < n; i++) {
        char linea[256];
        snprintf(linea, sizeof(linea), "%d|%d|%d",
                 salas[i].id, salas[i].numero, salas[i].capacidad);
        red_enviar(linea);
    }
}

static void cmd_buscar_sala(char *partes[], int n) {
    if (n < 2) {
        red_enviar("ERROR|Falta ID");
        return;
    }

    int id = atoi(partes[1]);
    Sala s;
    if (buscar_sala(id, &s) == 0) {
        char resp[256];
        snprintf(resp, sizeof(resp), "OK|%d|%d|%d", s.id, s.numero, s.capacidad);
        red_enviar(resp);
    } else {
        red_enviar("ERROR|Sala no encontrada");
    }
}

static void cmd_crear_sala(char *partes[], int n) {
    if (n < 3) {
        red_enviar("ERROR|Faltan parametros: numero|capacidad");
        return;
    }

    Sala s;
    s.id = 0;
    s.numero = atoi(partes[1]);
    s.capacidad = atoi(partes[2]);

    if (crear_sala(&s) == 0) {
        red_enviar("OK|Sala creada");
        escribir_log("Sala creada");
    } else {
        red_enviar("ERROR|No se pudo crear");
    }
}

static void cmd_modificar_sala(char *partes[], int n) {
    if (n < 4) {
        red_enviar("ERROR|Faltan parametros: id|numero|capacidad");
        return;
    }

    Sala s;
    s.id = atoi(partes[1]);
    s.numero = atoi(partes[2]);
    s.capacidad = atoi(partes[3]);

    if (modificar_sala(&s) == 0) {
        red_enviar("OK|Sala modificada");
        escribir_log("Sala modificada");
    } else {
        red_enviar("ERROR|No se pudo modificar");
    }
}

static void cmd_borrar_sala(char *partes[], int n) {
    if (n < 2) {
        red_enviar("ERROR|Falta ID");
        return;
    }

    if (borrar_sala(atoi(partes[1])) == 0) {
        red_enviar("OK|Sala borrada");
        escribir_log("Sala borrada");
    } else {
        red_enviar("ERROR|No se pudo borrar");
    }
}

/* ---- SESIONES ---- */
static void cmd_listar_sesiones(void) {
    Sesion sesiones[MAX_REGISTROS];
    int n = listar_sesiones(sesiones, MAX_REGISTROS);

    char resp[256];
    snprintf(resp, sizeof(resp), "OK|%d", n);
    red_enviar(resp);

    for (int i = 0; i < n; i++) {
        char linea[512];
        snprintf(linea, sizeof(linea), "%d|%d|%d|%s|%s|%.2f|%d",
                 sesiones[i].id, sesiones[i].pelicula_id, sesiones[i].sala_id,
                 sesiones[i].fecha, sesiones[i].hora, sesiones[i].precio,
                 sesiones[i].vendidas);
        red_enviar(linea);
    }
}

static void cmd_buscar_sesion(char *partes[], int n) {
    if (n < 2) {
        red_enviar("ERROR|Falta ID");
        return;
    }

    int id = atoi(partes[1]);
    Sesion s;
    if (buscar_sesion(id, &s) == 0) {
        char resp[512];
        snprintf(resp, sizeof(resp), "OK|%d|%d|%d|%s|%s|%.2f|%d",
                 s.id, s.pelicula_id, s.sala_id, s.fecha, s.hora, s.precio, s.vendidas);
        red_enviar(resp);
    } else {
        red_enviar("ERROR|Sesion no encontrada");
    }
}

static void cmd_crear_sesion(char *partes[], int n) {
    if (n < 6) {
        red_enviar("ERROR|Faltan parametros: pelicula_id|sala_id|fecha|hora|precio");
        return;
    }

    Sesion s;
    s.id = 0;
    s.pelicula_id = atoi(partes[1]);
    s.sala_id = atoi(partes[2]);
    strncpy(s.fecha, partes[3], sizeof(s.fecha) - 1);
    strncpy(s.hora, partes[4], sizeof(s.hora) - 1);
    s.precio = atof(partes[5]);
    s.vendidas = 0;

    if (crear_sesion(&s) == 0) {
        red_enviar("OK|Sesion creada");
        escribir_log("Sesion creada");
    } else {
        red_enviar("ERROR|No se pudo crear");
    }
}

static void cmd_modificar_sesion(char *partes[], int n) {
    if (n < 7) {
        red_enviar("ERROR|Faltan parametros");
        return;
    }

    Sesion s;
    s.id = atoi(partes[1]);
    s.pelicula_id = atoi(partes[2]);
    s.sala_id = atoi(partes[3]);
    strncpy(s.fecha, partes[4], sizeof(s.fecha) - 1);
    strncpy(s.hora, partes[5], sizeof(s.hora) - 1);
    s.precio = atof(partes[6]);
    s.vendidas = 0;

    if (modificar_sesion(&s) == 0) {
        red_enviar("OK|Sesion modificada");
        escribir_log("Sesion modificada");
    } else {
        red_enviar("ERROR|No se pudo modificar");
    }
}

static void cmd_borrar_sesion(char *partes[], int n) {
    if (n < 2) {
        red_enviar("ERROR|Falta ID");
        return;
    }

    if (borrar_sesion(atoi(partes[1])) == 0) {
        red_enviar("OK|Sesion borrada");
        escribir_log("Sesion borrada");
    } else {
        red_enviar("ERROR|No se pudo borrar");
    }
}

/* ---- CLIENTES ---- */
static void cmd_listar_clientes(void) {
    Cliente clientes[MAX_REGISTROS];
    int n = listar_clientes(clientes, MAX_REGISTROS);

    char resp[256];
    snprintf(resp, sizeof(resp), "OK|%d", n);
    red_enviar(resp);

    for (int i = 0; i < n; i++) {
        char linea[512];
        snprintf(linea, sizeof(linea), "%d|%s|%s|%s",
                 clientes[i].id, clientes[i].nombre,
                 clientes[i].email, clientes[i].telefono);
        red_enviar(linea);
    }
}

static void cmd_buscar_cliente(char *partes[], int n) {
    if (n < 2) {
        red_enviar("ERROR|Falta ID");
        return;
    }

    int id = atoi(partes[1]);
    Cliente c;
    if (buscar_cliente(id, &c) == 0) {
        char resp[512];
        snprintf(resp, sizeof(resp), "OK|%d|%s|%s|%s",
                 c.id, c.nombre, c.email, c.telefono);
        red_enviar(resp);
    } else {
        red_enviar("ERROR|Cliente no encontrado");
    }
}

static void cmd_crear_cliente(char *partes[], int n) {
    if (n < 4) {
        red_enviar("ERROR|Faltan parametros: nombre|email|telefono");
        return;
    }

    Cliente c;
    c.id = 0;
    strncpy(c.nombre, partes[1], sizeof(c.nombre) - 1);
    strncpy(c.email, partes[2], sizeof(c.email) - 1);
    strncpy(c.telefono, partes[3], sizeof(c.telefono) - 1);

    if (crear_cliente(&c) == 0) {
        red_enviar("OK|Cliente creado");
        escribir_log("Cliente creado");
    } else {
        red_enviar("ERROR|No se pudo crear");
    }
}

static void cmd_modificar_cliente(char *partes[], int n) {
    if (n < 5) {
        red_enviar("ERROR|Faltan parametros: id|nombre|email|telefono");
        return;
    }

    Cliente c;
    c.id = atoi(partes[1]);
    strncpy(c.nombre, partes[2], sizeof(c.nombre) - 1);
    strncpy(c.email, partes[3], sizeof(c.email) - 1);
    strncpy(c.telefono, partes[4], sizeof(c.telefono) - 1);

    if (modificar_cliente(&c) == 0) {
        red_enviar("OK|Cliente modificado");
        escribir_log("Cliente modificado");
    } else {
        red_enviar("ERROR|No se pudo modificar");
    }
}

static void cmd_borrar_cliente(char *partes[], int n) {
    if (n < 2) {
        red_enviar("ERROR|Falta ID");
        return;
    }

    if (borrar_cliente(atoi(partes[1])) == 0) {
        red_enviar("OK|Cliente borrado");
        escribir_log("Cliente borrado");
    } else {
        red_enviar("ERROR|No se pudo borrar");
    }
}

/* ---- ENTRADAS ---- */
static void cmd_vender_entrada(char *partes[], int n) {
    if (n < 3) {
        red_enviar("ERROR|Faltan parametros: sesion_id|cliente_id");
        return;
    }

    int sesion_id = atoi(partes[1]);
    int cliente_id = atoi(partes[2]);

    Sesion s;
    if (buscar_sesion(sesion_id, &s) != 0) {
        red_enviar("ERROR|Sesion no existe");
        return;
    }

    Entrada e;
    e.id = 0;
    e.sesion_id = sesion_id;
    e.cliente_id = cliente_id;
    e.precio = s.precio;

    time_t ahora = time(NULL);
    strftime(e.fecha, sizeof(e.fecha), "%Y-%m-%d %H:%M:%S", localtime(&ahora));

    if (vender_entrada(&e) == 0) {
        red_enviar("OK|Entrada vendida");
        escribir_log("Entrada vendida");
    } else {
        red_enviar("ERROR|No se pudo vender (sesion llena?)");
    }
}

static void cmd_listar_entradas(void) {
    Entrada entradas[MAX_REGISTROS];
    int n = listar_entradas(entradas, MAX_REGISTROS);

    char resp[256];
    snprintf(resp, sizeof(resp), "OK|%d", n);
    red_enviar(resp);

    for (int i = 0; i < n; i++) {
        char linea[512];
        snprintf(linea, sizeof(linea), "%d|%d|%d|%.2f|%s",
                 entradas[i].id, entradas[i].sesion_id, entradas[i].cliente_id,
                 entradas[i].precio, entradas[i].fecha);
        red_enviar(linea);
    }
}

/* ---- REPORTES ---- */
static void cmd_reporte_ingresos_peli(void) {
    Pelicula peliculas[MAX_REGISTROS];
    int n = listar_peliculas(peliculas, MAX_REGISTROS);

    char resp[256];
    snprintf(resp, sizeof(resp), "OK|%d", n);
    red_enviar(resp);

    for (int i = 0; i < n; i++) {
        float ingresos = obtener_ingresos_pelicula(peliculas[i].id);
        char linea[512];
        snprintf(linea, sizeof(linea), "%d|%s|%.2f",
                 peliculas[i].id, peliculas[i].titulo, ingresos);
        red_enviar(linea);
    }
}

static void cmd_reporte_ingresos_sala(void) {
    Sala salas[MAX_REGISTROS];
    int n = listar_salas(salas, MAX_REGISTROS);

    char resp[256];
    snprintf(resp, sizeof(resp), "OK|%d", n);
    red_enviar(resp);

    for (int i = 0; i < n; i++) {
        float ingresos = obtener_ingresos_sala(salas[i].id);
        char linea[256];
        snprintf(linea, sizeof(linea), "%d|%d|%d|%.2f",
                 salas[i].id, salas[i].numero, salas[i].capacidad, ingresos);
        red_enviar(linea);
    }
}

static void cmd_reporte_ocupacion(void) {
    Sala salas[MAX_REGISTROS];
    int n = listar_salas(salas, MAX_REGISTROS);

    char resp[256];
    snprintf(resp, sizeof(resp), "OK|%d", n);
    red_enviar(resp);

    for (int i = 0; i < n; i++) {
        int ocupacion = obtener_ocupacion_sala(salas[i].id);
        char linea[256];
        snprintf(linea, sizeof(linea), "%d|%d|%d|%d",
                 salas[i].id, salas[i].numero, salas[i].capacidad, ocupacion);
        red_enviar(linea);
    }
}

/* =========================================================
 * FUNCION PRINCIPAL: procesar_comando
 *
 * Recibe el texto crudo del cliente, lo separa por '|',
 * mira que comando es, y llama a la funcion correspondiente.
 * ========================================================= */
void procesar_comando(const char *comando) {
    char copia[4096];
    strncpy(copia, comando, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    char *partes[20];
    int n = separar(copia, partes, 20);

    if (n == 0) {
        red_enviar("ERROR|Comando vacio");
        return;
    }

    char *cmd = partes[0];

    /* Autenticacion */
    if (strcmp(cmd, "LOGIN") == 0) {
        cmd_login(partes, n);
    }
    else if (strcmp(cmd, "LOGOUT") == 0) {
        logout();
        red_enviar("OK|Sesion cerrada");
    }
    /* Peliculas */
    else if (strcmp(cmd, "LISTAR_PELICULAS") == 0) {
        cmd_listar_peliculas();
    }
    else if (strcmp(cmd, "BUSCAR_PELICULA") == 0) {
        cmd_buscar_pelicula(partes, n);
    }
    else if (strcmp(cmd, "CREAR_PELICULA") == 0) {
        cmd_crear_pelicula(partes, n);
    }
    else if (strcmp(cmd, "MODIFICAR_PELICULA") == 0) {
        cmd_modificar_pelicula(partes, n);
    }
    else if (strcmp(cmd, "BORRAR_PELICULA") == 0) {
        cmd_borrar_pelicula(partes, n);
    }
    /* Salas */
    else if (strcmp(cmd, "LISTAR_SALAS") == 0) {
        cmd_listar_salas();
    }
    else if (strcmp(cmd, "BUSCAR_SALA") == 0) {
        cmd_buscar_sala(partes, n);
    }
    else if (strcmp(cmd, "CREAR_SALA") == 0) {
        cmd_crear_sala(partes, n);
    }
    else if (strcmp(cmd, "MODIFICAR_SALA") == 0) {
        cmd_modificar_sala(partes, n);
    }
    else if (strcmp(cmd, "BORRAR_SALA") == 0) {
        cmd_borrar_sala(partes, n);
    }
    /* Sesiones */
    else if (strcmp(cmd, "LISTAR_SESIONES") == 0) {
        cmd_listar_sesiones();
    }
    else if (strcmp(cmd, "BUSCAR_SESION") == 0) {
        cmd_buscar_sesion(partes, n);
    }
    else if (strcmp(cmd, "CREAR_SESION") == 0) {
        cmd_crear_sesion(partes, n);
    }
    else if (strcmp(cmd, "MODIFICAR_SESION") == 0) {
        cmd_modificar_sesion(partes, n);
    }
    else if (strcmp(cmd, "BORRAR_SESION") == 0) {
        cmd_borrar_sesion(partes, n);
    }
    /* Clientes */
    else if (strcmp(cmd, "LISTAR_CLIENTES") == 0) {
        cmd_listar_clientes();
    }
    else if (strcmp(cmd, "BUSCAR_CLIENTE") == 0) {
        cmd_buscar_cliente(partes, n);
    }
    else if (strcmp(cmd, "CREAR_CLIENTE") == 0) {
        cmd_crear_cliente(partes, n);
    }
    else if (strcmp(cmd, "MODIFICAR_CLIENTE") == 0) {
        cmd_modificar_cliente(partes, n);
    }
    else if (strcmp(cmd, "BORRAR_CLIENTE") == 0) {
        cmd_borrar_cliente(partes, n);
    }
    /* Entradas */
    else if (strcmp(cmd, "VENDER_ENTRADA") == 0) {
        cmd_vender_entrada(partes, n);
    }
    else if (strcmp(cmd, "LISTAR_ENTRADAS") == 0) {
        cmd_listar_entradas();
    }
    /* Reportes */
    else if (strcmp(cmd, "REPORTE_INGRESOS_PELI") == 0) {
        cmd_reporte_ingresos_peli();
    }
    else if (strcmp(cmd, "REPORTE_INGRESOS_SALA") == 0) {
        cmd_reporte_ingresos_sala();
    }
    else if (strcmp(cmd, "REPORTE_OCUPACION") == 0) {
        cmd_reporte_ocupacion();
    }
    /* Comando desconocido */
    else {
        char resp[256];
        snprintf(resp, sizeof(resp), "ERROR|Comando desconocido: %s", cmd);
        red_enviar(resp);
    }
}