
#include <stdio.h>
#include <stdlib.h>

#include "auth.h"
#include "utiles.h"

#define RUTA_CONFIG "config/servidor.conf"

int main(void) {
    printf("========================================\n");
    printf("   GESTION DE CINE - Servidor local\n");
    printf("========================================\n\n");

    Config cfg;
    if (config_cargar(RUTA_CONFIG, &cfg) != 0) {
        fprintf(stderr, "No se pudo cargar la configuracion. Abortando.\n");
        return EXIT_FAILURE;
    }
    config_establecer(&cfg);
    printf("[OK] Configuracion cargada desde %s\n", RUTA_CONFIG);
    printf("     - Base de datos: %s\n", cfg.ruta_db);
    printf("     - Log:           %s\n", cfg.ruta_log);
    printf("     - Max intentos:  %d\n", cfg.max_intentos_login);

    if (log_iniciar(cfg.ruta_log) != 0) {
        fprintf(stderr, "No se pudo iniciar el log. Abortando.\n");
        return EXIT_FAILURE;
    }
    printf("[OK] Sistema de logs iniciado\n");
    log_escribir(LOG_INFO, NULL, "=== Arranque del servidor ===");

    printf("[--] db_init() pendiente de implementar (fase 2)\n");
    log_escribir(LOG_INFO, NULL, "db_init pendiente (fase 1)");

    printf("[--] ui_ejecutar() pendiente de implementar (fase 3/4)\n");

    printf("\n--- Prueba de utiles ---\n");
    char fecha[MAX_FECHA];
    char ts[MAX_TIMESTAMP];
    fecha_actual(fecha, sizeof(fecha));
    timestamp_actual(ts, sizeof(ts));
    printf("Fecha actual:     %s\n", fecha);
    printf("Timestamp actual: %s\n", ts);

    printf("Validar fecha '2026-04-05': %s\n",
           validar_fecha("2026-04-05") ? "OK" : "FAIL");
    printf("Validar fecha '2026-13-99': %s\n",
           validar_fecha("2026-13-99") ? "OK" : "FAIL (correcto)");
    printf("Validar hora  '18:30':      %s\n",
           validar_hora("18:30") ? "OK" : "FAIL");
    printf("Validar email 'a@b.com':    %s\n",
           validar_email("a@b.com") ? "OK" : "FAIL");

    log_escribir(LOG_INFO, NULL, "=== Cierre del servidor ===");
    log_cerrar();
    printf("\n[OK] Cierre limpio. Revisa %s para ver los logs.\n", cfg.ruta_log);

    return EXIT_SUCCESS;
}