#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

int main() {
    printf("Conectando a BD...\n");
    if (abrir_db("data/cine.db") != 0) {
        printf("Error abriendo BD.\n");
        return 1;
    }

    printf("Insertando peliculas...\n");
    Pelicula p1 = {0, "El Padrino", "Drama", 175, 18};
    Pelicula p2 = {0, "Matrix", "Ciencia Ficcion", 136, 12};
    Pelicula p3 = {0, "Toy Story", "Animacion", 81, 0};
    Pelicula p4 = {0, "Interstellar", "Ciencia Ficcion", 169, 7};
    crear_pelicula(&p1);
    crear_pelicula(&p2);
    crear_pelicula(&p3);
    crear_pelicula(&p4);

    printf("Insertando salas...\n");
    Sala s1 = {0, 1, 50};
    Sala s2 = {0, 2, 100};
    Sala s3 = {0, 3, 20};
    crear_sala(&s1);
    crear_sala(&s2);
    crear_sala(&s3);

    printf("Insertando sesiones...\n");
    Sesion se1 = {0, 1, 1, "2026-05-20", "18:00", 8.50, 0};
    Sesion se2 = {0, 2, 2, "2026-05-20", "20:30", 9.00, 0};
    Sesion se3 = {0, 3, 3, "2026-05-21", "17:00", 6.50, 0};
    Sesion se4 = {0, 4, 2, "2026-05-21", "22:00", 9.50, 0};
    crear_sesion(&se1);
    crear_sesion(&se2);
    crear_sesion(&se3);
    crear_sesion(&se4);

    printf("Insertando clientes...\n");
    Cliente c1 = {0, "Juan Perez", "juan@mail.com", "600123456"};
    Cliente c2 = {0, "Maria Garcia", "maria@mail.com", "600654321"};
    crear_cliente(&c1);
    crear_cliente(&c2);

    printf("Datos insertados correctamente.\n");
    cerrar_db();
    return 0;
}
