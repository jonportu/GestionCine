#include <iostream>
#include <string>
#include <vector>
#include "conexion.h"
#include "pelicula.h"
#include "sala.h"
#include "sesion.h"
#include "clientecine.h"
#include "entrada.h"
#include "cache.h"

using namespace std;

int main() {
    Conexion con;

    if (!con.conectar("127.0.0.1", 5000)) {
        cout << "No se pudo conectar al servidor" << endl;
        return 1;
    }

    con.enviar("LOGIN|admin|admin123");
    string resp = con.recibir();
    cout << "Login: " << resp << endl;

    Cache cache(con);

    cout << endl << "=== PELICULAS ===" << endl;
    vector<Pelicula> &pelis = cache.getPeliculas();
    if (pelis.empty()) {
        cout << "No hay peliculas" << endl;
    } else {
        for (size_t i = 0; i < pelis.size(); i++) {
            cout << pelis[i].toString() << endl;
        }
    }

    cout << endl << "=== SALAS ===" << endl;
    vector<Sala> &sal = cache.getSalas();
    if (sal.empty()) {
        cout << "No hay salas" << endl;
    } else {
        for (size_t i = 0; i < sal.size(); i++) {
            cout << sal[i].toString() << endl;
        }
    }

    cout << endl << "=== SESIONES ===" << endl;
    vector<Sesion> &ses = cache.getSesiones();
    if (ses.empty()) {
        cout << "No hay sesiones" << endl;
    } else {
        for (size_t i = 0; i < ses.size(); i++) {
            cout << ses[i].toString() << endl;
        }
    }

    cout << endl << "=== CLIENTES ===" << endl;
    vector<ClienteCine> &cli = cache.getClientes();
    if (cli.empty()) {
        cout << "No hay clientes" << endl;
    } else {
        for (size_t i = 0; i < cli.size(); i++) {
            cout << cli[i].toString() << endl;
        }
    }

    cout << endl << "=== SEGUNDA VEZ (desde cache) ===" << endl;
    vector<Pelicula> &pelis2 = cache.getPeliculas();
    cout << "Peliculas: " << pelis2.size() << endl;

    con.enviar("SALIR");
    resp = con.recibir();
    cout << endl << resp << endl;

    con.desconectar();
    return 0;
}