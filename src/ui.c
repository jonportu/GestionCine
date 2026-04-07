#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ui.h"
#include "auth.h"
#include "db.h"
#include "utiles.h"
#include "reportes.h"

void menu_reportes(void) {
    int salir = 0;
    while (!salir) {
        limpiar();
        printf("\n--- REPORTES ---\n\n");
        printf("1. Peliculas en cartelera\n");
        printf("2. Ingresos por pelicula\n");
        printf("3. Ingresos por sala\n");
        printf("4. Ocupacion de salas\n");
        printf("0. Volver\n\n");

        int opcion = leer_entero("Opcion: ");

        switch (opcion) {
            case 1: reporte_peliculas_cartelera(); break;
            case 2: reporte_ingresos_pelicula(); break;
            case 3: reporte_ingresos_sala(); break;
            case 4: reporte_ocupacion_salas(); break;
            case 0: salir = 1; break;
        }
    }
}

void menu_peliculas(void) {
    int salir = 0;
    while (!salir) {
        limpiar();
        printf("\n--- GESTION DE PELICULAS ---\n\n");
        printf("1. Listar peliculas\n");
        printf("2. Anyadir pelicula\n");
        printf("3. Modificar pelicula\n");
        printf("4. Borrar pelicula\n");
        printf("0. Volver\n\n");

        int opcion = leer_entero("Opcion: ");

        if (opcion == 1) {
            // Listar
            Pelicula peliculas[MAX_REGISTROS];
            int n = listar_peliculas(peliculas, MAX_REGISTROS);
            printf("\n");
            if (n == 0) {
                printf("No hay peliculas.\n");
            } else {
                printf("ID  | Titulo                    | Genero       | Duracion | Edad\n");
                printf("----+---------------------------+--------------+----------+-----\n");
                for (int i = 0; i < n; i++) {
                    printf("%-3d | %-25s | %-12s | %-8d | +%d\n",
                           peliculas[i].id, peliculas[i].titulo, peliculas[i].genero,
                           peliculas[i].duracion, peliculas[i].edad);
                }
                printf("\nTotal: %d\n", n);
            }
            pausar();
        }
        else if (opcion == 2) {

            Pelicula p;
            p.id = 0;
            printf("\n");
            leer_string("Titulo: ", p.titulo, sizeof(p.titulo));
            leer_string("Genero: ", p.genero, sizeof(p.genero));
            p.duracion = leer_entero("Duracion (min): ");
            p.edad = leer_entero("Edad minima: ");

            if (crear_pelicula(&p) == 0) {
                printf("\nPelicula anyadida.\n");
                escribir_log("Pelicula creada");
            } else {
                printf("\nError al anyadir.\n");
            }
            pausar();
        }
        else if (opcion == 3) {

            int id = leer_entero("\nID a modificar: ");
            Pelicula p;
            if (buscar_pelicula(id, &p) != 0) {
                printf("No existe.\n");
                pausar();
                continue;
            }
            printf("\nDatos actuales: %s | %s | %d min | +%d\n\n",
                   p.titulo, p.genero, p.duracion, p.edad);
            leer_string("Nuevo titulo: ", p.titulo, sizeof(p.titulo));
            leer_string("Nuevo genero: ", p.genero, sizeof(p.genero));
            p.duracion = leer_entero("Nueva duracion: ");
            p.edad = leer_entero("Nueva edad: ");

            if (modificar_pelicula(&p) == 0) {
                printf("\nModificada.\n");
                escribir_log("Pelicula modificada");
            } else {
                printf("\nError.\n");
            }
            pausar();
        }
        else if (opcion == 4) {
            // Borrar
            int id = leer_entero("\nID a borrar: ");
            Pelicula p;
            if (buscar_pelicula(id, &p) != 0) {
                printf("No existe.\n");
                pausar();
                continue;
            }
            printf("Vas a borrar: %s\n", p.titulo);
            if (confirmar("Confirmar")) {
                if (borrar_pelicula(id) == 0) {
                    printf("Borrada.\n");
                    escribir_log("Pelicula borrada");
                }
            }
            pausar();
        }
        else if (opcion == 0) {
            salir = 1;
        }
    }
}

void menu_salas(void) {
    int salir = 0;
    while (!salir) {
        limpiar();
        printf("\n--- GESTION DE SALAS ---\n\n");
        printf("1. Listar salas\n");
        printf("2. Anyadir sala\n");
        printf("3. Modificar sala\n");
        printf("4. Borrar sala\n");
        printf("0. Volver\n\n");

        int opcion = leer_entero("Opcion: ");

        if (opcion == 1) {
            Sala salas[MAX_REGISTROS];
            int n = listar_salas(salas, MAX_REGISTROS);
            printf("\n");
            if (n == 0) {
                printf("No hay salas.\n");
            } else {
                printf("ID  | Numero | Capacidad\n");
                printf("----+--------+----------\n");
                for (int i = 0; i < n; i++) {
                    printf("%-3d | %-6d | %d\n",
                           salas[i].id, salas[i].numero, salas[i].capacidad);
                }
                printf("\nTotal: %d\n", n);
            }
            pausar();
        }
        else if (opcion == 2) {
            Sala s;
            s.id = 0;
            printf("\n");
            s.numero = leer_entero("Numero de sala: ");
            s.capacidad = leer_entero("Capacidad: ");

            if (crear_sala(&s) == 0) {
                printf("\nSala anyadida.\n");
                escribir_log("Sala creada");
            } else {
                printf("\nError (numero duplicado?).\n");
            }
            pausar();
        }
        else if (opcion == 3) {
            int id = leer_entero("\nID a modificar: ");
            Sala s;
            if (buscar_sala(id, &s) != 0) {
                printf("No existe.\n");
                pausar();
                continue;
            }
            printf("\nDatos actuales: numero %d, capacidad %d\n\n", s.numero, s.capacidad);
            s.numero = leer_entero("Nuevo numero: ");
            s.capacidad = leer_entero("Nueva capacidad: ");

            if (modificar_sala(&s) == 0) {
                printf("\nModificada.\n");
                escribir_log("Sala modificada");
            }
            pausar();
        }
        else if (opcion == 4) {
            int id = leer_entero("\nID a borrar: ");
            if (confirmar("Confirmar")) {
                if (borrar_sala(id) == 0) {
                    printf("Borrada.\n");
                    escribir_log("Sala borrada");
                }
            }
            pausar();
        }
        else if (opcion == 0) {
            salir = 1;
        }
    }
}

void menu_sesiones(void) {
    int salir = 0;
    while (!salir) {
        limpiar();
        printf("\n--- GESTION DE SESIONES ---\n\n");
        printf("1. Listar sesiones\n");
        printf("2. Anyadir sesion\n");
        printf("3. Modificar sesion\n");
        printf("4. Borrar sesion\n");
        printf("0. Volver\n\n");

        int opcion = leer_entero("Opcion: ");

        if (opcion == 1) {
            Sesion sesiones[MAX_REGISTROS];
            int n = listar_sesiones(sesiones, MAX_REGISTROS);
            printf("\n");
            if (n == 0) {
                printf("No hay sesiones.\n");
            } else {
                printf("ID  | Peli | Sala | Fecha       | Hora  | Precio | Vendidas\n");
                printf("----+------+------+-------------+-------+--------+---------\n");
                for (int i = 0; i < n; i++) {
                    printf("%-3d | %-4d | %-4d | %-11s | %-5s | %-6.2f | %d\n",
                           sesiones[i].id, sesiones[i].pelicula_id, sesiones[i].sala_id,
                           sesiones[i].fecha, sesiones[i].hora, sesiones[i].precio,
                           sesiones[i].vendidas);
                }
                printf("\nTotal: %d\n", n);
            }
            pausar();
        }
        else if (opcion == 2) {
            Sesion s;
            s.id = 0;
            s.vendidas = 0;
            printf("\n");
            s.pelicula_id = leer_entero("ID pelicula: ");

            Pelicula p;
            if (buscar_pelicula(s.pelicula_id, &p) != 0) {
                printf("La pelicula no existe.\n");
                pausar();
                continue;
            }

            s.sala_id = leer_entero("ID sala: ");
            Sala sa;
            if (buscar_sala(s.sala_id, &sa) != 0) {
                printf("La sala no existe.\n");
                pausar();
                continue;
            }

            leer_string("Fecha (YYYY-MM-DD): ", s.fecha, sizeof(s.fecha));
            leer_string("Hora (HH:MM): ", s.hora, sizeof(s.hora));
            s.precio = leer_float("Precio: ");

            if (crear_sesion(&s) == 0) {
                printf("\nSesion anyadida.\n");
                escribir_log("Sesion creada");
            }
            pausar();
        }
        else if (opcion == 3) {
            int id = leer_entero("\nID a modificar: ");
            Sesion s;
            if (buscar_sesion(id, &s) != 0) {
                printf("No existe.\n");
                pausar();
                continue;
            }
            printf("\nDatos actuales: peli=%d sala=%d %s %s %.2f\n\n",
                   s.pelicula_id, s.sala_id, s.fecha, s.hora, s.precio);
            s.pelicula_id = leer_entero("Nueva pelicula ID: ");
            s.sala_id = leer_entero("Nueva sala ID: ");
            leer_string("Nueva fecha: ", s.fecha, sizeof(s.fecha));
            leer_string("Nueva hora: ", s.hora, sizeof(s.hora));
            s.precio = leer_float("Nuevo precio: ");

            if (modificar_sesion(&s) == 0) {
                printf("\nModificada.\n");
                escribir_log("Sesion modificada");
            }
            pausar();
        }
        else if (opcion == 4) {
            int id = leer_entero("\nID a borrar: ");
            if (confirmar("Confirmar")) {
                if (borrar_sesion(id) == 0) {
                    printf("Borrada.\n");
                    escribir_log("Sesion borrada");
                }
            }
            pausar();
        }
        else if (opcion == 0) {
            salir = 1;
        }
    }
}

void menu_clientes(void) {
    int salir = 0;
    while (!salir) {
        limpiar();
        printf("\n--- GESTION DE CLIENTES ---\n\n");
        printf("1. Listar clientes\n");
        printf("2. Anyadir cliente\n");
        printf("3. Modificar cliente\n");
        printf("4. Borrar cliente\n");
        printf("0. Volver\n\n");

        int opcion = leer_entero("Opcion: ");

        if (opcion == 1) {
            Cliente clientes[MAX_REGISTROS];
            int n = listar_clientes(clientes, MAX_REGISTROS);
            printf("\n");
            if (n == 0) {
                printf("No hay clientes.\n");
            } else {
                printf("ID  | Nombre                | Email                 | Telefono\n");
                printf("----+-----------------------+-----------------------+-----------\n");
                for (int i = 0; i < n; i++) {
                    printf("%-3d | %-21s | %-21s | %s\n",
                           clientes[i].id, clientes[i].nombre,
                           clientes[i].email, clientes[i].telefono);
                }
                printf("\nTotal: %d\n", n);
            }
            pausar();
        }
        else if (opcion == 2) {
            Cliente c;
            c.id = 0;
            printf("\n");
            leer_string("Nombre: ", c.nombre, sizeof(c.nombre));
            leer_string("Email: ", c.email, sizeof(c.email));
            leer_string("Telefono: ", c.telefono, sizeof(c.telefono));

            if (crear_cliente(&c) == 0) {
                printf("\nCliente anyadido.\n");
                escribir_log("Cliente creado");
            }
            pausar();
        }
        else if (opcion == 3) {
            int id = leer_entero("\nID a modificar: ");
            Cliente c;
            if (buscar_cliente(id, &c) != 0) {
                printf("No existe.\n");
                pausar();
                continue;
            }
            printf("\nDatos actuales: %s | %s | %s\n\n", c.nombre, c.email, c.telefono);
            leer_string("Nuevo nombre: ", c.nombre, sizeof(c.nombre));
            leer_string("Nuevo email: ", c.email, sizeof(c.email));
            leer_string("Nuevo telefono: ", c.telefono, sizeof(c.telefono));

            if (modificar_cliente(&c) == 0) {
                printf("\nModificado.\n");
                escribir_log("Cliente modificado");
            }
            pausar();
        }
        else if (opcion == 4) {
            int id = leer_entero("\nID a borrar: ");
            if (confirmar("Confirmar")) {
                if (borrar_cliente(id) == 0) {
                    printf("Borrado.\n");
                    escribir_log("Cliente borrado");
                }
            }
            pausar();
        }
        else if (opcion == 0) {
            salir = 1;
        }
    }
}

void menu_venta(void) {
    limpiar();
    printf("\n--- VENTA DE ENTRADAS ---\n\n");

    // Mostrar sesiones disponibles
    Sesion sesiones[MAX_REGISTROS];
    int n = listar_sesiones(sesiones, MAX_REGISTROS);
    if (n == 0) {
        printf("No hay sesiones disponibles.\n");
        pausar();
        return;
    }

    printf("Sesiones disponibles:\n\n");
    printf("ID  | Peli | Sala | Fecha       | Hora  | Precio | Vendidas\n");
    printf("----+------+------+-------------+-------+--------+---------\n");
    for (int i = 0; i < n; i++) {
        printf("%-3d | %-4d | %-4d | %-11s | %-5s | %-6.2f | %d\n",
               sesiones[i].id, sesiones[i].pelicula_id, sesiones[i].sala_id,
               sesiones[i].fecha, sesiones[i].hora, sesiones[i].precio,
               sesiones[i].vendidas);
    }

    printf("\n");
    int sesion_id = leer_entero("ID de la sesion: ");

    Sesion s;
    if (buscar_sesion(sesion_id, &s) != 0) {
        printf("Sesion no existe.\n");
        pausar();
        return;
    }

    Entrada e;
    e.id = 0;
    e.sesion_id = sesion_id;
    e.precio = s.precio;

    if (confirmar("Es para un cliente registrado?")) {
        e.cliente_id = leer_entero("ID cliente: ");
    } else {
        e.cliente_id = 0;
    }

    // Fecha actual como timestamp
    time_t ahora = time(NULL);
    strftime(e.fecha, sizeof(e.fecha), "%Y-%m-%d %H:%M:%S", localtime(&ahora));

    if (vender_entrada(&e) == 0) {
        printf("\nEntrada vendida correctamente.\n");
        escribir_log("Entrada vendida");
    } else {
        printf("\nNo se pudo vender (sesion llena o error).\n");
    }
    pausar();
}

void vaciar_base_datos(void) {
    limpiar();
    printf("\n--- VACIAR BASE DE DATOS ---\n\n");
    printf("ATENCION: se borraran todos los datos.\n\n");

    if (!confirmar("Seguro?")) {
        pausar();
        return;
    }
    if (!confirmar("De verdad?")) {
        pausar();
        return;
    }

    if (vaciar_db() == 0) {
        printf("\nBase de datos vaciada.\n");
        escribir_log("Base de datos vaciada");
    } else {
        printf("\nError.\n");
    }
    pausar();
}

int pantalla_login(void) {
    char nombre[50];
    char password[50];

    while (1) {
        limpiar();
        printf("\n--- INICIO DE SESION ---\n\n");
        leer_string("Usuario: ", nombre, sizeof(nombre));
        leer_string("Password: ", password, sizeof(password));

        if (login(nombre, password) == 0) {
            printf("\nLogin correcto.\n");
            escribir_log("Login correcto");
            pausar();
            return 0;
        }

        printf("\nUsuario o password incorrectos.\n");
        pausar();
    }
}

void menu_principal(void) {
    int salir = 0;
    while (!salir) {
        limpiar();
        printf("\n========================================\n");
        printf("   GESTION DE CINE\n");
        printf("========================================\n\n");
        printf("Sesion: %s (%s)\n\n", usuario_actual.nombre, usuario_actual.rol);
        printf("1. Peliculas\n");
        printf("2. Salas\n");
        printf("3. Sesiones\n");
        printf("4. Clientes\n");
        printf("5. Venta de entradas\n");
        printf("6. Reportes\n");
        printf("7. Vaciar base de datos\n");
        printf("0. Salir\n\n");

        int opcion = leer_entero("Opcion: ");

        switch (opcion) {
            case 1: menu_peliculas(); break;
            case 2: menu_salas(); break;
            case 3: menu_sesiones(); break;
            case 4: menu_clientes(); break;
            case 5: menu_venta(); break;
            case 6: menu_reportes(); break;
            case 7: vaciar_base_datos(); break;
            case 0: salir = 1; break;
        }
    }
}

void ejecutar_ui(void) {
    if (pantalla_login() != 0) return;
    menu_principal();
    logout();
}