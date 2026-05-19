#ifndef MENU_H
#define MENU_H

#include "conexion.h"
#include "cache.h"

using namespace std;

class Menu {
private:
    Conexion &con;
    Cache &cache;

    void limpiar();
    void pausar();
    string leerString(const string &prompt);
    int leerEntero(const string &prompt);
    float leerFloat(const string &prompt);
    bool confirmar(const string &prompt);

    string enviarYRecibir(const string &comando);

    void menuPeliculas();
    void menuSalas();
    void menuSesiones();
    void menuClientes();
    void menuVenta();
    void menuReportes();

public:
    Menu(Conexion &conexion, Cache &cache);

    bool pantallaLogin();
    void menuPrincipal();
};

#endif