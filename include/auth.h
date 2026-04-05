#ifndef AUTH_H
#define AUTH_H

#include "modelos.h"

#define MAX_RUTA 256

typedef struct {
    char ruta_db[MAX_RUTA];
    char ruta_log[MAX_RUTA];
    char ruta_csv_import[MAX_RUTA];
    char admin_default_user[MAX_USUARIO];
    char admin_default_pass_hash[MAX_PASSWORD];
    int  max_intentos_login;
} Config;

int config_cargar(const char *ruta_fichero, Config *cfg);

const Config *config_obtener(void);

void config_establecer(const Config *cfg);

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} NivelLog;

int log_iniciar(const char *ruta_fichero);

void log_cerrar(void);

void log_escribir(NivelLog nivel, const char *usuario, const char *formato, ...);

void auth_hash_password(const char *password, char *destino);

int auth_login(const char *nombre_usuario, const char *password);

void auth_logout(void);

const Usuario *auth_usuario_actual(void);

int auth_hay_sesion(void);

#endif