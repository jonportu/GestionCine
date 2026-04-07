#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auth.h"
#include "db.h"

Config config;
Usuario usuario_actual;
int hay_sesion = 0;

int cargar_config(const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (f == NULL) {
        printf("Error: no se puede abrir %s\n", ruta);
        return -1;
    }

    char linea[300];
    while (fgets(linea, sizeof(linea), f)) {
  
        if (linea[0] == '#' || linea[0] == '\n' || linea[0] == '\r') continue;


        int len = strlen(linea);
        if (len > 0 && linea[len-1] == '\n') linea[len-1] = '\0';
        if (len > 1 && linea[len-2] == '\r') linea[len-2] = '\0';

        char *igual = strchr(linea, '=');
        if (igual == NULL) continue;
        *igual = '\0';
        char *clave = linea;
        char *valor = igual + 1;

        if (strcmp(clave, "db_path") == 0) {
            strcpy(config.ruta_db, valor);
        } else if (strcmp(clave, "admin_user") == 0) {
            strcpy(config.admin_user, valor);
        } else if (strcmp(clave, "admin_pass") == 0) {
            strcpy(config.admin_pass, valor);
        }
    }

    fclose(f);
    return 0;
}

void hash_password(const char *password, char *destino) {
    unsigned long hash = 5381;
    int i = 0;
    while (password[i] != '\0') {
        hash = ((hash << 5) + hash) + password[i];
        i++;
    }
    sprintf(destino, "%lx", hash);
}

int login(const char *nombre, const char *password) {
    Usuario u;
    if (buscar_usuario_por_nombre(nombre, &u) != 0) {
        return -1;
    }

    char hash[50];
    hash_password(password, hash);

    if (strcmp(hash, u.password) != 0) {
        return -1;
    }

    usuario_actual = u;
    hay_sesion = 1;
    return 0;
}

void logout(void) {
    hay_sesion = 0;
}