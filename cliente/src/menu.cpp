#include <iostream>
#include <sstream>
#include <cstdlib>
#include "menu.h"
#include "pelicula.h"
#include "sala.h"
#include "sesion.h"
#include "clientecine.h"
#include "entrada.h"

using namespace std;

Menu::Menu(Conexion &conexion, Cache &c) : con(conexion), cache(c) {}

void Menu::limpiar() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Menu::pausar() {
    cout << endl << "Pulsa ENTER para continuar...";
    cin.get();
}

string Menu::leerString(const string &prompt) {
    cout << prompt;
    string linea;
    getline(cin, linea);
    return linea;
}

int Menu::leerEntero(const string &prompt) {
    string linea = leerString(prompt);
    return atoi(linea.c_str());
}

float Menu::leerFloat(const string &prompt) {
    string linea = leerString(prompt);
    return atof(linea.c_str());
}

bool Menu::confirmar(const string &prompt) {
    string linea = leerString(prompt + " (s/n): ");
    return (linea[0] == 's' || linea[0] == 'S');
}

string Menu::enviarYRecibir(const string &comando) {
    con.enviar(comando);
    return con.recibir();
}

bool Menu::pantallaLogin() {
    while (true) {
        limpiar();
        cout << endl << "--- INICIO DE SESION ---" << endl << endl;
        string usuario = leerString("Usuario: ");
        string password = leerString("Password: ");

        string resp = enviarYRecibir("LOGIN|" + usuario + "|" + password);

        if (resp.substr(0, 2) == "OK") {
            cout << endl << "Login correcto." << endl;
            pausar();
            return true;
        }

        cout << endl << "Usuario o password incorrectos." << endl;
        pausar();
    }
}

/* ---- MENU PRINCIPAL ---- */

void Menu::menuPrincipal() {
    int salir = 0;
    while (!salir) {
        limpiar();
        cout << endl;
        cout << "========================================" << endl;
        cout << "   GESTION DE CINE (CLIENTE REMOTO)" << endl;
        cout << "========================================" << endl << endl;
        cout << "1. Peliculas" << endl;
        cout << "2. Salas" << endl;
        cout << "3. Sesiones" << endl;
        cout << "4. Clientes" << endl;
        cout << "5. Venta de entradas" << endl;
        cout << "6. Reportes" << endl;
        cout << "0. Salir" << endl << endl;

        int opcion = leerEntero("Opcion: ");

        switch (opcion) {
            case 1: menuPeliculas(); break;
            case 2: menuSalas(); break;
            case 3: menuSesiones(); break;
            case 4: menuClientes(); break;
            case 5: menuVenta(); break;
            case 6: menuReportes(); break;
            case 0: salir = 1; break;
        }
    }
}

void Menu::menuPeliculas() {
    int salir = 0;
    while (!salir) {
        limpiar();
        cout << endl << "--- GESTION DE PELICULAS ---" << endl << endl;
        cout << "1. Listar peliculas" << endl;
        cout << "2. Anyadir pelicula" << endl;
        cout << "3. Modificar pelicula" << endl;
        cout << "4. Borrar pelicula" << endl;
        cout << "0. Volver" << endl << endl;

        int opcion = leerEntero("Opcion: ");

        if (opcion == 1) {
            vector<Pelicula> &pelis = cache.getPeliculas();
            cout << endl;
            if (pelis.empty()) {
                cout << "No hay peliculas." << endl;
            } else {
                cout << "ID  | Titulo                    | Genero       | Duracion | Edad" << endl;
                cout << "----+---------------------------+--------------+----------+-----" << endl;
                for (size_t i = 0; i < pelis.size(); i++) {
                    cout << pelis[i].toString() << endl;
                }
                cout << endl << "Total: " << pelis.size() << endl;
            }
            pausar();
        }
        else if (opcion == 2) {
            cout << endl;
            string titulo = leerString("Titulo: ");
            string genero = leerString("Genero: ");
            int duracion = leerEntero("Duracion (min): ");
            int edad = leerEntero("Edad minima: ");

            string cmd = "CREAR_PELICULA|" + titulo + "|" + genero + "|" +
                         to_string(duracion) + "|" + to_string(edad);
            string resp = enviarYRecibir(cmd);

            if (resp.substr(0, 2) == "OK") {
                cout << endl << "Pelicula anyadida." << endl;
                cache.invalidarPeliculas();
            } else {
                cout << endl << "Error al anyadir." << endl;
            }
            pausar();
        }
        else if (opcion == 3) {
            int id = leerEntero(string("\nID a modificar: "));

            string resp = enviarYRecibir("BUSCAR_PELICULA|" + to_string(id));
            if (resp.substr(0, 2) != "OK") {
                cout << "No existe." << endl;
                pausar();
                continue;
            }

            Pelicula p;
            p.fromRespuesta(resp.substr(3));
            cout << endl << "Datos actuales: " << p.toString() << endl << endl;

            string titulo = leerString("Nuevo titulo: ");
            string genero = leerString("Nuevo genero: ");
            int duracion = leerEntero("Nueva duracion: ");
            int edad = leerEntero("Nueva edad: ");

            string cmd = "MODIFICAR_PELICULA|" + to_string(id) + "|" + titulo + "|" +
                         genero + "|" + to_string(duracion) + "|" + to_string(edad);
            resp = enviarYRecibir(cmd);

            if (resp.substr(0, 2) == "OK") {
                cout << endl << "Modificada." << endl;
                cache.invalidarPeliculas();
            } else {
                cout << endl << "Error." << endl;
            }
            pausar();
        }
        else if (opcion == 4) {
            int id = leerEntero(string("\nID a borrar: "));

            string resp = enviarYRecibir("BUSCAR_PELICULA|" + to_string(id));
            if (resp.substr(0, 2) != "OK") {
                cout << "No existe." << endl;
                pausar();
                continue;
            }

            Pelicula p;
            p.fromRespuesta(resp.substr(3));
            cout << "Vas a borrar: " << p.getTitulo() << endl;

            if (confirmar("Confirmar")) {
                resp = enviarYRecibir("BORRAR_PELICULA|" + to_string(id));
                if (resp.substr(0, 2) == "OK") {
                    cout << "Borrada." << endl;
                    cache.invalidarPeliculas();
                }
            }
            pausar();
        }
        else if (opcion == 0) {
            salir = 1;
        }
    }
}


void Menu::menuSalas() {
    int salir = 0;
    while (!salir) {
        limpiar();
        cout << endl << "--- GESTION DE SALAS ---" << endl << endl;
        cout << "1. Listar salas" << endl;
        cout << "2. Anyadir sala" << endl;
        cout << "3. Modificar sala" << endl;
        cout << "4. Borrar sala" << endl;
        cout << "0. Volver" << endl << endl;

        int opcion = leerEntero("Opcion: ");

        if (opcion == 1) {
            vector<Sala> &salas = cache.getSalas();
            cout << endl;
            if (salas.empty()) {
                cout << "No hay salas." << endl;
            } else {
                cout << "ID  | Numero | Capacidad" << endl;
                cout << "----+--------+----------" << endl;
                for (size_t i = 0; i < salas.size(); i++) {
                    cout << salas[i].toString() << endl;
                }
                cout << endl << "Total: " << salas.size() << endl;
            }
            pausar();
        }
        else if (opcion == 2) {
            cout << endl;
            int numero = leerEntero("Numero de sala: ");
            int capacidad = leerEntero("Capacidad: ");

            string cmd = "CREAR_SALA|" + to_string(numero) + "|" + to_string(capacidad);
            string resp = enviarYRecibir(cmd);

            if (resp.substr(0, 2) == "OK") {
                cout << endl << "Sala anyadida." << endl;
                cache.invalidarSalas();
            } else {
                cout << endl << "Error (numero duplicado?)." << endl;
            }
            pausar();
        }
        else if (opcion == 3) {
            int id = leerEntero(string("\nID a modificar: "));

            string resp = enviarYRecibir("BUSCAR_SALA|" + to_string(id));
            if (resp.substr(0, 2) != "OK") {
                cout << "No existe." << endl;
                pausar();
                continue;
            }

            Sala s;
            s.fromRespuesta(resp.substr(3));
            cout << endl << "Datos actuales: " << s.toString() << endl << endl;

            int numero = leerEntero("Nuevo numero: ");
            int capacidad = leerEntero("Nueva capacidad: ");

            string cmd = "MODIFICAR_SALA|" + to_string(id) + "|" +
                         to_string(numero) + "|" + to_string(capacidad);
            resp = enviarYRecibir(cmd);

            if (resp.substr(0, 2) == "OK") {
                cout << endl << "Modificada." << endl;
                cache.invalidarSalas();
            }
            pausar();
        }
        else if (opcion == 4) {
            int id = leerEntero(string("\nID a borrar: "));
            if (confirmar("Confirmar")) {
                string resp = enviarYRecibir("BORRAR_SALA|" + to_string(id));
                if (resp.substr(0, 2) == "OK") {
                    cout << "Borrada." << endl;
                    cache.invalidarSalas();
                }
            }
            pausar();
        }
        else if (opcion == 0) {
            salir = 1;
        }
    }
}


void Menu::menuSesiones() {
    int salir = 0;
    while (!salir) {
        limpiar();
        cout << endl << "--- GESTION DE SESIONES ---" << endl << endl;
        cout << "1. Listar sesiones" << endl;
        cout << "2. Anyadir sesion" << endl;
        cout << "3. Modificar sesion" << endl;
        cout << "4. Borrar sesion" << endl;
        cout << "0. Volver" << endl << endl;

        int opcion = leerEntero("Opcion: ");

        if (opcion == 1) {
            cache.invalidarSesiones();
            vector<Sesion> &ses = cache.getSesiones();
            cout << endl;
            if (ses.empty()) {
                cout << "No hay sesiones." << endl;
            } else {
                for (size_t i = 0; i < ses.size(); i++) {
                    cout << ses[i].toString() << endl;
                }
                cout << endl << "Total: " << ses.size() << endl;
            }
            pausar();
        }
        else if (opcion == 2) {
            cout << endl;
            int peliId = leerEntero("ID pelicula: ");
            int salaId = leerEntero("ID sala: ");
            string fecha = leerString("Fecha (YYYY-MM-DD): ");
            string hora = leerString("Hora (HH:MM): ");
            float precio = leerFloat("Precio: ");

            string cmd = "CREAR_SESION|" + to_string(peliId) + "|" + to_string(salaId) +
                         "|" + fecha + "|" + hora + "|" + to_string(precio);
            string resp = enviarYRecibir(cmd);

            if (resp.substr(0, 2) == "OK") {
                cout << endl << "Sesion anyadida." << endl;
                cache.invalidarSesiones();
            } else {
                cout << endl << "Error." << endl;
            }
            pausar();
        }
        else if (opcion == 3) {
            int id = leerEntero(string("\nID a modificar: "));

            string resp = enviarYRecibir("BUSCAR_SESION|" + to_string(id));
            if (resp.substr(0, 2) != "OK") {
                cout << "No existe." << endl;
                pausar();
                continue;
            }

            Sesion s;
            s.fromRespuesta(resp.substr(3));
            cout << endl << "Datos actuales: " << s.toString() << endl << endl;

            int peliId = leerEntero("Nueva pelicula ID: ");
            int salaId = leerEntero("Nueva sala ID: ");
            string fecha = leerString("Nueva fecha: ");
            string hora = leerString("Nueva hora: ");
            float precio = leerFloat("Nuevo precio: ");

            string cmd = "MODIFICAR_SESION|" + to_string(id) + "|" + to_string(peliId) +
                         "|" + to_string(salaId) + "|" + fecha + "|" + hora + "|" + to_string(precio);
            resp = enviarYRecibir(cmd);

            if (resp.substr(0, 2) == "OK") {
                cout << endl << "Modificada." << endl;
                cache.invalidarSesiones();
            }
            pausar();
        }
        else if (opcion == 4) {
            int id = leerEntero(string("\nID a borrar: "));
            if (confirmar("Confirmar")) {
                string resp = enviarYRecibir("BORRAR_SESION|" + to_string(id));
                if (resp.substr(0, 2) == "OK") {
                    cout << "Borrada." << endl;
                    cache.invalidarSesiones();
                }
            }
            pausar();
        }
        else if (opcion == 0) {
            salir = 1;
        }
    }
}


void Menu::menuClientes() {
    int salir = 0;
    while (!salir) {
        limpiar();
        cout << endl << "--- GESTION DE CLIENTES ---" << endl << endl;
        cout << "1. Listar clientes" << endl;
        cout << "2. Anyadir cliente" << endl;
        cout << "3. Modificar cliente" << endl;
        cout << "4. Borrar cliente" << endl;
        cout << "0. Volver" << endl << endl;

        int opcion = leerEntero("Opcion: ");

        if (opcion == 1) {
            vector<ClienteCine> &cli = cache.getClientes();
            cout << endl;
            if (cli.empty()) {
                cout << "No hay clientes." << endl;
            } else {
                for (size_t i = 0; i < cli.size(); i++) {
                    cout << cli[i].toString() << endl;
                }
                cout << endl << "Total: " << cli.size() << endl;
            }
            pausar();
        }
        else if (opcion == 2) {
            cout << endl;
            string nombre = leerString("Nombre: ");
            string email = leerString("Email: ");
            string telefono = leerString("Telefono: ");

            string cmd = "CREAR_CLIENTE|" + nombre + "|" + email + "|" + telefono;
            string resp = enviarYRecibir(cmd);

            if (resp.substr(0, 2) == "OK") {
                cout << endl << "Cliente anyadido." << endl;
                cache.invalidarClientes();
            }
            pausar();
        }
        else if (opcion == 3) {
            int id = leerEntero(string("\nID a modificar: "));

            string resp = enviarYRecibir("BUSCAR_CLIENTE|" + to_string(id));
            if (resp.substr(0, 2) != "OK") {
                cout << "No existe." << endl;
                pausar();
                continue;
            }

            ClienteCine c;
            c.fromRespuesta(resp.substr(3));
            cout << endl << "Datos actuales: " << c.toString() << endl << endl;

            string nombre = leerString("Nuevo nombre: ");
            string email = leerString("Nuevo email: ");
            string telefono = leerString("Nuevo telefono: ");

            string cmd = "MODIFICAR_CLIENTE|" + to_string(id) + "|" + nombre + "|" + email + "|" + telefono;
            resp = enviarYRecibir(cmd);

            if (resp.substr(0, 2) == "OK") {
                cout << endl << "Modificado." << endl;
                cache.invalidarClientes();
            }
            pausar();
        }
        else if (opcion == 4) {
            int id = leerEntero(string("\nID a borrar: "));
            if (confirmar("Confirmar")) {
                string resp = enviarYRecibir("BORRAR_CLIENTE|" + to_string(id));
                if (resp.substr(0, 2) == "OK") {
                    cout << "Borrado." << endl;
                    cache.invalidarClientes();
                }
            }
            pausar();
        }
        else if (opcion == 0) {
            salir = 1;
        }
    }
}


void Menu::menuVenta() {
    limpiar();
    cout << endl << "--- VENTA DE ENTRADAS ---" << endl << endl;

    cache.invalidarSesiones();
    vector<Sesion> &ses = cache.getSesiones();
    if (ses.empty()) {
        cout << "No hay sesiones disponibles." << endl;
        pausar();
        return;
    }

    cout << "Sesiones disponibles:" << endl << endl;
    for (size_t i = 0; i < ses.size(); i++) {
        cout << ses[i].toString() << endl;
    }

    cout << endl;
    int sesionId = leerEntero("ID de la sesion: ");
    int clienteId = 0;

    if (confirmar("Es para un cliente registrado?")) {
        clienteId = leerEntero("ID cliente: ");
    }

    string cmd = "VENDER_ENTRADA|" + to_string(sesionId) + "|" + to_string(clienteId);
    string resp = enviarYRecibir(cmd);

    if (resp.substr(0, 2) == "OK") {
        cout << endl << "Entrada vendida correctamente." << endl;
        cache.invalidarSesiones();
        cache.invalidarEntradas();
    } else {
        cout << endl << "No se pudo vender (sesion llena o error)." << endl;
    }
    pausar();
}


void Menu::menuReportes() {
    int salir = 0;
    while (!salir) {
        limpiar();
        cout << endl << "--- REPORTES ---" << endl << endl;
        cout << "1. Ingresos por pelicula" << endl;
        cout << "2. Ingresos por sala" << endl;
        cout << "3. Ocupacion de salas" << endl;
        cout << "0. Volver" << endl << endl;

        int opcion = leerEntero("Opcion: ");

        if (opcion == 1) {
            con.enviar("REPORTE_INGRESOS_PELI");
            string primera = con.recibir();
            int n = 0;
            size_t pos = primera.find('|');
            if (pos != string::npos) n = stoi(primera.substr(pos + 1));

            cout << endl;
            for (int i = 0; i < n; i++) {
                string linea = con.recibir();
                cout << linea << endl;
            }
            pausar();
        }
        else if (opcion == 2) {
            con.enviar("REPORTE_INGRESOS_SALA");
            string primera = con.recibir();
            int n = 0;
            size_t pos = primera.find('|');
            if (pos != string::npos) n = stoi(primera.substr(pos + 1));

            cout << endl;
            for (int i = 0; i < n; i++) {
                string linea = con.recibir();
                cout << linea << endl;
            }
            pausar();
        }
        else if (opcion == 3) {
            con.enviar("REPORTE_OCUPACION");
            string primera = con.recibir();
            int n = 0;
            size_t pos = primera.find('|');
            if (pos != string::npos) n = stoi(primera.substr(pos + 1));

            cout << endl;
            for (int i = 0; i < n; i++) {
                string linea = con.recibir();
                cout << linea << endl;
            }
            pausar();
        }
        else if (opcion == 0) {
            salir = 1;
        }
    }
}