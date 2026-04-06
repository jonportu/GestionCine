#include "auth.h"
#include "db.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

static Config g_config;
static int    g_config_cargada = 0;

static void trim(char *s) {
    if (s == NULL) return;

    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == ' ' || s[len-1] == '\t' ||
                       s[len-1] == '\n' || s[len-1] == '\r')) {
        s[--len] = '\0';
    }

    size_t inicio = 0;
    while (s[inicio] == ' ' || s[inicio] == '\t') {
        inicio++;
    }
    if (inicio > 0) {
        memmove(s, s + inicio, len - inicio + 1);
    }
}

int config_cargar(const char *ruta_fichero, Config *cfg) {
    if (ruta_fichero == NULL || cfg == NULL) return -1;

    FILE *f = fopen(ruta_fichero, "r");
    if (f == NULL) {
        fprintf(stderr, "Error: no se puede abrir el fichero de config '%s'\n",
                ruta_fichero);
        return -1;
    }

    memset(cfg, 0, sizeof(Config));
    cfg->max_intentos_login = 3;

    char linea[512];
    int num_linea = 0;

    while (fgets(linea, sizeof(linea), f) != NULL) {
        num_linea++;
        trim(linea);

        if (linea[0] == '\0' || linea[0] == '#') continue;

        char *igual = strchr(linea, '=');
        if (igual == NULL) {
            fprintf(stderr, "Advertencia: linea %d sin '=' en %s\n",
                    num_linea, ruta_fichero);
            continue;
        }

        *igual = '\0';
        char *clave = linea;
        char *valor = igual + 1;
        trim(clave);
        trim(valor);

        if (strcmp(clave, "db_path") == 0) {
            strncpy(cfg->ruta_db, valor, MAX_RUTA - 1);
            cfg->ruta_db[MAX_RUTA - 1] = '\0';
        } else if (strcmp(clave, "log_path") == 0) {
            strncpy(cfg->ruta_log, valor, MAX_RUTA - 1);
            cfg->ruta_log[MAX_RUTA - 1] = '\0';
        } else if (strcmp(clave, "csv_import_path") == 0) {
            strncpy(cfg->ruta_csv_import, valor, MAX_RUTA - 1);
            cfg->ruta_csv_import[MAX_RUTA - 1] = '\0';
        } else if (strcmp(clave, "admin_default_user") == 0) {
            strncpy(cfg->admin_default_user, valor, MAX_USUARIO - 1);
            cfg->admin_default_user[MAX_USUARIO - 1] = '\0';
        } else if (strcmp(clave, "admin_default_pass_hash") == 0) {
            strncpy(cfg->admin_default_pass_hash, valor, MAX_PASSWORD - 1);
            cfg->admin_default_pass_hash[MAX_PASSWORD - 1] = '\0';
        } else if (strcmp(clave, "max_intentos_login") == 0) {
            cfg->max_intentos_login = atoi(valor);
        } else {
            fprintf(stderr, "Advertencia: clave desconocida '%s' en linea %d\n",
                    clave, num_linea);
        }
    }

    fclose(f);
    return 0;
}

void config_establecer(const Config *cfg) {
    if (cfg == NULL) return;
    memcpy(&g_config, cfg, sizeof(Config));
    g_config_cargada = 1;
}

const Config *config_obtener(void) {
    if (!g_config_cargada) return NULL;
    return &g_config;
}


static FILE *g_log_file = NULL;

int log_iniciar(const char *ruta_fichero) {
    if (ruta_fichero == NULL) return -1;

    g_log_file = fopen(ruta_fichero, "a");
    if (g_log_file == NULL) {
        fprintf(stderr, "Error: no se puede abrir el log '%s'\n", ruta_fichero);
        return -1;
    }
    return 0;
}

void log_cerrar(void) {
    if (g_log_file != NULL) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
}

void log_escribir(NivelLog nivel, const char *usuario, const char *formato, ...) {
    if (g_log_file == NULL) return;

    char timestamp[20];
    time_t ahora = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&ahora));

    const char *nivel_str;
    switch (nivel) {
        case LOG_INFO:  nivel_str = "INFO";  break;
        case LOG_WARN:  nivel_str = "WARN";  break;
        case LOG_ERROR: nivel_str = "ERROR"; break;
        default:        nivel_str = "?";     break;
    }

    fprintf(g_log_file, "[%s] [%s] [%s] ",
            timestamp,
            nivel_str,
            usuario != NULL ? usuario : "-");

    va_list args;
    va_start(args, formato);
    vfprintf(g_log_file, formato, args);
    va_end(args);

    fprintf(g_log_file, "\n");
    fflush(g_log_file);
}



static Usuario g_usuario_actual;
static int     g_sesion_activa = 0;


void auth_hash_password(const char *password, char *destino) {
    if (password == NULL || destino == NULL) return;

    unsigned long hash = 5381;
    const unsigned char *p = (const unsigned char *)password;

    while (*p != '\0') {

        hash = ((hash << 5) + hash) + *p;
        p++;
    }

    snprintf(destino, MAX_PASSWORD, "%lx", hash);
}


int auth_login(const char *nombre_usuario, const char *password) {
    if (nombre_usuario == NULL || password == NULL) return -1;


    Usuario *u = db_usuario_buscar_por_nombre(nombre_usuario);
    if (u == NULL) {
        log_escribir(LOG_WARN, nombre_usuario, "Intento de login con usuario inexistente");
        return -1;
    }

    char hash_introducido[MAX_PASSWORD];
    auth_hash_password(password, hash_introducido);

    if (strcmp(hash_introducido, u->password_hash) != 0) {
        log_escribir(LOG_WARN, nombre_usuario, "Password incorrecta");
        free(u);
        return -1;
    }

    memcpy(&g_usuario_actual, u, sizeof(Usuario));
    g_sesion_activa = 1;

    log_escribir(LOG_INFO, nombre_usuario, "Login correcto");

    free(u); 
    return 0;
}


void auth_logout(void) {
    if (g_sesion_activa) {
        log_escribir(LOG_INFO, g_usuario_actual.nombre_usuario, "Logout");
    }
    memset(&g_usuario_actual, 0, sizeof(Usuario));
    g_sesion_activa = 0;
}


const Usuario *auth_usuario_actual(void) {
    return g_sesion_activa ? &g_usuario_actual : NULL;
}

int auth_hay_sesion(void) {
    return g_sesion_activa;
}