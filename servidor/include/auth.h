#ifndef AUTH_H
#define AUTH_H

#include "modelos.h"

typedef struct {
    char ruta_db[200];
    char admin_user[50];
    char admin_pass[50];
} Config;

extern Config config;
extern Usuario usuario_actual;
extern int hay_sesion;

int cargar_config(const char *ruta);
int login(const char *nombre, const char *password);
void logout(void);

#endif