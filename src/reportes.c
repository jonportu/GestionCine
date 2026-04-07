#include <stdio.h>
#include <stdlib.h>

#include "reportes.h"
#include "db.h"
#include "modelos.h"
#include "utiles.h"

void reporte_peliculas_cartelera(void) {
    limpiar();
    printf("\n--- REPORTE: PELICULAS EN CARTELERA ---\n\n");

    Sesion sesiones[MAX_REGISTROS];
    int n = listar_sesiones(sesiones, MAX_REGISTROS);

    if (n == 0) {
        printf("No hay sesiones programadas.\n");
        pausar();
        return;
    }

    printf("%-3s | %-25s | %-12s | %-11s | %-5s | Precio\n", "ID", "Titulo", "Genero", "Fecha", "Hora");
    printf("----+---------------------------+--------------+-------------+-------+-------\n");

    int mostradas = 0;
    for (int i = 0; i < n; i++) {
        Pelicula p;
        if (buscar_pelicula(sesiones[i].pelicula_id, &p) == 0) {
            printf("%-3d | %-25s | %-12s | %-11s | %-5s | $%.2f\n",
                   p.id, p.titulo, p.genero, sesiones[i].fecha,
                   sesiones[i].hora, sesiones[i].precio);
            mostradas++;
        }
    }

    printf("\nTotal peliculas en cartelera: %d\n", mostradas);
    pausar();
}

void reporte_ingresos_pelicula(void) {
    limpiar();
    printf("\n--- REPORTE: INGRESOS POR PELICULA ---\n\n");

    Pelicula peliculas[MAX_REGISTROS];
    int n = listar_peliculas(peliculas, MAX_REGISTROS);

    if (n == 0) {
        printf("No hay peliculas.\n");
        pausar();
        return;
    }

    printf("%-3s | %-25s | Ingresos   | Entradas Vendidas\n", "ID", "Titulo");
    printf("----+---------------------------+------------+-------------------\n");

    float ingresos_totales = 0.0;
    for (int i = 0; i < n; i++) {
        float ingresos = obtener_ingresos_pelicula(peliculas[i].id);
        ingresos_totales += ingresos;
        printf("%-3d | %-25s | $%-9.2f |\n", peliculas[i].id, peliculas[i].titulo, ingresos);
    }

    printf("----+---------------------------+------------+\n");
    printf("TOTAL INGRESOS: $%.2f\n\n", ingresos_totales);
    pausar();
}

void reporte_ingresos_sala(void) {
    limpiar();
    printf("\n--- REPORTE: INGRESOS POR SALA ---\n\n");

    Sala salas[MAX_REGISTROS];
    int n = listar_salas(salas, MAX_REGISTROS);

    if (n == 0) {
        printf("No hay salas.\n");
        pausar();
        return;
    }

    printf("%-3s | %-6s | Capacidad | Ingresos  \n", "ID", "Numero");
    printf("----+--------+-----------+----------\n");

    float ingresos_totales = 0.0;
    for (int i = 0; i < n; i++) {
        float ingresos = obtener_ingresos_sala(salas[i].id);
        ingresos_totales += ingresos;
        printf("%-3d | %-6d | %-9d | $%-8.2f\n",
               salas[i].id, salas[i].numero, salas[i].capacidad, ingresos);
    }

    printf("----+--------+-----------+----------\n");
    printf("TOTAL INGRESOS: $%.2f\n\n", ingresos_totales);
    pausar();
}

void reporte_ocupacion_salas(void) {
    limpiar();
    printf("\n--- REPORTE: OCUPACION DE SALAS ---\n\n");

    Sala salas[MAX_REGISTROS];
    int n_salas = listar_salas(salas, MAX_REGISTROS);

    if (n_salas == 0) {
        printf("No hay salas.\n");
        pausar();
        return;
    }

    printf("%-3s | %-6s | Capacidad | Vendidas | Ocupacion\n", "ID", "Numero");
    printf("----+--------+-----------+----------+----------\n");

    for (int i = 0; i < n_salas; i++) {
        int ocupacion = obtener_ocupacion_sala(salas[i].id);
        float porcentaje = (salas[i].capacidad > 0) ?
                          (float)ocupacion / salas[i].capacidad * 100 : 0;
        printf("%-3d | %-6d | %-9d | %-8d | %.1f%%\n",
               salas[i].id, salas[i].numero, salas[i].capacidad,
               ocupacion, porcentaje);
    }

    printf("\n");
    pausar();
}
