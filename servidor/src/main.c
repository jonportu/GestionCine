#include <stdio.h>
#include <string.h>

#include "red.h"
#include "protocolo.h"
#include "auth.h"
#include "db.h"
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

    if (red_iniciar(5000) != 0) {
        printf("No se pudo iniciar el servidor\n");
        cerrar_db();
        return 1;
    }

    escribir_log("Servidor remoto iniciado");

    while (1) {

        if (red_esperar_cliente() != 0) {
            continue;
        }

        escribir_log("Cliente conectado");

        int conectado = 1;
        while (conectado) {
            char buffer[4096];

            int leido = red_recibir(buffer, 4096);

            if (leido < 0) {
                printf("[SERVIDOR] Cliente desconectado inesperadamente\n");
                escribir_log("Cliente desconectado inesperadamente");
                conectado = 0;
                continue;
            }

            printf("[SERVIDOR] Recibido: '%s'\n", buffer);

            if (strcmp(buffer, "SALIR") == 0) {
                red_enviar("OK|Hasta luego");
                escribir_log("Cliente desconectado");
                conectado = 0;
                continue;
            }

            procesar_comando(buffer);
        }

        red_cerrar_cliente();
        logout();
        printf("[SERVIDOR] Listo para otro cliente\n\n");
    }

    red_cerrar();
    cerrar_db();
    return 0;
}