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

    Pelicula p_test;
    if (buscar_pelicula(1, &p_test) != 0) {
        Pelicula p1 = {0, "El Padrino", "Drama", 175, 18};
        crear_pelicula(&p1);
        Pelicula p2 = {0, "Matrix", "Ciencia Ficcion", 136, 12};
        crear_pelicula(&p2);
        Sala s1 = {0, 1, 50};
        crear_sala(&s1);
        Sala s2 = {0, 2, 100};
        crear_sala(&s2);
        Sesion se1 = {0, 1, 1, "2026-05-20", "18:00", 8.50, 0};
        crear_sesion(&se1);
        Cliente c1 = {0, "Juan Perez", "juan@mail.com", "600123456"};
        crear_cliente(&c1);
        printf("[SERVIDOR] Datos de prueba inicializados en BD.\n");
    }

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