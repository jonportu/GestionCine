#include <stdio.h>

#include "auth.h"
#include "db.h"
#include "ui.h"
#include "utiles.h"

int main(void) {

    if (cargar_config("config/servidor.conf") != 0) {
        printf("No se pudo cargar la configuracion.\n");
        return 1;
    }

    if (abrir_db(config.ruta_db) != 0) {
        printf("No se pudo abrir la base de datos.\n");
        return 1;
    }

    crear_admin_si_no_existe(config.admin_user, config.admin_pass);

    escribir_log("Servidor iniciado");

    ejecutar_ui();

    escribir_log("Servidor cerrado");
    cerrar_db();

    return 0;
}