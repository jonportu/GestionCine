
#include <stdio.h>
#include <stdlib.h>

#include "auth.h"
#include "db.h"
#include "utiles.h"

#define RUTA_CONFIG "config/servidor.conf"

int main(void) {
    Config cfg;

    if (config_cargar(RUTA_CONFIG, &cfg) != 0) {
        fprintf(stderr, "Error: no se pudo cargar '%s'\n", RUTA_CONFIG);
        return EXIT_FAILURE;
    }
    config_establecer(&cfg);

    if (log_iniciar(cfg.ruta_log) != 0) {
        fprintf(stderr, "Error: no se pudo iniciar el log\n");
        return EXIT_FAILURE;
    }
    log_escribir(LOG_INFO, NULL, "=== Arranque del servidor ===");

    if (db_init(cfg.ruta_db) != 0) {
        fprintf(stderr, "Error: no se pudo inicializar la base de datos\n");
        log_cerrar();
        return EXIT_FAILURE;
    }

    printf("========================================\n");
    printf("   GESTION DE CINE\n");
    printf("========================================\n\n");

    printf("[Auth] Login admin/admin123: %s\n",
           auth_login("admin", "admin123") == 0 ? "OK" : "FAIL");
    printf("[Auth] Login admin/wrong: %s\n",
           auth_login("admin", "wrong") == 0 ? "FAIL (mal)" : "OK (rechazado)");

    Pelicula p = {0, "Oppenheimer", "Drama", 180, 16};
    printf("\n[BD] Crear pelicula: %s\n",
           db_pelicula_crear(&p) == 0 ? "OK" : "FAIL");

    Sala s = {0, 1, 100};
    printf("[BD] Crear sala:     %s\n",
           db_sala_crear(&s) == 0 ? "OK" : "FAIL");

    printf("\n[BD] Listado de peliculas:\n");
    Lista *pelis = db_pelicula_listar_todas();
    if (pelis != NULL) {
        printf("     Total: %d\n", lista_tamano(pelis));
        for (NodoLista *n = pelis->cabeza; n != NULL; n = n->siguiente) {
            Pelicula *px = (Pelicula *)n->dato;
            printf("       id=%d '%s' (%s, %d min)\n",
                   px->id, px->titulo, px->genero, px->duracion_min);
        }
        lista_destruir(pelis, free);
    }

    printf("\n[BD] Listado de salas:\n");
    Lista *salas = db_sala_listar_todas();
    if (salas != NULL) {
        printf("     Total: %d\n", lista_tamano(salas));
        for (NodoLista *n = salas->cabeza; n != NULL; n = n->siguiente) {
            Sala *sx = (Sala *)n->dato;
            printf("       id=%d num=%d cap=%d\n",
                   sx->id, sx->numero_sala, sx->capacidad);
        }
        lista_destruir(salas, free);
    }

    auth_logout();

    db_close();
    log_escribir(LOG_INFO, NULL, "=== Cierre del servidor ===");
    log_cerrar();

    printf("\n[OK] Cierre limpio. Revisa %s para ver los logs.\n", cfg.ruta_log);
    return EXIT_SUCCESS;
}