#include <stdio.h>
#include <string.h>
#include "red.h"

int main(void) {

    if (red_iniciar(5000) != 0) {
        printf("No se pudo iniciar el servidor\n");
        return 1;
    }

    while (1) {

        if (red_esperar_cliente() != 0) {
            continue;
        }

        int conectado = 1;
        while (conectado) {
            char buffer[4096];

            int leido = red_recibir(buffer, 4096);

            if (leido < 0) {
                printf("[SERVIDOR] Cliente desconectado inesperadamente\n");
                conectado = 0;
                continue;
            }

            printf("[SERVIDOR] Recibido: '%s'\n", buffer);

            if (strcmp(buffer, "SALIR") == 0) {
                red_enviar("OK|Hasta luego");
                conectado = 0;
                continue;
            }

            char respuesta[4096];
            snprintf(respuesta, 4096, "OK|Recibido: %s", buffer);
            red_enviar(respuesta);
        }

        red_cerrar_cliente();
        printf("[SERVIDOR] Listo para otro cliente\n\n");
    }

    red_cerrar();
    return 0;
}